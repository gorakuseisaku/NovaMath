#pragma once
#include <stdint.h>
#include <string>
enum class OpCode : uint8_t {
#define OPCODE(op) op,
#include "opcode.inc"
#undef OPCODE
};
inline std::string OpCodeToString(OpCode op) {
	switch(op) {
#define OPCODE(op) case(OpCode::op): return #op;
#include "opcode.inc"
#undef OPCODE
	}
}