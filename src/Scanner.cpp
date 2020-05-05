#include "lpp/Scanner.h"
#include <fstream>
#include <sstream>
#include "lpp/ScannerError.h"

namespace lpp{

	std::map<std::string, TokenType> Scanner::keywords{
		{"true", TRUE},  {"false", FALSE},
		{"var", VAR},    {"func", FUNC},
		{"if", IF},      {"else", ELSE},
		{"for", FOR},    {"while", WHILE},
		{"this", THIS},  {"nil", NIL},
		{"or", OR},      {"and", AND},
		{"print", PRINT},{"return", RETURN},
	};

	Scanner::Scanner(const std::string& source)
		: source(source) , lineNumber(1){ tokenize(); }


	void Scanner::skipLine(){
		while(nextChar()!='\n')
			getCurrentAndAdvanceChar();
	}

	void Scanner::number(){
		int offset = 0;
		while( 
			isdigit(source[currentCharIndex+offset]) || 
			source[currentCharIndex+offset] =='.') 
			offset++;
		auto tok = std::string(source.c_str()+currentCharIndex, offset);
		currentCharIndex+=offset-1;
		addToken(NUMBER, tok);
	}

	void Scanner::identifier(){
		std::string id = "";
		do{
			id.push_back(getCurrentAndAdvanceChar());
		}while(isalnum(currentChar()));
		currentCharIndex--;

		if(keywords.find(id) != std::end(keywords))
			addToken(keywords.at(id));
		else
			addToken(IDENTIFIER, id);
	}

	void Scanner::string(){
		std::string	literal ="";
		while( currentChar() != '"'){
			if( end() ) 
				throw ScannerError{ ScannerError::UNTERMINATED_STRING_LITERAL, literal, lineNumber };
			literal.push_back(getCurrentAndAdvanceChar());
		}
		addToken(STRING, literal);
	}

	//match things in front of current char
	bool Scanner::restMatches(std::string rest){
		for(int i = 0; i < (int)rest.length(); i++){
			if(rest[i] != source[1+i+currentCharIndex]) return false;
		}
		if( isalpha(source[1+rest.length()+currentCharIndex]) ||
			isdigit(source[1+rest.length()+currentCharIndex]) ) 
			return false;
		currentCharIndex += rest.length();
		return true;
	};


	void Scanner::tokenize(){
		while(!end()){
			char c = currentChar();
			switch(c){
				// ----------------------- Single char tokens --------------------
				case '(': addToken(LEFT_PAREN); break;
				case ')': addToken(RIGHT_PAREN); break;
				case '{': addToken(LEFT_BRACE); break;
				case '}': addToken(RIGHT_BRACE); break;
				case ',': addToken(COMMA); break;
				case '.': addToken(DOT); break;
				case '-': addToken(MINUS); break;
				case '+': addToken(PLUS); break;
				case ';': addToken(SEMICOLON); break;
				case '*': addToken(STAR); break; 

				// ------------------------ One or two chars -----------------------
				case '!': addToken(restMatches("=") ? BANG_EQUAL : BANG); break;      
				case '=': addToken(restMatches("=") ? EQUAL_EQUAL : EQUAL); break;    
				case '<': addToken(restMatches("=") ? LESS_EQUAL : LESS); break;      
				case '>': addToken(restMatches("=") ? GREATER_EQUAL : GREATER); break;

				// ------------------------ comments -----------------------
				case'/':
					if(nextChar() == '/') skipLine();
					else addToken(SLASH); break;

				//------------------------- whitespace	-----------------------
				case' ':  case'\r': case '\t': break;
				//line counting
				case'\n': lineNumber++; break;
				//eof
				case 0: addToken(END);break;

				// -------------------------- string literals -------------------
				case'"': /* skip "*/getCurrentAndAdvanceChar(); string(); break;

				// ---------- Number literals / Identifiers / Bad chars ---------
				default:
					if      (isdigit(c)) number();
					else if (isalpha(c)) identifier();
					else	throw ScannerError{ ScannerError::UNEXPCETED_CHAR, std::string()+c, lineNumber,};
					break;
			};
			getCurrentAndAdvanceChar();
		}
		addToken(END);
	};

}
