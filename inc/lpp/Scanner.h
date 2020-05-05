#ifndef __SCANNER_H__
#define __SCANNER_H__

#include <variant>
#include <map>
#include <vector>
#include "lpp/Token.h"

/* 
 * Lexer/Scanner - takes in plain text and converts it to a vector of
 * tokens that are accepted by interpreter
 */
namespace lpp{
class Scanner{
	static std::map<std::string, TokenType> keywords;
	std::string source;
	int currentCharIndex = 0;
	int lineNumber = 0;
	std::vector<Token> tokens;

public:
	/* * Scan string * */
	Scanner(const std::string& source);
	/* * Scan whole file * */
	Scanner(const std::ifstream& file);

	inline const std::vector<Token>& getTokens() const{ return tokens; };

private:

	//run through source and tokenize it
	void tokenize();

	//add token to tokens vector 
	inline void addToken(TokenType type, std::string llit)
		{ tokens.push_back(Token{type,lineNumber,llit}); }
	inline void addToken(TokenType type)
		{ tokens.push_back(Token{type,lineNumber, ""}); }

	//check if whole source has been read
	inline bool end(){ return currentCharIndex >= (int)source.length(); };

	bool restMatches(std::string rest);
	// parse number;
	void number();
	// parse identifier
	void identifier();
	// parse string literal;
	void string();
	// skip all until \n;
	void skipLine();

	inline char currentChar() { 
		if(currentCharIndex>=(int)source.length()) 
			return 0;
		return source[currentCharIndex];
	}
	inline char getCurrentAndAdvanceChar() { 
		if(currentCharIndex+1>(int)source.length()) 
			return 0;
		return source[currentCharIndex++];
	}
	inline char nextChar() { 
		if(currentCharIndex+1>=(int)source.length()) 
			return 0;
		return source[currentCharIndex+1];
	}
	inline char prevChar() { 
		if(currentCharIndex-1<0) return 0;
			return source[currentCharIndex-1];
	}
};

}
#endif
