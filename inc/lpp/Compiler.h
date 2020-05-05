#ifndef __COMPILER_H__
#define __COMPILER_H__

#include <algorithm>
#include "lpp/OpCode.h"
#include "lpp/Value.h"
#include "lpp/Tokenizer.h"
#include "lpp/CompilerError.h"
#include "lpp/Chunk.h"

namespace lpp{

	/*
	 * A single pass compiler simultaneously parsing with recursive descent technique
	 * and compiling to bytecode to be executed by VM
	 */
	class Compiler{

		/*
		 * A helper struct to hold local('stack') 
		 * variable identifier and scope depth
		 */
		struct Local{
			Identifier id;
			int scope; //0 = global so >=1
		};

		int currentScope = 0;
		std::vector<Local> locals;
		Tokenizer tk;

	public:
		Compiler(const std::string& src) 
			: tk(src){}
		FunctionObj compile();

	private:
		/*
		 * every chunk should end with return statement
		 * if return statement is not stated in source
		 * compiler will treat that like "return nil;"
		 */
		void appendReturnStmtIfNecessary(){
			if( chunkPtr->code.back() != OP_RET ){
				writePushConst( Value{nil()} , 0);
				write(OP_RET);
			}
		}

		/* -------------------------------------------------------------------------
		 *					 	Variable compiling helpers
		 * -------------------------------------------------------------------------*/

		/*
		 * return -1 if id is not local or >=0( position on stack ) for local variable
		 */
		int resolveLocal(Identifier id);

		/*
		 * get & set checks if var is local(on stack) or global and uses
		 * approriate instructions
		 */
		void setVariable(Identifier id);	
		void getVariable(Identifier id);

		/*
		 * if id is local adds it to locals vector otherwise declares global
		 */
		void declareVariable(Identifier id);

		/* -------------------------------------------------------------------------
		 * 						Chunk access & writing
		 * -------------------------------------------------------------------------*/

		Chunk* chunkPtr;
		inline Chunk& currentChunk(){ return *chunkPtr; }
		void write(OpCode op);
		void write(OpCode op, int line);
		void writePushConst( const Value& val, int line);
		void writeConst(const Value& value);
		void writeConst(const Value& value, int line);

		/* -------------------------------------------------------------------------
		 * 						Jump 
		 * -------------------------------------------------------------------------*/

		/*
		 * writes jumpCode and placeholder for number of instructions to be jumped
		 */
		size_t prepareJump(OpCode jumpCode);

		/* 
		 * writes in code vector at placeholder index difference between 
		 * in case jump is too big to be stored int int8_t error is reported
		 * on token
		 */
		void completeJump(size_t placeholder_index, const Token& token);

		/* -------------------------------------------------------------------------
		 * 						Jump 
		 * -------------------------------------------------------------------------*/

		/*
		 * Called before compiling boody to be looped over, 
		 * returns index of first instruction in the body
		 */
		size_t loopBodyStart();
		/*
		 * writes a jump instruction to the indexOfBodyStart
		 * loopToken is used for reporting too large loop body 
		 */
		void loopToBodyStart(size_t indexOfBodyStart, const Token& loopToken);

		/* -------------------------------------------------------------------------
		 * 						Grammar related methos
		 * -------------------------------------------------------------------------*/

		/*
		 * Notation:
		 *  NAME   -> basic type
		 *  "name" -> literal token
		 * 	name+  -> one or more
		 * 	(name) -> zero or one
		 * 	name*  -> any number
		 *
		 * Grammar:
		 *  whole program: declaration+
		 * 	declaration: varDeclaration | funcDeclaration | statement
		 *
		 * 	varDeclaration:  "var"  IDENTIFIER ( "=" expression ) ";"
		 * 	funcDeclaration: "func" IDENTIFIER ( args ) { statement+ }
		 * 	args: IDENTIFIER | IDENTIFIER , args
		 *
		 * 	statement: ifStatement | forStatement
		 * 	ifStatement:        "if"    "(" expression ")" statement
		 * 	forStatement:       "for"   "(" expression ";" expression ";" expression ")" statement
		 *	whileStatement:     "while" "(" expression ")" statement
		 *	blockStatement:     "{" statement+ "}"
		 *	returnStatement:    "return" (expression) ";"
		 *	expressionStatement: expression ";"
		 *
		 *  expression:     assignment
		 *  assignment:     IDENTIFIER "=" expression ";" | logic
		 *  logic:          comparison ( "and"/"or" comparison )+
		 *  comparison:     addition ( ">" "<" ..  addition)+
		 *  addition:       multiplication ( "+"/"-" multiplication )+
		 *  multiplication: unary ( "&"/"/" unary )+
		 *  unary:          "-"/"!" primary  | call
		 *  call:
		 *  primary:        IDENTIFIER | NUMBER | "(" expression ")"
		 *
		 *
		 *
		 */
		void declaration();
		void varDeclaration();
		void funcDeclaration();

		void statement();
		void ifStatement();
		void forStatement();
		void whileStatement();
		void blockStatement();
		void returnStatement();
		void expressionStatement();

		void expression();
		void assignment();
		void logic();
		void comparison();
		void addition();
		void multiplication();
		void unary();
		void call();
		void primary();
	};


}


#endif
