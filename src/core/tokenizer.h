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
    
    Token readToken();
private:
    void tokenNext();
    void tokenNextSkip(); 
    std::string tokenStr();
    std::string tokenIdent();
    const char* src;    
    uint tokenc = 0;
    uint size = 0;

    char token;
    uint token_pos = 0;
};

#endif
