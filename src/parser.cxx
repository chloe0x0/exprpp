#include <iostream>
#include <vector>
#include <stdlib.h>
#include <cassert>
#include "parser.hxx"
#include "lexer.hxx"

// Uses the Shunting-Yard Algorithm to parse infix mathematical expressions into equivalent postfix
std::vector<Token> ShuntingYard(std::string expression) {
    // The operator Stack
    std::vector<Token> op_stack{};
    // The Output Queue
    std::vector<Token> outq{};
    // Tokenize the input
    Lexer lx = Lexer(expression);
    lx.tokenize();
    std::vector<Token> tokens = lx.get_tokens();
    // reserve space for at least tokens.size() many tokens for the stack and output queue
    op_stack.reserve(tokens.size());
    outq.reserve(tokens.size());

    Token top;

    for (Token t : tokens) {
        switch (t.flag) {
            case Type::lp:
                op_stack.push_back(t);
                break;
            case Type::rp:
                if (op_stack.empty()) {
                    std::cerr << "Unbalanced parantheses in expression!";
                    exit(-1);
                }
                top = op_stack.back();
                while (top.flag != Type::lp) {
                    if (op_stack.empty()) {
                        std::cerr << "Unbalanced parantheses in expression!";
                        exit(-1);
                    }

                    // pop token off the operator stack and onto the output queue
                    outq.push_back(top);
                    op_stack.pop_back();
                    top = op_stack.back();
                }
                // remove the remaining (
                assert(top.flag == Type::lp);
                op_stack.pop_back();
                
                if (!op_stack.empty()) {
                    // check for function
                    top = op_stack.back();
                    if (top.flag == Type::Fun) {
                        // pop into the outq
                        op_stack.pop_back();
                        outq.push_back(top);
                    }
                } 
                
                break;
            case Type::Num:
            case Type::Var:
                outq.push_back(t);
                break;
            case Type::Fun:
                op_stack.push_back(t);
                break;
            case Type::Sum:
            case Type::Sub:
            case Type::Mul:
            case Type::Div:
            case Type::Exp:
            case Type::Neg:
                while (op_stack.size()) {
                    top = op_stack.back();
                    if (!OPERATOR[top.flag])
                        break;

                    uint8_t o2_p = PRECEDENCE[top.flag];
                    uint8_t o1_p = PRECEDENCE[t.flag];
                    Assoc   o1_a = ASSOCIATIVITY[t.flag];

                    if ((o1_p < o2_p && o1_a == Assoc::RIGHT)
                        || (o1_p <= o2_p && o1_a == Assoc::LEFT)) {
                            // pop off the operator stack and onto the output queue
                            op_stack.pop_back();
                            outq.push_back(top);
                    } else {
                        break;
                    }
                }
                // push current node to operator stack
                op_stack.push_back(t);

                break;
            default:
                break;
        }
    }

    while (op_stack.size()) {
        top = op_stack.back();
        op_stack.pop_back();
        if (top.flag == Type::lp) {
            std::cerr << "Unbalanced parentheses!" << std::endl;
            exit(-1);
        }

        assert(OPERATOR[top.flag]);
        // pop to the output queue
        outq.push_back(top);
    }

    return outq;
}

// Parse an Infix mathematical expression into an Expression Tree
Expr_Node* construct_tree(std::string expr) {
    // Convert the expression to postfix
    std::vector<Token> postfix = ShuntingYard(expr);
    // check if expression is empty
    if (postfix.empty())
        return nullptr;
    // Stack of nodes
    std::vector<Expr_Node*> nodes{};

    Expr_Node* node;

    for (Token t : postfix) {
        // Create an expression node
        node = new Expr_Node {
            nullptr, nullptr,
            {},
            t.flag
        };

        switch (t.flag) {
            case Type::Num:
                node->data.val = std::stof(t.lexeme);
                break;
            case Type::Var:
                node->data.id = new std::string {t.lexeme};
                break;
            case Type::Fun:
                node->data.id = new std::string {t.lexeme};
                assert(nodes.size());
                node->left = std::unique_ptr<Expr_Node>(nodes.back());
                nodes.back();
                nodes.pop_back();

                break;
            // Binary Operations
            case Type::Sub:
            case Type::Sum:
            case Type::Mul:
            case Type::Div:
            case Type::Exp:
                // set the left and right children
                assert(nodes.size() >= 2);
                node->right = std::unique_ptr<Expr_Node>(nodes.back());
                nodes.pop_back();
                node->left = std::unique_ptr<Expr_Node>(nodes.back());
                nodes.pop_back();

                break;
            // Unary Operations
            case Type::Neg:
                assert(nodes.size());
                node->left = std::unique_ptr<Expr_Node>(nodes.back());
                nodes.pop_back();

                break;
            default:
                std::cerr << "Invalid token flag: " << t.flag << std::endl;
                exit(-1);
        }

        nodes.push_back(node);
    }

    assert(nodes.size() == 1);
    return nodes.back();
}

Expr_Tree* Parse(std::string expr) {
    Expr_Node* root = construct_tree(expr);
    return new Expr_Tree {root};
}

void infix_to_postfix(std::string infix) {
    std::vector<Token> postfix = ShuntingYard(infix);
    std::cout << "Infix: " << infix << std::endl << "Postfix: ";
    for (Token t : postfix) {
        std::cout << t.lexeme << " ";
    }
    std::cout << std::endl;
}