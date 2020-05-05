#include "lpp/Value.h"

namespace lpp{

	bool boolify( const Value& v ){
		return std::visit( [&](const auto& arg){
			using T = std::decay_t<decltype(arg)>;

			if constexpr (std::is_same_v<T, nil>) {
				return false;
			} else
			if constexpr (std::is_same_v<T, double>) {
				return arg != 0;	
			} else
			if constexpr (std::is_same_v<T, bool>) {
				return arg;
			} else
			if constexpr (std::is_same_v<T, Identifier>) {
				return true;
			} else
			if constexpr (std::is_same_v<T, ObjectRef>) {
				if( v.isObj<Object::STRING>() )
					return  not v.toObj<StringObj>().str.empty();
				 else 
					return true;
			} 
		}, v.value);
	}

	namespace { 
		std::string to_string(double t) { 
			std::string str{ std::to_string(t) };
		   	int offset{ 1 };
		   	if ( str.find_last_not_of('0') == str.find('.') ) 
				offset = 0;
			str.erase( str.find_last_not_of('0') + offset, std::string::npos );
		   	return str; 
		} 
	}

	std::string stringify( const Value& v ){
		return std::visit( [&](const auto& arg){
			using T = std::decay_t<decltype(arg)>;

			if constexpr (std::is_same_v<T, nil>) {
				return std::string("nil");
			} else
			if constexpr (std::is_same_v<T, double>) {
				return to_string(arg);
			} else
			if constexpr (std::is_same_v<T, bool>) {
				return std::string( (arg ? "true" : "false" ) );
			} else
			if constexpr (std::is_same_v<T, Identifier>) {
				return std::string("Id: ") + std::to_string(arg);
			} else
			if constexpr (std::is_same_v<T, ObjectRef>) {
				if( v.isObj<Object::STRING>() )
					return v.toObj<StringObj>().str;
				 else 
					return std::string("Object Reference" );
			} 
		}, v.value);
	}

}
