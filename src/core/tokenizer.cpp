#include "tokenizer.h"

Token Tokenizer::read_token() {
    token_next_skip();

    TokenType type = TokenType::NONE;
    std::string value = "";

    if(token_pos != size) {
	switch(token) {
	case '"': {
	    std::string str = token_str();
	    type = TokenType::STR;
	    value = str;
	}
	    break;
	case '#': {
	    if(tokenc == 0) {
		token_next_skip();
		std::string ident = token_ident();
		if(ident.size() != 0) {
		    type = TokenType::DIRECTIVE;
		    value = ident;	    
		}
		break;
	    }
	}	
	default: {
	    std::string ident = token_ident();
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

void Tokenizer::token_next() {
    if(size == token_pos
	|| token == EOF) return;    
    token = *(src + token_pos);
    token_pos++;    
}

void Tokenizer::token_next_skip() {
    while(1) {
	token_next();	
       
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

std::string Tokenizer::token_str() {
    if(token != '"') {
	token_pos--;
	token_next();
	return "";
    }

    token_next();
    
    std::string str;
    while(1) {
	if(token == EOF
	   || size == token_pos) {	    
	    break;
	}
	
	if(token == '"') {
	    token_next();
	    break;
	}
	
	str += token;
	token_next();
    }
    
    return str;
}

std::string Tokenizer::token_ident() {    
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
	token_next();	
    }

    return ident;
}

