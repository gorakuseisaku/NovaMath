#include "vm.h"
#include "op_func.h"
double VM::Run()
{
	uint8_t lastDst = 0;
	const Instr* const startIp = bin.data();
	const Instr* const endIp = bin.data() + bin.size();
	const Instr* ip = startIp;
	while (ip < endIp) {
		lastDst = ip->a;
		opFuncTable[(uint8_t)ip->op](*ip, *this);
		++ip;
	}
	return regs[lastDst];
}
