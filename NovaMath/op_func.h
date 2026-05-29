#pragma once
#include "instr.h"
#include "vm.h"
#define OPCODE(op) extern void op##Func(const Instr& instr,VM& vm);
#include "opcode.inc"
#undef OPCODE
using OpFunc = void(*)(const Instr&, VM&);
inline constexpr OpFunc opFuncTable[] = {
#define OPCODE(op) op##Func,
#include "opcode.inc"
#undef OPCODE
};