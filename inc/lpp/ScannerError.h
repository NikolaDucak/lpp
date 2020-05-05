#ifndef __SCANNER_ERROR_H__
#define __SCANNER_ERROR_H__

#include "lpp/Token.h"

namespace lpp{

struct ScannerError{
	enum Type{ UNEXPCETED_CHAR, UNTERMINATED_STRING_LITERAL } type;
	std::string str;
	int lineNumber;
};

}



#endif
