#ifndef __LPP_ERROR_FORMATER_H__
#define __LPP_ERROR_FORMATER_H__

#include <string>
#include <sstream>

#include "lpp/ScannerError.h"
#include "lpp/CompilerError.h"
#include "lpp/RuntimeError.h"
namespace lpp{

std::string formatError(ScannerError er);
std::string formatError(CompilerError er);
std::string formatError(RuntimeError er);

}

#endif
