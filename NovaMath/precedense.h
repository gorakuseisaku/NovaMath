#pragma once
#include <stdint.h>
enum class Precedense {
	PREC_NONE = 0,
	PREC_ASSIGN,
	PREC_COMPARE,
	PREC_SUM,
	PREC_PRODUCT,
	PREC_UNARY,
	PREC_CALL
};