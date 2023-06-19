#include "tokenizer.h"

Token Tokenizer::readToken() {
    tokenNextSkip();

    TokenType type = TokenType::NONE;
    std::string value = "";

    if(token_pos != size) {
	switch(token) {
	case '"': {
	    std::string str = tokenStr();
	    type = TokenType::STR;
	    value = str;
	}
	    break;
	case '#': {
	    if(tokenc == 0) {
		tokenNextSkip();
		std::string ident = tokenIdent();
		if(ident.size() != 0) {
		    type = TokenType::DIRECTIVE;
		    value = ident;	    
		}
		break;
	    }
	}	
	default: {
	    std::string ident = tokenIdent();
	    if(ident.size() != 0) {
		type = TokenType::IDENT;
		value = ident;	    
	    }
	}
	}
    }

    Token token;
    token.type = type;
    token.value = value;
    tokenc++;

    return token;
}

void Tokenizer::tokenNext() {
    if(size == token_pos
	|| token == EOF) return;    
    token = *(src + token_pos);
    token_pos++;    
}

void Tokenizer::tokenNextSkip() {
    while(1) {
	tokenNext();	
       
	if(token_pos == size
	    || token == EOF) {
	    break;
	}
	
	if(token != ' '
	   && token != '\n'
	   && token != '\t'
	   && token != '\r') {
	    break;
	}
    }
}

std::string Tokenizer::tokenStr() {
    if(token != '"') {
	token_pos--;
	tokenNext();
	return "";
    }

    tokenNext();
    
    std::string str;
    while(1) {
	if(token == EOF
	   || size == token_pos) {	    
	    break;
	}
	
	if(token == '"') {
	    tokenNext();
	    break;
	}
	
	str += token;
	tokenNext();
    }
    
    return str;
}

std::string Tokenizer::tokenIdent() {    
    std::string ident;
    while(1) {
	if(size == token_pos
	    || token == EOF) {
	    break;
	}
	
	if(token == ' '
	   || token == '\n'
	   || token == '\t'
	   || token == '\r') {
	    break;	       
	}

	ident += token;
	tokenNext();	
    }

    return ident;
}

