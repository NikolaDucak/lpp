#ifndef __VALUE_H__
#define __VALUE_H__

#include <variant>
#include <memory>
#include <iostream>

#include "lpp/Object.h"

namespace lpp{

	// Equivalent to null
	using nil = std::monostate; 

	// String of identifiers are hashed to eliminate need for operations 
	using Identifier = size_t;  

	// shared pointer to Object struc to allow for polymorphism 
	using ObjectRef = std::shared_ptr<Object>;

	// tagged union of all possible types of values lpp works with
	using SumType = std::variant<nil,bool,double,Identifier,ObjectRef> ;

	struct Value{
		SumType value;

		template<Object::Type T>
		bool isObj() const {
			if( !is<ObjectRef>() ) 
				return false;
			return to<ObjectRef>()->type == T;
		}
		
		template<class T>
		const T& toObj() const {
			return *(T*)to<ObjectRef>().get();
		}

		template<class T>
		T& toObj() {
			return *(T*)to<ObjectRef>().get();
		}

		template<typename T>
		bool is() const {
			return std::holds_alternative<T>(value);
		}

		template<typename T>
		const T& to() const {
			return std::get<T>(value);
		}
	};

	/*
	 * based on type of value & value itself convert it to bool
	 * number    : 0 = false; non 0  = true
	 * string    : ""= false; non "" = true
	 * nil       : false;
	 * ObjectRef : true
	 * Identifier: true; 
	 */
	bool boolify( const Value& v );

	/*
	 * string representation of value
	 */
	std::string stringify( const Value& v );

	/*
	 * Functions callable from lpp code  implemented in lpp sourcecode 
	 */
	using NativeFunctionImpl = std::function< Value(std::vector<Value>) >;
	struct NativeFunction: public Object{
		NativeFunctionImpl implementation;
		NativeFunction( const NativeFunctionImpl& impl ) 
			: Object(NATIVE_FUNCTION), implementation(impl){}
	};
}

#endif
