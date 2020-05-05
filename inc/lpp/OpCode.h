#ifndef __OP_CODE_H__
#define __OP_CODE_H__

#include <string>

namespace lpp{

	enum OpCode: int8_t{

		// call * return op
		OP_CALL,			// peek num of args from top
		OP_RET,				// pop numOfArs + identifier

		// binary ops
		// arithemtic ops
		OP_ADD, OP_SUB,
		OP_DIV, OP_MLT,      // pop 2, push 1 
		// comparison op
		OP_LESS, OP_GREATER, // pop 2, push 1 
		// comparison op
		OP_OR, OP_AND,       // pop 2, push 1

		// unary ops
		OP_NEG, OP_NOT, 	 // pop 1, push 1

		//stack ops
		OP_PUSH, OP_POP,

		//var decl;
		OP_DECL_GLOB, // pop 1(identifier), peek top

		//global var
		OP_GLOBAL_GET, // pop 1(id), push 1(val)
		OP_GLOBAL_SET, // pop 1(id), peek top

		// local var
		OP_LOCAL_GET, // pop 1(id), push 1(val)
		OP_LOCAL_SET, // pop 1(id), peek top

		// jump
		OP_JMP, 
		OP_JMP_IF,  // pop 1(condition)
		OP_JMP_IF_NOT,// pop 1(condition)
	};

	std::string opCodeToString(OpCode);

}

#endif
