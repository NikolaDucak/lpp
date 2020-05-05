#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <variant>
#include <string>
namespace lpp{

enum TokenType  {
	LEFT_PAREN, RIGHT_PAREN, 
	LEFT_BRACE, RIGHT_BRACE,

	// Comparison
	BANG,    BANG_EQUAL,                                
	EQUAL,   EQUAL_EQUAL,                              
	GREATER, GREATER_EQUAL,                          
	LESS,    LESS_EQUAL,                                

	// Declarations.
	VAR, FUNC, CLASS, 

	// Literals.                                     
	IDENTIFIER, STRING, NUMBER,                      
	TRUE, FALSE, NIL, 

	//Arithemtic operators
	PLUS, MINUS, STAR, SLASH,

	//Bool related,
	AND, OR,  

	//loops and flow controls
	IF, ELSE, 
	FOR, WHILE,    
	
	//printing 
	PRINT, 

	//other	
	THIS, SUPER, RETURN, 
	DOT, COMMA, SEMICOLON,

	// End of file.
	END,
	NUMBER_OF_TOKENS = 70
};


std::string tokenTypeString(TokenType tokenType);
const char* tokenTypeCharPtr(TokenType tokenType);

struct Token{
	const TokenType type;
	const int lineNumber;
	const std::string src;
};

}

#endif
