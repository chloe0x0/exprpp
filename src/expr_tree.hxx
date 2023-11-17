#ifndef EXPR_TREE_H
#define EXPR_TREE_H
#define _USE_MATH_DEFINES // For constants used in the stdlib
#include <string>
#include <unordered_map>
#include <math.h>   // for STD_CONSTS/ STD_FNS
#include <memory>
#include "token.hxx"

// Literal value can be either variable name or a numeric literal
union data_t {
    float val;
    std::string* id;
};

// Expression Tree Node
struct Expr_Node {
    // Children
    std::unique_ptr<Expr_Node> left;
    std::unique_ptr<Expr_Node> right;
    // Data stored in the node
    data_t data;
    // Type of the expression node
    Type flag;
};

// Deep-Copy a subtree
inline Expr_Node* copy_subtree(Expr_Node* root) {
    Expr_Node* out = new Expr_Node;

    if (root->left != nullptr) {
        out->left = std::unique_ptr<Expr_Node>(copy_subtree(&*(root->left)));
    } if (root->right != nullptr) {
        out->right = std::unique_ptr<Expr_Node>(copy_subtree(&*(root->right)));
    }  

    out->data = root->data;
    out->flag = root->flag;

    return out;
}

// Return a boolean indicating whether an expression is a constant. Used during simplification and differentiation
bool constant_subtree(Expr_Node*,std::unordered_map<std::string,float>);

// Get a subtree expression as a infix mathematical expression
std::string subtree_infix(Expr_Node*);

// typedef for readability
typedef float (* function)(float);

// Standard library of functions and constants that can be loaded into any Expr_Tree
static std::unordered_map<std::string, function> STD_FNS = { 
    // Triginometric Functions
    {"sin", &sinf}, {"cos", &cosf}, {"tan", &tanf},
    {"sinh", &sinhf}, {"cosh", &cosh}, {"tanh", &tanhf},
    // Natural Logarithm
    {"log", &logf}, {"ln", &logf},
    // Base 2 and 10 logs
    {"log10", &log10f}, {"log2", &log2f},
    // Exp function
    {"exp", &expf},
    // Floor and Ceil
    {"floor", &floorf}, {"ceil", &ceilf},
    // Absolute value
    {"abs", &fabs},
    // Square/ Cube root
    {"sqrt", &sqrtf}, {"cbrt", &cbrtf},
};

static std::unordered_map<std::string, float> STD_CONSTS = {
    {"pi", M_PI}, {"e", M_E}
};

class Expr_Tree {
    // The root node of the expression tree
    std::unique_ptr<Expr_Node> root;
    // Hashmap between constant names and assigned values
    std::unordered_map<std::string, float> constants;
    // Hashmap between variable names and assigned values
    std::unordered_map<std::string, float> vars;
    // Hashmap between function names and function pointers
    std::unordered_map<std::string, function> fns;
    // Private methods used in the expression simplifier
    Expr_Node* fold_constant_subtrees(Expr_Node*);
    Expr_Node* simplify_binary_operation(Expr_Node*);
    public:
        Expr_Tree(Expr_Node* root) {
            this->root = std::unique_ptr<Expr_Node>(root);
            this->constants = std::unordered_map<std::string, float>{};
            this->fns = std::unordered_map<std::string, function>{};
            this->vars = std::unordered_map<std::string, float>{};
        }
        Expr_Tree(Expr_Node* root, std::unordered_map<std::string, float> ctx, std::unordered_map<std::string, function> fns) {
            this->root = std::unique_ptr<Expr_Node>(root);
            this->constants = ctx;
            this->fns = fns;
        }
        // set a constant value in the constants hashmap
        inline void set_const(std::string id, float val) {
            this->constants[id] = val;
        }
        inline void set_fun(std::string id, function f) {
            this->fns[id] = f;
        }
        inline void set_var(std::string id, float val) {
            this->vars[id] = val;
        }
        inline void set_constants(std::unordered_map<std::string, float> new_ctx) {
            this->constants = new_ctx;
        }
        inline void set_fns(std::unordered_map<std::string, function> new_fun) {
            this->fns = new_fun;
        }
        inline std::unique_ptr<Expr_Node>* get_root() {
            return &this->root;
        }
        inline void load_stdlib() {
            this->set_constants(STD_CONSTS);
            this->set_fns(STD_FNS);
        }
        // Evaluates the expression
        float eval_(Expr_Node*);
        float eval();
        // Compile expression to LaTeX
        std::string latex_(Expr_Node*, int);
        std::string latex(int);
        // Simplify the expression
        Expr_Node* simplify_(std::unique_ptr<Expr_Node>*);
        Expr_Tree* simplify();
};

#endif /* End of Expr Tree implementation*/