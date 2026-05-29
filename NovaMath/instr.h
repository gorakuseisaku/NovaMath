#pragma once
#include "opcode.h"
#include <stdint.h>
struct Instr {
	OpCode op;
	uint16_t a;
	uint16_t b;
	uint16_t c;
};