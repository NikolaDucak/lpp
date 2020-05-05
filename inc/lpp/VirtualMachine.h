#ifndef __VIRTUAL_MACHINE_H__
#define __VIRTUAL_MACHINE_H__

#include <unordered_map>
#include <iostream>
#include "lpp/OpCode.h"
#include "lpp/Value.h"
#include "lpp/Stack.h"
#include "lpp/RuntimeError.h"
#include "lpp/Chunk.h"

namespace lpp{

	class VirtualMachine{
		/*
		 * save the state of machine before call
		 */
		struct CallFrame{
			std::vector<OpCode>::const_iterator returnIp;
			Chunk* returnChunk;
			int stackStart;
		};

		Stack<Value> valueStack;
		Stack<CallFrame> callStack;
		std::unordered_map<Identifier, Value> globals;		
		std::vector<OpCode>::const_iterator ip;	
		std::deque<Value>::iterator currentStack;
		size_t currentStackStart = 0;
		void a(){
		}

	public:
		void execute( const FunctionObj& );
		void defineNative( const std::string identifier, NativeFunctionImpl impl );

	private:
		Chunk* currentChunk;
		OpCode read();
		void run();

		/*
		 * Helper used for accessing variables
		 */
		Identifier getID();

		/*
		 * Operation execution helpers
		 */
		void binaryOp(OpCode op);
		void logicOp(OpCode op);
		void negateOp();
		void notOp();
		void bang();
		void call();
		void returnFromCall(Value returnValue);
		void jump(bool condition);
	};

}

#endif
