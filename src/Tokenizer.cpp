#include "lpp/Tokenizer.h"

namespace lpp{

Tokenizer::Tokenizer(const std::string& src) 
	: tokens( Scanner(src).getTokens() ), currentTokenIndex(0){ }


Token& Tokenizer::currentToken() { 
	return tokens[currentTokenIndex]; 
}

Token& Tokenizer::nextToken(){ 
	if( currentTokenIndex == tokens.size() - 1 )
		return tokens[currentTokenIndex];
	return tokens[currentTokenIndex+1]; 
}

void Tokenizer::moveToNextToken(){
	if( currentTokenIndex < tokens.size() - 1 )
		currentTokenIndex++;
}

bool Tokenizer::currentTokenIs(std::initializer_list<TokenType> types){
	if( types.size() == 0)
		return false;
	for(const auto& type : types){
		if(currentToken().type == type)
			return true;
	}
	return false;
}

bool Tokenizer::nextTokenIs(std::initializer_list<TokenType> types){
	if( types.size() == 0)
		return false;
	for(const auto& type : types){
		if(nextToken().type != type)
			return false;
	}
	return true;
}

void Tokenizer::expect(TokenType t){
	auto temp = currentToken();
	auto temp2 = Token{ t, temp.lineNumber, ""};
	if(currentToken().type != t)
		throw CompilerError{CompilerError::EXPECTED_TOKEN_NOT_FOUND, temp2 };
}

void Tokenizer::expectAndConsume(TokenType t){
	expect(t);
	moveToNextToken();
}

}
