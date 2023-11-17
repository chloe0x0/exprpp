#ifndef LEXER_H_
#define LEXER_H_

#include <string>
#include <vector>
#include "token.hxx"

class Lexer {
    // index into the current string being tokenized
    uint32_t index;  
    // the current character stream being tokenized
    std::string string; 
    // the current vector of tokens
    std::vector<Token> tokens;
    public:
        // consume the current character and return it, iterating the Lexer's index
        inline char get(){ return this->string[this->index++]; };
        // peek the current character and return it, does not iterate the Lexer's index
        inline char peek(){ return this->string[this->index]; };
        // peek at the previous character
        inline char prev(){ return this->string[this->index - 1]; }
        // Helper function for getting the current vector of tokens
        inline std::vector<Token> get_tokens(){ return this->tokens; }
        // Move the lexer back one character
        inline void back(){ this->index--; }
        // scan a numeric literal
        void consume_number();
        // scan an identifier
        void consume_identifier();
        // Scan through the target string to accumulate a single token
        void scan();
        // Scan through the entire target
        void tokenize();
        // Constructor for a Lexer given a target string
        Lexer(std::string target) {
            string = target;
            index = 0;
            tokens = std::vector<Token>{};
        }
};

// Utility function for printing the current vector of Lexical Tokens from a given Lexer
void display_tokens(Lexer*);

#endif /* End of Lexer header */