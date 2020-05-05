#ifndef __LPP_COMPILER_ERROR_H__
#define __LPP_COMPILER_ERROR_H__

#include "lpp/Token.h"
namespace lpp{

	struct CompilerError{
		enum Type{
			LOCAL_FUNC_DEFINTION,
			UNEXPECTED_TOKEN,
			EXPECTED_TOKEN_NOT_FOUND, 
			TOO_LARGE_JUMP_OFFSET
		} type;
		Token token;
	};

}

#endif
