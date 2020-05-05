#include "lpp/NativeFunctions.h"

namespace lpp{
	namespace { 
		template <typename T> 
		std::string to_string(const T& t) { 
			std::string str{std::to_string (t)};
		   	int offset{1};
		   	if (str.find_last_not_of('0') == str.find('.')) 
				{ offset = 0; } 
			str.erase(str.find_last_not_of('0') + offset, std::string::npos);
		   	return str; 
		} 
	}

	Value println( std::vector<Value> args){
		for(const auto& arg : args){
			std::cout << stringify( arg ) << std::endl;
		}
		return Value{nil()};
	}

}
