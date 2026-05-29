#pragma once
#include <vector>
#include <stdint.h>
#include "opcode.h"
#include "instr.h"
class CodeEmitter {
public:
	CodeEmitter() = default;
	void Emit4(OpCode op, uint16_t a = 0, uint16_t b = 0, uint16_t c = 0) {
		code.push_back(Instr{ op,a,b,c });
	}
	std::vector<Instr> code;
};