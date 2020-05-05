#ifndef __LPP_OBJECT_H__
#define __LPP_OBJECT_H__

#include <string>
#include <functional>

namespace lpp{

	struct Object{
		enum Type { STRING, FUNCTION, NATIVE_FUNCTION } type;
		Object(Type t) : type(t){}
	};

	struct StringObj : public Object{
		std::string str;
		StringObj(const std::string& src) : Object(STRING), str(src){}
	};

	struct Chunk;

	struct FunctionObj : public Object{
		Chunk* chunk;
		uint8_t numOfArgs;
		FunctionObj() : Object(FUNCTION){}
		FunctionObj(Chunk* c, int i) : Object(FUNCTION), chunk(c), numOfArgs(i){}
	};

}
#endif
