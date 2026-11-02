#pragma once
#include <vector>
#include <stdint.h>
#include "opcode.h"
#include "instr.h"
class CodeEmitter {
public:
	CodeEmitter() = default;
	size_t Emit4(OpCode op, uint16_t a = 0, uint16_t b = 0, uint16_t c = 0) {
		size_t size = code.size();
		code.push_back(Instr{ op,a,b,c });
		return size;
	}
	uint32_t GetCurrentIp() const {
		return static_cast<uint32_t>(code.size());
	}
	std::vector<Instr> code;
};