#pragma once
#include "instr.h"
#include "vm.h"
#include "vm_ctx.h"
#define OPCODE(op) extern void op##Func(const Instr& instr,VMCtx& ctx,VM& vm);
#include "opcode.inc"
#undef OPCODE
using OpFunc = void(*)(const Instr&,VMCtx&, VM&);
inline constexpr OpFunc opFuncTable[] = {
#define OPCODE(op) op##Func,
#include "opcode.inc"
#undef OPCODE
};