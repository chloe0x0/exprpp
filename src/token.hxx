#ifndef TOKEN_H_
#define TOKEN_H_

#include <string>
#include <sstream>

enum Type {
    Num, Var, Fun,  // numeric literal, variable, function
    lp, rp, // left and right parentheses
    Sum, Sub, Div, Mul, Exp, Neg, // Operators
};

// Enumerated type for the associativity of an operator
enum Assoc {
    LEFT, 
    NONE,
    RIGHT,
};

// Lookup table for printing Types as strings
const std::string TYPE_STR[11] = {
    "Num", "Var", "Fun",
    "lp", "rp", "Sum",
    "Sub", "Div", "Mul",
    "Exp", "Neg"
};

// Lookup table to get the precedence of an implemented Operator
const uint8_t PRECEDENCE[11] = {
    // Not operators
    0, 0, 0, 
    0, 0,

    // Operators

    2, 2, // Addition, Subtraction
    3, 3, // Division, Multiplication
    4,    // Exponentiation
    5,    // Unary Negation
};

// Lookup table for the associativity of an operator
const Assoc ASSOCIATIVITY[11] = {
    // Not operators
    Assoc::NONE, Assoc::NONE, Assoc::NONE,
    Assoc::NONE, Assoc::NONE,
    
    // Operators
    Assoc::LEFT, Assoc::LEFT, // Addition, Subtraction
    Assoc::LEFT, Assoc::LEFT, // Division, Multiplication
    Assoc::RIGHT, Assoc::RIGHT// Exponentiation, Unary Negation
};

// Lookup table for telling if a flag is an operator
const bool OPERATOR[11] = {
    false, false, false,
    false, false,

    true, true,
    true, true,
    true, true
};

struct Token {
    Type flag;
    std::string lexeme;
};

// Helper function for printing a Token
std::ostream &operator <<(std::ostream &, const Token&);

#endif /* End of Token Header */