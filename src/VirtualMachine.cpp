#include "lpp/VirtualMachine.h"
#include <functional>

namespace lpp{

	void VirtualMachine::defineNative( const std::string identifier, NativeFunctionImpl impl){
		Identifier id =  std::hash<std::string>{}(identifier);
		globals[id] = Value{ ObjectRef( new NativeFunction{ impl } ) };
	}

	Identifier VirtualMachine::getID(){
		if( !valueStack.top().is<Identifier>() ) 
			throw RuntimeError{ RuntimeError::ACCESS_TO_NON_IDENTIFIER };
		return valueStack.pop().to<Identifier>();
	}

	void VirtualMachine::call(){
		int numOfArgs = read();
		int functionStackStart = valueStack.size() - numOfArgs; 
		// no -1 for stack correction cus +1 bcus stack starts with identfier of callee

		// if called function is native
		if ( valueStack.fromTop( numOfArgs ).isObj<Object::NATIVE_FUNCTION>() ){
			std::vector<Value> args { 
				valueStack.begin() + functionStackStart, 
				valueStack.end()
			};

			Value returnValue = 
				valueStack.fromTop( numOfArgs ) .toObj<NativeFunction>() .implementation(args);

			// 1 for identifier;
			valueStack.clearFrom( functionStackStart - 1);
			// push return value to stack
			valueStack.push(returnValue);
		} else {
			auto& func = valueStack.fromTop( numOfArgs ).toObj<FunctionObj>();
			// save current vm state state
			callStack.push( CallFrame{ ip, currentChunk, functionStackStart } );
			// set new state
			currentChunk = func.chunk;
			ip = currentChunk->code.begin();

		}
		currentStackStart = functionStackStart;
	}

	void VirtualMachine::returnFromCall(Value returnValue){
		ip = callStack.top().returnIp;
		currentChunk = callStack.top().returnChunk;
		valueStack.clearFrom( callStack.top().stackStart - 1 ); // minus 1 for identifier;
		// valueStack.clearFrom( localValueStackStart ); // minus 1 for identifier;
		/*
		 * currentStackStart = callStack.top().oldStackStartIndex;
		 * localStackStart = callStack.top().localValueStackStart;
		 */
		callStack.pop(); 
		if( callStack.empty() )
			currentStackStart = 0;
		else
			currentStackStart = callStack.top().stackStart;
		valueStack.push(returnValue);
	}

	void VirtualMachine::execute(const FunctionObj& func){
		currentChunk = func.chunk;
		ip = currentChunk->code.begin();
		run();
	}

	OpCode VirtualMachine::read(){	
		return *ip++;
	}

	void VirtualMachine::binaryOp(OpCode op){
		double r, l;
		try{
			r = valueStack.pop().to<double>();
			l = valueStack.top().to<double>();
		}catch(const std::bad_variant_access&){
			throw RuntimeError{RuntimeError::NON_NUMBER_IN_ARITHMETIC_OP};
		}

		switch(op){
			case OP_ADD:     valueStack.top() = Value{l+r}; break;
			case OP_SUB:     valueStack.top() = Value{l-r}; break;
			case OP_MLT:     valueStack.top() = Value{l*r}; break;
			case OP_DIV:     
				if( r == 0 ){
					throw RuntimeError{ RuntimeError::DIVISION_BY_ZERO};
				}
				valueStack.top() = Value{l/r}; 
			break;
			case OP_GREATER: valueStack.top() = Value{l>r}; break;
			case OP_LESS:    valueStack.top() = Value{l<r}; break;
			default: break;
		}
	}

	void VirtualMachine::logicOp(OpCode op){
		bool r = boolify( valueStack.pop() );
		bool l = boolify( valueStack.pop() );
		switch(op){
			case OP_AND: valueStack.push( Value{l && r} ); break;
			case OP_OR: valueStack.push( Value{l || r} ); break;
			default:/* maybe throw? but it shouldnt be possible to  get here*/ break;
		}
	}

	void VirtualMachine::notOp(){
		bool value = boolify( valueStack.pop() );
		valueStack.push( { not value} );
	}

	void VirtualMachine::negateOp(){
		if( !valueStack.top().is<double>() ){
			throw RuntimeError{ RuntimeError::NEGATING_NON_NUMBER };
		}	
		double value = valueStack.pop().to<double>();
		valueStack.push( {-value} );
	}

	void VirtualMachine::jump( bool condition ){
		int offset = read(); // read must be called to consume offset stored in op code vector
		if( condition ){
			// offset is difference between index of offset and index of instruction to land on
			ip += offset-1; // if already points to next instruction thats why -1
		}
	}

	void VirtualMachine::run(){
		bool running = true;
		while( running ){
			OpCode op = read();
			switch(op){
				// ------------------ stack ops  --------------------
				case OP_POP:  valueStack.pop(); break;
				case OP_PUSH: valueStack.push( currentChunk->constants[int(read())] ); break;

				// ------------------ binary ops -------------------
				// arithmetic op
				case OP_ADD:     case OP_SUB: 
				case OP_MLT:     case OP_DIV:
				case OP_GREATER: case OP_LESS:
					binaryOp(op);
				break;

				// logic op
				case OP_AND: case OP_OR:
					logicOp(op);
				break;

				// -------------- unary instructions ---------------
				case OP_NEG: negateOp();  break;
				case OP_NOT: notOp();     break;
				
				case OP_RET:
					if( callStack.empty() ) 
						running = false;
					else 
						returnFromCall( valueStack.pop() );
				break;
				case OP_CALL:
					call();
				break;

				 // ----------------- Jump ops --------------------
				case OP_JMP:{
					jump(true);
				}break;
				case OP_JMP_IF:{
					jump( boolify( valueStack.pop() ) );
				}break;
				case OP_JMP_IF_NOT:{
					jump( not boolify( valueStack.pop() ) );
				}break;

				// ---------------- Variable ops -----------------
				case OP_DECL_GLOB: {
					auto id = getID();
					globals[id] = valueStack.top();
				} break;

				case OP_GLOBAL_GET: {
					auto id = getID();
					valueStack.push( globals.at(id) );
				} break;

				case OP_GLOBAL_SET: {
					auto id = getID();
					globals.at( id ) = valueStack.top();
				} break;

				case OP_LOCAL_GET:{
					auto id = getID();
					// localStackStart[id] = valueStack.top();
					valueStack.push( valueStack.fromStart( currentStackStart + id ) );
				}break;
				
				case OP_LOCAL_SET:{
					auto id = getID();
					// localStackStart[id] = valueStack.top();
					valueStack.fromStart( currentStackStart + id ) = valueStack.top();
				}break;

				default: /*unreachable*/ break;
			}
		}
	};

}
