#include "expr.hxx"
#include <math.h>
#include <iostream>
#include <stdio.h>

// Reads an expression from stdin and prints the result
bool read_expr(void) {
    std::string expr;
    std::cout << "expr> ";
    std::getline(std::cin, expr);

    if (expr == "q")
        return false;

    std::unique_ptr<Expr_Tree> tree(Parse(expr));
    tree->load_stdlib();
    std::cout << expr << " = " << tree->eval() << std::endl;

    return true;
}

/*      

*/

int main(void) {
    while (read_expr()) {
        // lol
    }
}