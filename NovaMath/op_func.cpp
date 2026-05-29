#include "op_func.h"
#include "builtin.h"
void ADDFunc(const Instr& instr, VM& vm) {
	vm.regs[instr.a] = vm.regs[instr.b] + vm.regs[instr.c];
}
void SUBFunc(const Instr& instr, VM& vm) {
	vm.regs[instr.a] = vm.regs[instr.b] - vm.regs[instr.c];
}
void DIVFunc(const Instr& instr, VM& vm) {
	vm.regs[instr.a] = vm.regs[instr.b] / vm.regs[instr.c];
}
void MULFunc(const Instr& instr, VM& vm) {
	vm.regs[instr.a] = vm.regs[instr.b] * vm.regs[instr.c];
}
void NEGFunc(const Instr& instr, VM& vm) {
	vm.regs[instr.a] = -vm.regs[instr.b];
}
void LOAD_CONSTFunc(const Instr& instr, VM& vm) {
	vm.regs[instr.a] = vm.prog.doubleTable[instr.b];
}
void MOVFunc(const Instr& instr, VM& vm) {
	vm.regs[instr.a] = vm.regs[instr.b];
}
void PUSHFunc(const Instr& instr, VM& vm) {
	vm.valueStack.push_back(vm.regs[instr.a]);
}
void CALL_BUILTINFunc(const Instr& instr, VM& vm) {
	int argNum = static_cast<int>(vm.prog.doubleTable[instr.c]);
	std::vector<double> args(argNum);
	for (int i = argNum - 1; i >= 0; --i) {
		args[i] = vm.valueStack.back();
		vm.valueStack.pop_back();
	}
	builtinFunc[instr.b](vm, args);
	vm.regs[instr.a] = args[0];
}