#include "lpp/ErrorFormater.h"

namespace lpp{

	namespace error{

	};

	const char* ScannerErrorString[]{ 

	};

	const char* RuntimeErrorString[]{ 
		[RuntimeError::DIVISION_BY_ZERO] = "Division by zero",
		[RuntimeError::ACCESS_TO_NON_IDENTIFIER] = "Attempting to use non identifier as indentifier",
		[RuntimeError::NEGATING_NON_NUMBER] = "Unary operand '-' on value thats not a number",
		[RuntimeError::NON_NUMBER_IN_ARITHMETIC_OP] = "A non number in arithemtic binary operation",

	};

	const char* CompilerErrorString[]{ 
		[CompilerError::LOCAL_FUNC_DEFINTION] = "Can't declare function insde a block ",
		[CompilerError::UNEXPECTED_TOKEN] = "Unexpected Token",
		[CompilerError::EXPECTED_TOKEN_NOT_FOUND] = "Expected Token not Found", 
		[CompilerError::TOO_LARGE_JUMP_OFFSET] = "Too large body",
	};

	std::string formatError(ScannerError er){
		std::stringstream ss;
		ss << "Scanning error: " << er.type << 
			"\nat:  "<< er.lineNumber << 
			"\nToken: " << er.str;
		return ss.str();
	}

	std::string formatError(CompilerError er){
		std::stringstream ss;
		ss << "Compiling error: " << CompilerErrorString[er.type] << 
			"\nat line:  "<< er.token.lineNumber << 
			"\nToken: " << tokenTypeString( er.token.type )<< ' ' << er.token.src;
		return ss.str();
	}

	std::string formatError(RuntimeError er){
		std::stringstream ss;
		ss << "RuntimeError error: " << er.type;
		return ss.str();
	}

}
