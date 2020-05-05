#include "lpp/Scanner.h"
#include "lpp/CompilerError.h"

namespace lpp{
/*
 *	A helper class used in compiler used to 
 *	manage tokens and provide some usefull methods
 */
class Tokenizer{
	std::vector<Token> tokens;
	size_t currentTokenIndex;

public:
	Tokenizer(const std::string& src);

	Token& currentToken();
	Token& nextToken();

	void moveToNextToken();

	bool currentTokenIs(std::initializer_list<TokenType>);
	bool nextTokenIs(std::initializer_list<TokenType>);

	void expect(TokenType);
	void expectAndConsume(TokenType);
};

}
