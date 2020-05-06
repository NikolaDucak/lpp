#include "lpp/Compiler.h"

namespace lpp{

	// small helpers
	namespace {
		inline double toNumber(const std::string& str){
			return std::stod(str);
		}

		inline Identifier toIdentifier(const std::string& str){
			return std::hash<std::string>{}(str);	
		}

		inline bool overflowsOpCode( long val ){
			using OpCodeType = std::underlying_type<OpCode>::type;
			return  val > std::numeric_limits<OpCodeType>::max() || 
					val < std::numeric_limits<OpCodeType>::min();
		}
	}

	/* --------------------------------------------------------------------------------------- 
	 *							Access to current chunk
	 * --------------------------------------------------------------------------------------*/

	void Compiler::write(OpCode op){
		currentChunk().code.push_back(op);
	}

	void Compiler::write(OpCode op, int line){
		currentChunk().code.push_back(op);
		currentChunk().lines.push_back(line);
	}

	void Compiler::writeConst(const Value& value){
		currentChunk().code.push_back( 
			OpCode(currentChunk().insertConstant(value) )
		);
	}
	void Compiler::writePushConst( const Value& val, int line){
		write(OP_PUSH, line);	
		writeConst(val);
	}

	void Compiler::writeConst(const Value& value, int line){
		currentChunk().code.push_back( 
			OpCode(currentChunk().insertConstant(value) )
		);
		currentChunk().lines.push_back(line);
	}

	/* --------------------------------------------------------------------------------------- 
	 *							Jump compiling helpers
	 * --------------------------------------------------------------------------------------*/

	size_t Compiler::prepareJump(OpCode jumpCode){
		write(jumpCode);
		write(OpCode(0xff));
		return currentChunk().code.size() - 1;
	}

	void Compiler::completeJump(size_t placeholder_index, const Token& token){
		auto diff = long(currentChunk().code.size()) - placeholder_index;
		if( overflowsOpCode( diff ) )
			throw CompilerError{ CompilerError::TOO_LARGE_JUMP_OFFSET, token };
		currentChunk().code[placeholder_index] =
			OpCode(diff);
	}

	/* --------------------------------------------------------------------------------------- 
	 *							Loop compiling helpers
	 * --------------------------------------------------------------------------------------*/

	size_t Compiler::loopBodyStart(){
		return currentChunk().code.size(); // no minus 1 because we are going back
	};

	/*
	 * i index of first instruction in std::vector<OpCode> of chunk
	 */
	void Compiler::loopToBodyStart(size_t indexOfBodyStart, const Token& token){
		write(OP_JMP);
		auto diff = indexOfBodyStart  - currentChunk().code.size();
		if( overflowsOpCode( diff ) )
			throw CompilerError{ CompilerError::TOO_LARGE_JUMP_OFFSET, token };
		write(OpCode( indexOfBodyStart - currentChunk().code.size() ));
	};

	/* --------------------------------------------------------------------------------------- 
	 *				Variable declaration & accessing compiling - implementation
	 * --------------------------------------------------------------------------------------*/

	int Compiler::resolveLocal(Identifier id){
		// size_t is unsigned so if i go to i>=0 then next i-- makes it a huge positive number & segfaults
		for(size_t i = locals.size(); i > 0; i--){
			if (locals[i-1].id == id)
				return i-1;
		}
		return -1;
	}

	void Compiler::declareVariable(Identifier id){
		if(currentScope == 0){ // global
			write(OP_PUSH); writeConst(Value{id});
			write(OP_DECL_GLOB);
		}else{
			locals.push_back( Local{id,currentScope} );	
		}
	}

	void Compiler::getVariable(Identifier id){
		int onStackFromTop = resolveLocal(id);
		write(OP_PUSH);
		if(onStackFromTop == -1){ // global
			writeConst({id});
			write(OP_GLOBAL_GET);
		}else{
			writeConst(Value{Identifier(onStackFromTop)});
			write(OP_LOCAL_GET);
		}
	}

	void Compiler::setVariable(Identifier id){
		int onStackFromTop = resolveLocal(id);
		write(OP_PUSH);
		if(onStackFromTop == -1){ // global
			writeConst({id});
			write(OP_GLOBAL_SET);
		}else{
			writeConst(Value{Identifier(onStackFromTop)});
			write(OP_LOCAL_SET);
		}
	}

	/* --------------------------------------------------------------------------------------- 
	 *							public methods
	 * --------------------------------------------------------------------------------------*/

	FunctionObj Compiler::compile(){
		FunctionObj f;
		f.chunk = new Chunk();
		chunkPtr = f.chunk;
		while(!tk.currentTokenIs( {END} )){
			declaration();	
		}
		appendReturnStmtIfNecessary();

		return f;
	}

	/* --------------------------------------------------------------------------------------- 
	 *							Decaration compiling - implementation
	 * --------------------------------------------------------------------------------------*/

	void Compiler::declaration(){
		switch( tk.currentToken().type ){
			case VAR:    varDeclaration();  break;
			case FUNC:   funcDeclaration(); break;
			case CLASS:  break;
			default: statement();
		}
	}

	void Compiler::funcDeclaration(){
		// maybe allow local definitions of 
		if( currentScope != 0 )
			throw CompilerError{ CompilerError::LOCAL_FUNC_DEFINTION, tk.currentToken() };

		tk.moveToNextToken();
		tk.expect(IDENTIFIER);
		auto func_id = toIdentifier(tk.currentToken().src);
		tk.moveToNextToken();
		int arg_num = 0;

		currentScope++;
		tk.expectAndConsume(LEFT_PAREN);
		while( !tk.currentTokenIs({RIGHT_PAREN}) ) {
			if ( tk.currentTokenIs({COMMA})) {
				tk.moveToNextToken();
			} else if( tk.currentTokenIs({IDENTIFIER}) ){
				arg_num++;
				auto id = toIdentifier(tk.currentToken().src);
				tk.moveToNextToken();
				declareVariable(id);
			} else {
				throw CompilerError{ CompilerError::UNEXPECTED_TOKEN, tk.currentToken()};
			}
		}
		tk.moveToNextToken(); //skip riht_paren
		currentScope--;

		// create new function object
		FunctionObj* func = new FunctionObj();
		func->numOfArgs = arg_num;
		func->chunk =  new Chunk();

		// set current chunk to be function chunk
		Chunk* originalChunk = chunkPtr;
		chunkPtr = func->chunk;

		// compile code to function chunk
		tk.expect(LEFT_BRACE);
		blockStatement();

		// make sure function has a return statement	
		appendReturnStmtIfNecessary();
		
		// return to original chunk
		chunkPtr = originalChunk;
		
		//write declaration code to original chunk
		write(OP_PUSH); writeConst( Value{ ObjectRef(func)} );	
		write(OP_PUSH); writeConst( Value{func_id} );	
		write(OP_DECL_GLOB);
	}

	void Compiler::varDeclaration(){
		tk.moveToNextToken(); // skip 'var' keyword

		tk.expect(IDENTIFIER);
		auto id = toIdentifier(tk.currentToken().src);
		tk.moveToNextToken();

		// check for intializer
		if(tk.currentTokenIs( {EQUAL} )){
			tk.moveToNextToken();
			expression();;
		}else{ // if no initializer, initialize to nil
			write(OP_PUSH); writeConst({nil()});
		}

		// call declareVaraible which will write correct code
		// for local or global scope deppending on current scope
		declareVariable(id);
		
		tk.expectAndConsume(SEMICOLON);
	}

	/* --------------------------------------------------------------------------------------- 
	 *							Statement compiling - implementation
	 * --------------------------------------------------------------------------------------*/

	void Compiler::statement() {
		switch( tk.currentToken().type ) {
			case IF:           ifStatement();     break;
			case FOR:		   forStatement();    break;
			case WHILE: 	   whileStatement();  break;	
			case RETURN:       returnStatement(); break;
			case LEFT_BRACE:   blockStatement();  break;
			default:           expressionStatement();
		}
	}

	void Compiler::whileStatement(){
		// save token, in case of error it will be used for reporting that error
		auto whileTok = tk.currentToken(); 
		tk.moveToNextToken(); 

		// loop entry
		tk.expectAndConsume( LEFT_PAREN );
		auto loopStart = loopBodyStart(); // save position of loop start
		expression(); // condition
		tk.expectAndConsume( RIGHT_PAREN );

		// juimping over body in case of false condition
		int jumpOverBody = prepareJump(OP_JMP_IF_NOT);
		write(OP_POP); // clear comparison result from stack
		statement(); // main loop body

		// return to the condition
		loopToBodyStart(loopStart, whileTok);
		// land here if condition is false
		completeJump(jumpOverBody, whileTok);
	}

	void Compiler::forStatement(){
		// save token, in case of error it will be used for reporting that error
		auto forTok = tk.currentToken(); 
		tk.moveToNextToken();

		// INIT expression
		tk.expectAndConsume( LEFT_PAREN ); // (
		if( tk.currentTokenIs( {VAR} ) )
			varDeclaration();              // var ...
		else if ( !tk.currentTokenIs( {SEMICOLON} ) ){
			expression();                  // ...
			write(OP_POP);
			tk.expectAndConsume( SEMICOLON );
		}else tk.expectAndConsume( SEMICOLON );  // ;

		// CONDITION expression
		auto loopStart = loopBodyStart();
		size_t skipOverBody = 0;
		if( !tk.currentTokenIs( {SEMICOLON} ) ){
			expression();					// condition
			skipOverBody = prepareJump(OP_JMP_IF_NOT);
		}
		tk.expectAndConsume( SEMICOLON );  // ;

		// INCREMENT expression
		std::vector<OpCode> incrementCode;
		if( !tk.currentTokenIs( {RIGHT_PAREN} ) ){
			// compile to new chunk that 
			// will be apended to bottom of current chunk
			std::vector<OpCode> oldCode = chunkPtr->code;
			chunkPtr->code.clear();
			
			expression();					// increment expr	
			write(OP_POP);
			// save compiled code
			incrementCode = chunkPtr->code;
			chunkPtr->code = oldCode;
			// return to old chunk
		}
		tk.expectAndConsume( RIGHT_PAREN );// )

		// FOR BODY
		// compile body
		statement();
		// append increment code
		
		chunkPtr->code.insert( 
				chunkPtr->code.end(), 
				incrementCode.begin(), incrementCode.end() 
		);
		
		//jump to CONDITION after executing body
		write(OP_JMP);
		write(OpCode( loopStart - currentChunk().code.size() ));
		if( skipOverBody ) // skipOver body == 0 if condition is empty so no jump
			completeJump( skipOverBody, forTok );
	}

	void Compiler::returnStatement(){
		tk.moveToNextToken();
		expression();
		write( OP_RET );
		tk.expectAndConsume( SEMICOLON );
	}

	void Compiler::ifStatement(){
		// save token, in case of error it will be used for reporting that error
		Token if_tok = tk.currentToken();
		tk.moveToNextToken();	

		//compile expression
		tk.expectAndConsume( LEFT_PAREN );
		expression();
		tk.expectAndConsume( RIGHT_PAREN );
		
		//skip to else
		int toElseBlock = prepareJump(OP_JMP_IF_NOT);
		// if block
		statement();

		// if there is 'else'
		if( tk.currentTokenIs({ELSE}) ){
			Token else_tok = tk.currentToken();
			tk.moveToNextToken();
			int skip_else = prepareJump(OP_JMP);
			completeJump(toElseBlock, if_tok);
			statement();
			completeJump(skip_else, else_tok);
		}else{
			completeJump(toElseBlock, if_tok);
		}
	}

	void Compiler::blockStatement(){
		tk.moveToNextToken();
		currentScope++;	
		while( !tk.currentTokenIs( {RIGHT_BRACE, END}) ){
			declaration();
		}
		tk.expectAndConsume( RIGHT_BRACE );
		for(const auto& local : locals){
			if( local.scope == currentScope )
				write(OP_POP);
		}
		locals.erase( std::remove_if( locals.begin(), locals.end(),
				[&](Local& l){ return l.scope == currentScope; }
		), locals.end() );
		currentScope--;	

	}

	// expressionStatement: expression ';'.
	void Compiler::expressionStatement(){
		expression();
		tk.expectAndConsume(SEMICOLON);
		//its gonna leave a value on stack 
		//that can cause a problem if not removed
		write(OP_POP);
	}

	/* --------------------------------------------------------------------------------------- 
	 *							Expression compiling - implementation
	 * --------------------------------------------------------------------------------------*/
	void Compiler::expression(){
		assignment();
	}

	void Compiler::assignment(){
		if(tk.currentTokenIs( {IDENTIFIER} ) && tk.nextTokenIs( {EQUAL} )){
			Identifier id = toIdentifier(tk.currentToken().src);
			tk.moveToNextToken(); tk.moveToNextToken(); // skip equals
			expression();
			setVariable(id);
		}else{
			logic();
		}
	}

	void Compiler::logic(){
		comparison();
		while(tk.currentTokenIs( {OR, AND} )){
			OpCode operation =
				(tk.currentToken().type == OR) ? OP_OR : OP_AND;
			tk.moveToNextToken();
			comparison();	
			write( operation );
		}
	}

	void Compiler::comparison(){
		addition();
		while(true){
			if( tk.currentTokenIs( {GREATER_EQUAL, LESS_EQUAL} ) ){
				OpCode operation =
					(tk.currentToken().type == GREATER_EQUAL) ? OP_LESS : OP_GREATER;
				tk.moveToNextToken();

				addition();

				write( operation );
				write( OP_NOT );
			}else if( tk.currentTokenIs( {GREATER, LESS} ) ){
				OpCode operation =
					(tk.currentToken().type == GREATER) ? OP_GREATER : OP_LESS;
				tk.moveToNextToken();

				addition();

				write( operation );
			}else{
				break;
			}
		}
	}

	void Compiler::addition(){
		multiplication();
		while(tk.currentTokenIs( {PLUS, MINUS} )){
			OpCode operation =
				(tk.currentToken().type == MINUS) ? OP_SUB : OP_ADD;
			tk.moveToNextToken();
			addition();	
			write( operation );
		}
	}

	void Compiler::multiplication(){
		unary();
		while(tk.currentTokenIs( {SLASH, STAR} )){
			OpCode operation =
				(tk.currentToken().type == SLASH) ? OP_DIV : OP_MLT;
			tk.moveToNextToken();
			unary();
			write( operation );
		}
	}

	void Compiler::unary(){
		if(tk.currentTokenIs( {BANG, MINUS} )){
			OpCode operation =
				(tk.currentToken().type == BANG) ? OP_NOT : OP_NEG;
			tk.moveToNextToken();
			call();	
			write( operation );
		}else{
			call();
		}
	}

	void Compiler::call(){
		primary();	
		if( tk.currentTokenIs( {LEFT_PAREN} ) ){
			tk.moveToNextToken();
			int numOfArgs = 0;
			while( true ){
				if(tk.currentTokenIs( {RIGHT_PAREN} )){
					tk.moveToNextToken();
					break;
				}
				expression();
				numOfArgs++;
				if(tk.currentTokenIs( {COMMA} ) ){
					tk.moveToNextToken();
				}
			}
			write(OP_CALL);
			write(OpCode(numOfArgs));
		}
	}


	void Compiler::primary(){
		const Token tok = tk.currentToken();
		tk.moveToNextToken();

		switch(tok.type){
			/*
			 * if token is num/string/bool/nil 
			 * save that value to constants vector in current chunk and
			 * write OP_PUSH folowed by index of said value in constants vector
			 */
			case NUMBER:
				writePushConst( Value{ toNumber(tok.src) }, tok.lineNumber );
			break;

			case STRING:
				writePushConst( Value{ ObjectRef(new StringObj{tok.src}) }, tok.lineNumber );
			break;

			case TRUE: case FALSE: 
				writePushConst( Value{ (tok.type == TRUE) ? true : false }, tok.lineNumber );
			break;

			case NIL:
				writePushConst( Value{ nil() }, tok.lineNumber );
			break;

			/*
			 * if token is identifier call getVariable()
			 * which will handle getting a global or a local variable and pushing it to stack
			 * depending of scope of that identifier
			 * setVariable is handled in assignment() method
			 * so if primary() finds an Identifier its compiling usage of value in variable
			 * but not changing the stored value
			 */
			case IDENTIFIER:
				getVariable( toIdentifier(tok.src) );
			break;

			/*
			 * if token is '(' move past it and compile that expression
			 */
			case LEFT_PAREN:
				expression();	
				tk.expectAndConsume(RIGHT_PAREN);
			break;

			default:
				throw CompilerError{ CompilerError::UNEXPECTED_TOKEN, tok};
		}
	}

}
