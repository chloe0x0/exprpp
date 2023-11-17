#include <iostream>
#include <math.h>
#include <sstream>
#include <iomanip>
#include "expr_tree.hxx"

float Expr_Tree::eval_(Expr_Node* node) {
    switch (node->flag) {
        case Type::Num:
            return node->data.val;
        case Type::Var:
            if (this->vars.find(*node->data.id) != this->vars.end()) {
                return this->vars[*node->data.id];
            } else if (this->constants.find(*node->data.id) != this->constants.end()) {
                return this->constants[*node->data.id];
            }
            // variable is not defined
            std::cerr << "Variable " << *node->data.id <<  " undefined " << std::endl;
            exit(-1);
        case Type::Sum:
            return this->eval_(&*node->left) + this->eval_(&*node->right);
        case Type::Sub:
            return this->eval_(&*node->left) - this->eval_(&*node->right);
        case Type::Mul:
            return this->eval_(&*node->left) * this->eval_(&*node->right);
        case Type::Div:
            return this->eval_(&*node->left) / this->eval_(&*node->right);
        case Type::Neg:
            return -this->eval_(&*node->left);
        case Type::Exp:
            return powf(this->eval_(&*node->left), this->eval_(&*node->right));
        case Type::Fun:
            if (this->fns.find(*node->data.id) == this->fns.end()) {
                // variable is not defined
                std::cerr << "Function " << *node->data.id <<  " undefined " << std::endl;
                exit(-1);
            }
            return this->fns[*node->data.id](this->eval_(&*node->left));
        default:
            std::cerr << "Invalid flag on node. (" << node->flag << ")" << std::endl;
            exit(-1);
    }
}

float Expr_Tree::eval() {
    return this->eval_(&*this->root);
}

std::string Expr_Tree::latex_(Expr_Node* node, int decimals) {
    std::stringstream ss;
    switch (node->flag) {
        case Type::Var:
            return *node->data.id;
        case Type::Num:
            ss << std::fixed << std::setprecision(decimals) << node->data.val;
            return ss.str();
        case Type::Sum:
            return this->latex_(&*node->left, decimals) + " + " + this->latex_(&*node->right, decimals);
        case Type::Sub:
            return this->latex_(&*node->left, decimals) + " - " + this->latex_(&*node->right, decimals);
        case Type::Exp:
            return this->latex_(&*node->left, decimals) + "^{" + this->latex_(&*node->right, decimals) + "}";
        case Type::Mul:
            return this->latex_(&*node->left, decimals) + " *(" + this->latex_(&*node->right, decimals) + ")";
        case Type::Div:
            return "\\frac{" + this->latex_(&*node->left, decimals) + "}{" + this->latex_(&*node->right, decimals) + "}";
        case Type::Neg:
            return "-" + this->latex_(&*node->left, decimals);
        case Type::Fun:
            return *node->data.id + "(" + this->latex_(&*node->left, decimals) + ")";
        default:
            std::cerr << "Invalid flag on node. (" << node->flag << ")" << std::endl;
            exit(-1);
    }
}

// Compile the expression tree to LaTeX
std::string Expr_Tree::latex(int decimals) {
    return this->latex_(&*this->root, decimals);   
}

std::string subtree_infix(Expr_Node* root) {
    switch (root->flag) {
        case Type::Var:
            return *root->data.id;
        case Type::Num:
            return std::to_string(root->data.val);
        case Type::Fun:
            return *root->data.id + "(" + subtree_infix(&*root->left) + ")";
        case Type::Sum:
            return subtree_infix(&*root->left) + " + " + subtree_infix(&*root->right);
        case Type::Sub:
            return subtree_infix(&*root->left) + " - " + subtree_infix(&*root->right);
        case Type::Mul:
            return subtree_infix(&*root->left) + "" + subtree_infix(&*root->right);
        case Type::Exp:
            return "(" + subtree_infix(&*root->left) + ")^(" + subtree_infix(&*root->right) + ")";
        case Type::Div:
            return "(" + subtree_infix(&*root->left) + ")/(" + subtree_infix(&*root->right) + ")";
        case Type::Neg:
            return "-" + subtree_infix(&*root->left);
        default:
            std::cerr << "Invalid flag on node. (" << root->flag << ")" << std::endl;
            exit(-1);
    }
}

/*
    Expression Simplification Algorithm

    let . be a binary operator
        ( . )       ( . )
        /   \       /   \
       L     R     L    (k)
    
    if L is a constant AND R is a constant (no variable nodes)
        let the node = L . R
    if either is a constant, precompute it
    otherwise, recurse into both the left and right to simplify them               

    How to detect if a subtree is a constant?
        it cannot have ANY variable nodes
    
    ^ recursively traverse the subtree, if at any point the node->flag == Type::Var return false
    otherwise, continue recursing. If we encounter nullptrs and we still havent found one return true

    ( * )
    /   \
   L    R

    L OR R is constant and is equal to 0.
        replace the subtree with 
            ( 0 )
    L = 1 =>
        ( R )
    R = 1 =>
        ( L )
*/

bool constant_subtree(Expr_Node* node, std::unordered_map<std::string, float> constants) {
    switch (node->flag) {
        case Type::Var:
            return (constants.find(*node->data.id) != constants.end());
        case Type::Num:
            return true;
        case Type::Neg:
        case Type::Fun:
            // a function can still be constant
            return constant_subtree(&*node->left, constants);
        case Type::Sum:
        case Type::Sub:
        case Type::Mul:
        case Type::Exp:
        case Type::Div:
            return constant_subtree(&*node->left, constants) && constant_subtree(&*node->right, constants);
        default:
            std::cerr << "Invalid node in expression tree for simplification: " << node->flag << std::endl;
            exit(-1);
    }
}

Expr_Node* Expr_Tree::simplify_binary_operation(Expr_Node* root) {
    // check which subtree is constant
    bool left_const, right_const;
    left_const = root->left->flag == Type::Num;
    right_const = root->right->flag == Type::Num;
    // if neither are constant, just simplify the subtrees and return
    if (!left_const && !right_const) {
        return new Expr_Node {
            std::unique_ptr<Expr_Node>(this->simplify_(&root->left)),
            std::unique_ptr<Expr_Node>(this->simplify_(&root->right)),
            {},
            root->flag
        };
    }
    // Get the values of the constant subtrees
    float left,right;
    if      (left_const)  left  = root->left->data.val;
    else if (right_const) right = root->right->data.val;
    // Apply reduction rules which are specific to the operator
    switch (root->flag) {
        case Type::Sub:
        case Type::Sum:
            if (left == 0) {
                // keep the simplified right subtree
                return this->simplify_(&root->right);
            } else if (right == 0) {
                return this->simplify_(&root->left);
            }

            break;
        case Type::Mul:
            if (left == 0 || right == 0) {
                return new Expr_Node {
                    nullptr, nullptr,
                    {0.f},
                    Type::Num
                };
            } else if (left == 1) {
                return this->simplify_(&root->right);
            } else if (right == 1) {
                return this->simplify_(&root->left);
            }
            
            break;
        case Type::Div:
            // division by 1
            if (right == 1) {
                return this->simplify_(&root->left);
            }
            break;
        case Type::Exp:
            // exponent of 0 (x^0 = 1 for all x) and base of 1 (1^x = 1 for all x)
            if (right == 0 || left == 1) {
                return new Expr_Node {
                    nullptr, nullptr,
                    {1.f},
                    Type::Num
                };
            } else if (right == 1) {
                return this->simplify_(&root->left);
            } else if (left == 0) {
                return new Expr_Node {
                    nullptr, nullptr,
                    {0.f},
                    Type::Num
                };
            }
            break;
        default:
            std::cerr << "simplify_binary_operation method expects a binary operation" << std::endl;
            exit(-1);
    }

    return new Expr_Node {
        std::unique_ptr<Expr_Node>(this->simplify_(&root->left)),
        std::unique_ptr<Expr_Node>(this->simplify_(&root->right)),
        {},
        root->flag
    };
}

Expr_Node* Expr_Tree::fold_constant_subtrees(Expr_Node* root) {
    // check which subtrees are constant
    bool left_const, right_const;
    left_const = constant_subtree(&*root->left, this->constants);
    right_const = constant_subtree(&*root->right, this->constants);

    if (left_const && right_const) {
        // The entire subtree is a constant expression which can be precomputed
        return new Expr_Node {
            nullptr, nullptr,
            {this->eval_(root)},
            Type::Num           
        };
    } else if (!left_const && right_const) {
        // The left subtree is not constant, whereas the right subtree is
        float right_subtree_eval = this->eval_(&*root->right);
        // Create a constant node to replace the constant subtree expression
        Expr_Node* right_const = new Expr_Node {
            nullptr, nullptr,
            {right_subtree_eval},
            Type::Num
        };

        return new Expr_Node {
            // simplify the left subtree
            std::unique_ptr<Expr_Node>(this->simplify_(&root->left)),
            std::unique_ptr<Expr_Node>(right_const),
            {},
            root->flag
        };
    } else if (left_const && !right_const) {
        // The right subtree is not constant, whereas the left subtree is
        float left_subtree_eval = this->eval_(&*root->left);
        Expr_Node *left_const = new Expr_Node {
            nullptr, nullptr,
            {left_subtree_eval},
            Type::Num
        };

        return new Expr_Node {
            std::unique_ptr<Expr_Node>(left_const),
            std::unique_ptr<Expr_Node>(this->simplify_(&root->right)),
            {},
            root->flag
        };
    }
    // try to simplify the operand subtrees
    return new Expr_Node {
        std::unique_ptr<Expr_Node>(this->simplify_(&root->left)),
        std::unique_ptr<Expr_Node>(this->simplify_(&root->right)),
        {},
        root->flag
    };
}

Expr_Node* Expr_Tree::simplify_(std::unique_ptr<Expr_Node>* node_ptr) {
    Expr_Node* node = node_ptr->get();

    Expr_Node* new_node;

    switch (node->flag) {
        // Currently no simplification implemented/ possible for Functions, Numeric Literals or Variables
        case Type::Var:
        case Type::Num:
            // return a deep copy of the current node
            return new Expr_Node {
                nullptr, nullptr,
                node->data,
                node->flag
            };
        // Functions and Unary Operators
        case Type::Fun:
        case Type::Neg:
            if (constant_subtree(&*node->left, this->constants)) {
                return new Expr_Node {
                    nullptr, nullptr,
                    {this->eval_(node)}, 
                    Type::Num
                };
            } else {
                // attempt to simplify the interior of the function/ negation call
                Expr_Node* simplified_args = simplify_(&node->left);
                return new Expr_Node {
                    std::unique_ptr<Expr_Node>(simplified_args), nullptr,
                    node->data,
                    node->flag
                };
            }
        // Binary Operations
        case Type::Sum:
        case Type::Sub:
        case Type::Mul:
        case Type::Div:
        case Type::Exp:
            new_node = this->fold_constant_subtrees(node);
            if (new_node->flag == Type::Num) return new_node;
            return simplify_binary_operation(new_node);
        default:
            std::cerr << "Invalid node in expression tree simplification: " << node->flag << std::endl;
            exit(-1);   
    }
}

Expr_Tree* Expr_Tree::simplify() {
    return new Expr_Tree {
        this->simplify_(&this->root),
        this->constants,
        this->fns
    };
}
