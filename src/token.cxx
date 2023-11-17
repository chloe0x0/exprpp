#include "token.hxx"


// Helper function for printing tokens
std::ostream &operator <<(std::ostream &out, const Token &token) {
    out << "Token<Type: " << TYPE_STR[token.flag] << " |";
    out << " Lexeme: " << token.lexeme << ">" << std::endl;
    return out;
}

