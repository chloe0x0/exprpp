#ifndef PARSER_H_
#define PARSER_H_

#include "expr_tree.hxx"
#include <string>
#include <vector>

// Uses the Shunting-Yard Algorithm to parse infix mathematical expressions into equivalent postfix
std::vector<Token> ShuntingYard(std::string);
// Shunting Yard Parser for infix mathematical expressions
Expr_Node* construct_tree(std::string);
Expr_Tree* Parse(std::string);
// Function for displaying postfix conversion of an infix expression
void infix_to_postfix(std::string);

#endif /* End of parser implementation */