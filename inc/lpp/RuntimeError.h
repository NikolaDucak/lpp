#ifndef __LPP_RUNTIMER_ERROR_H__
#define __LPP_RUNTIMER_ERROR_H__

namespace lpp{

	struct RuntimeError{
		enum Type{
			DIVISION_BY_ZERO,
			ACCESS_TO_NON_IDENTIFIER,
			NEGATING_NON_NUMBER,
			NON_NUMBER_IN_ARITHMETIC_OP,
		} type;
	};

}
#endif
