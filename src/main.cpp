#include <cstring>
#include <fstream>
#include <streambuf>
#include <iomanip>

#include "lpp/Compiler.h"
#include "lpp/VirtualMachine.h"
#include "lpp/ScannerError.h"
#include "lpp/RuntimeError.h"
#include "lpp/CompilerError.h"
#include "lpp/ErrorFormater.h"
#include "lpp/NativeFunctions.h"

using namespace lpp;

/*
 * TODO:
 * 		- chainging calls is currently not possible eg returnsCallable()(arg);
 * 		- "break" statement
 * 		- comment stuff
 * 		- better error reporting for scan & runtime error
 * 		- OOP stuff
 * 		- arrays  // make it an object?
 * 		- string manipulation and char acces 
 * 		- accesing to an undefined varaible makes std::map end
 *
 * "Local" functions:
 * 		- currently not allowed, maybe implement necessary things like closures for it in future & enable it
 * 		- it is posible to declare function inside a function and it treats it as global;
 * 			because it is posible do declare func inside a func & call inside one the posibility
 * 			of inner func using vars of outer func is bad, because no closure is implemented 
 * 			& those vars are gonna get popped 
 * 			same thing happends if function is inside any kind of {} block, 
 * 			using local vars inside funcs not defined insde func body is bad
 */

void executeFile( const char * filePath, bool dissasembe );
void dissasembleFile( const char * filePath );

int main(int argc, const char* argv[]){
	if( argc == 2 ){
		executeFile( argv[1], false );
	}else if( argc == 3 && strcmp("d", argv[2]) == 0 ){
		executeFile( argv[1], true );
	}else{
		std::cout << "Usage: lpp filename [d] \n" 
			      << "\td - show dissasebmly & don't execute \n";
	}
}

void printInst( size_t& index, const Chunk& chnk){
	auto& i = chnk.code[index];
	std::cout 
		<< std::right <<  std::setw(3) << index << " |  " 
		<< std::left << std::setw(14) << opCodeToString( i );

  	if( i == OP_PUSH ) {
		index++;
		size_t indexOfConstant = chnk.code[index];
		std::cout << "value: "  << stringify( chnk.constants[ indexOfConstant ] );
	} else if ( i == OP_JMP || i == OP_JMP_IF || i == OP_JMP_IF_NOT ){
		index++;
		size_t offset = chnk.code[index];
		std::cout << "offset: " << static_cast<int>(offset);
	} else if( i == OP_CALL ) {
		index++;
		size_t numOfArgs = chnk.code[index];
		std::cout << "args: " << static_cast<int>(numOfArgs);
	}

	std::cout << '\n';
}

void dissasembleChunk( const Chunk& c ){
	for(size_t i = 0; i < c.code.size(); i++){
		printInst(i, c);
	}
}

void executeFile( const char*  filePath, bool dissasembe){
	std::ifstream file(filePath);
	std::string source { 
		(std::istreambuf_iterator<char>( file )),  
		std::istreambuf_iterator<char>() 
	};

	Compiler c{ source };
	VirtualMachine vm;
	vm.defineNative( "println", println );
	try {
		if( dissasembe ) {
			dissasembleChunk( *c.compile().chunk );
		} else {
			vm.execute( c.compile() );
		}
	}
	catch( const ScannerError&  err ) {  std::cerr <<  formatError(err) << std::endl; }
	catch( const CompilerError& err ) {  std::cerr <<  formatError(err) << std::endl; }
	catch( const RuntimeError&  err ) {  std::cerr <<  formatError(err) << std::endl; }
}
