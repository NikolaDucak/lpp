#include "lpp/OpCode.h"

namespace lpp{

	std::string OpCodeStrings[] = {
		[OP_CALL] = "CALL",
		[OP_RET] = "RET",

		// arithemtic ops
		[OP_ADD]="ADD", [OP_SUB]="SUB",
		[OP_DIV]="DIV", [OP_MLT]="MLT",

		// comparisson op	
		[OP_LESS]="LESS", [OP_GREATER]="GREATER",

		// comparison op
		[OP_OR]="OR", [OP_AND]="AND",

		// negation op
		[OP_NEG] ="NEG", [OP_NOT]="NOT",

		//stack ops
		[OP_PUSH]="PUSH", [OP_POP]="POP",

		//var decl;
		[OP_DECL_GLOB]="DECL_GLOB",

		//global var
		[OP_GLOBAL_GET]="GLOBAL_GET",
		[OP_GLOBAL_SET]="GLOBAL_SET",

		// local var
		[OP_LOCAL_GET]="LOCAL_GET",
		[OP_LOCAL_SET]="LOCAL_SET",

		//jump
		[OP_JMP]="JMP", 
		[OP_JMP_IF]="JMP_IF", 
		[OP_JMP_IF_NOT]="JMP_IF_NOT",
	};

	std::string opCodeToString(OpCode code){
		return OpCodeStrings[code];
	}
}
