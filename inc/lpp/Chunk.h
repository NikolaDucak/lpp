#ifndef __CHUNK_H__
#define __CHUNK_H__

#include <vector>

#include "lpp/Value.h"
#include "lpp/OpCode.h"

namespace lpp{

	struct Chunk{
		std::vector<OpCode> code;
		std::vector<int> lines;
		std::vector<Value> constants;

		size_t insertConstant(const Value& v){
			constants.push_back(v);
			return constants.size() - 1;
		}
	};

}

#endif
