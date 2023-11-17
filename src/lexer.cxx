#include <vector>
#include <iostream>
#include <sstream>
#include "lexer.hxx"

// utility function for telling if a given character is whitespace
bool whitespace(char c) {
    switch ( c ) {
        case ' ':
        case '\n':
        case '\t':
        case '\r':
            return true;
        default:
            return false;
    }
}

// Utility function for printing the current vector of Lexical Tokens from a given Lexer
void display_tokens(Lexer* lexer) {
    for (Token t : lexer->get_tokens()) 
        std::cout << t;
    std::cout << std::endl;
}

void Lexer::consume_number() {
    std::stringstream ss;

    char curr = this->prev();
    ss << curr;
    // until a decimal point
    while (isdigit(curr = this->get())) {
        ss << curr;
    }
    // check for decimal point
    if (curr == '.') {
        ss << curr;
        while (isdigit(curr = this->get())) {
            ss << curr;
        }
    }
    // Add to the list of tokens
    this->tokens.push_back(Token{Type::Num, ss.str()});
    this->back();
}

void Lexer::consume_identifier() {
    std::stringstream ss;
 
    char curr = this->prev();
    ss << curr;
    while (isalpha(curr = this->get())) {
        ss << curr;
    }
    this->back();
    bool is_fun = this->peek() == '(';

    this->tokens.push_back(Token{is_fun ? Type::Fun : Type::Var, ss.str()});
}

void Lexer::scan() {
    char current_character = this->get();
    // ignore whitespace
    if (whitespace(current_character))
        return;

    Type flag;
    std::string lexeme;

    // check the previous token to see if it is a Var or Number
    std::vector<Token> curr_tokens = this->get_tokens();
    uint32_t len = curr_tokens.size();
    Type previous_flag;
    bool is_sub = false;
    bool impl_mul = false;
    if (len != 0) {
        previous_flag = curr_tokens[len - 1].flag;
        is_sub = previous_flag == Type::Var || previous_flag == Type::Num
                                || previous_flag == Type::rp;
        impl_mul = previous_flag == Type::Var || previous_flag == Type::Num
                                || previous_flag == Type::rp;
    }
    
    switch (current_character) {
        // Identifiers
        case 'a' ... 'z':
        case 'A' ... 'Z':
            // if the previous flag was a numeric literal or another variable insert a MUL token (implicit multiplication)
            if (impl_mul) {
                this->tokens.push_back(Token{Type::Mul, "*"});
            }
            this->consume_identifier();
            return;
        // Numeric literal
        case '0' ... '9':
            if (impl_mul) {
                this->tokens.push_back(Token{Type::Mul, "*"});
            }
            this->consume_number();
            return;
        // Operators
        case '+':
            flag = Type::Sum;
            lexeme = "+";
            break;
        // Handle negation AND subtraction
        case '-':
            flag = is_sub ? Type::Sub : Type::Neg;
            lexeme = "-";
            break;
        case '/':
            flag = Type::Div;
            lexeme = "/";
            break;
        case '*':
            flag = Type::Mul;
            lexeme = "*";
            break;
        case '^':
            flag = Type::Exp;
            lexeme = "^";
            break;
        // Parens
        case '(':
            if (impl_mul) {
                this->tokens.push_back(Token{Type::Mul, "*"});
            }
            flag = Type::lp;
            lexeme = "(";
            break;
        case ')':
            flag = Type::rp;
            lexeme = ")";
            break;
        default:
            std::cout << "Unknown character encountered at index: " << this->index << " | Did Not expect: " << current_character << std::endl;
            break;
    }

    this->tokens.push_back(Token{flag, lexeme});
}

void Lexer::tokenize() {
    while (this->index < this->string.length()) {
        this->scan();
    }
}
