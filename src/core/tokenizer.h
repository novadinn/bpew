#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>

enum TokenType {
    STR,
    IDENT,
    DIRECTIVE,
    NONE,    
};

struct Token {
    TokenType type;
    std::string value;
};

struct Tokenizer {
    Tokenizer(const char* src, uint size) : src(src), size(size) {}
    
    Token read_token();
private:
    void token_next();
    void token_next_skip(); 
    std::string token_str();
    std::string token_ident();
    const char* src;    
    uint tokenc = 0;
    uint size = 0;

    char token;
    uint token_pos = 0;
};

#endif
