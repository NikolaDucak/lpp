#include "lpp/Token.h"

namespace lpp{

const char* tokenStrings[NUMBER_OF_TOKENS]{
	// grouping
	[LEFT_PAREN] = "(", [RIGHT_PAREN] = ")",
	[LEFT_BRACE] = "{", [RIGHT_BRACE] = "}",
	
	// logical operators
	[BANG] = "!", [BANG_EQUAL] = "!=",
	[EQUAL] = "=", [EQUAL_EQUAL] = "==",
	[GREATER] = ">", [GREATER_EQUAL] = ">=",
	[LESS] = "<", [LESS_EQUAL] = "<=",

	// declaration keywords
	[VAR] = "var", [FUNC] = "func", [CLASS] = "class",

	//literals.
	[IDENTIFIER] = "identifier", [STRING] = "stringltrl", [NUMBER] = "numberLtrl",
	[TRUE] = "true", [FALSE] = "FALSe", [NIL] = "nil",

	// arithemtic operators;
	[PLUS] = "+", [MINUS] = "-",
	[STAR] = "*", [SLASH] = "/",

	//logical
	[AND] = "and", [OR] = "or",

	//statements
	[IF] = "if", [ELSE] = "else",
	[FOR] = "for", [WHILE] = "while",

	[PRINT] = "print",

	[THIS] = "this", [SUPER] = "super", [RETURN] = "return",

	// other
	[DOT] = ".", [COMMA] = ",", [SEMICOLON] = ";",

	[END] = "0"
};

std::string tokenTypeString(TokenType tokenType){
	return tokenStrings[tokenType];
}
const char* tokenTypeCharPtr(TokenType tokenType){
	return tokenStrings[tokenType];
}

}
