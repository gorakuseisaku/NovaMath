#include "vm.h"
#include "vm_ctx.h"
#include "op_func.h"
#include <cstdio>
void VM::Run()
{
	answers.clear();
	auto ctxIdx = vmCtxPool.Create();
	VMCtx& ctx = vmCtxPool.Get(*ctxIdx);
	const size_t size = bin.size();
	while (ctx.pc < size) {
		Instr instr = bin[ctx.pc];
		opFuncTable[(uint8_t)instr.op](bin[ctx.pc],ctx, *this);
	}
	vmCtxPool.Delete(*ctxIdx);
}

void VM::PrintAnswers()
{
	size_t size = answers.size();
	for (int i = 0; i < size; ++i) {
		std::printf("ans%i = %.15g\n",i, answers[i]);
	}
}

void VM::ExecuteVM(size_t ctxIdx)
{
	VMCtx& ctx = vmCtxPool.Get(ctxIdx);
	const size_t size = bin.size();
	while (!ctx.isFinished && ctx.pc < size) {
		Instr instr = bin[ctx.pc];
		opFuncTable[(uint8_t)instr.op](bin[ctx.pc],ctx, *this);
	}
	vmCtxPool.Delete(ctxIdx);
}
