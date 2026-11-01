#include "vm.h"
#include "op_func.h"
void VM::Run()
{
	answers.clear();
	const size_t size = bin.size();
	while (pc < size) {
		Instr instr = bin[pc];
		opFuncTable[(uint8_t)instr.op](bin[pc], *this);
	}
}

void VM::PrintAnswers()
{
	size_t size = answers.size();
	for (int i = 0; i < size; ++i) {
		std::printf("ans%i = %.15g\n",i, answers[i]);
	}
}
