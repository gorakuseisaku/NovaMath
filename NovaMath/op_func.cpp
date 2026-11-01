#include "op_func.h"
#include "builtin.h"
void ADDFunc(const Instr& instr, VM& vm) {
	vm.regs[vm.bp + instr.a] = vm.regs[vm.bp + instr.b] + vm.regs[vm.bp + instr.c];
vm.pc++; 
 }
void SUBFunc(const Instr& instr, VM& vm) {
	vm.regs[vm.bp + instr.a] = vm.regs[vm.bp + instr.b] - vm.regs[vm.bp + instr.c];
vm.pc++; 
 }
void DIVFunc(const Instr& instr, VM& vm) {
	vm.regs[vm.bp + instr.a] = vm.regs[vm.bp + instr.b] / vm.regs[vm.bp + instr.c];
vm.pc++; 
 }
void MULFunc(const Instr& instr, VM& vm) {
	vm.regs[vm.bp + instr.a] = vm.regs[vm.bp + instr.b] * vm.regs[vm.bp + instr.c];
vm.pc++; 
 }
void EXPFunc(const Instr& instr, VM& vm) {
	vm.regs[vm.bp + instr.a] = std::pow(vm.regs[vm.bp + instr.b],vm.regs[vm.bp + instr.c]);
	vm.pc++; 
}
void NEGFunc(const Instr& instr, VM& vm) {
	vm.regs[vm.bp + instr.a] = -vm.regs[vm.bp + instr.b];
vm.pc++; 
 }
void LOAD_CONSTFunc(const Instr& instr, VM& vm) {
	vm.regs[vm.bp + instr.a] = vm.prog.doubleTable[instr.b];
vm.pc++; 
 }
void MOVFunc(const Instr& instr, VM& vm) {
	vm.regs[vm.bp + instr.a] = vm.regs[vm.bp + instr.b];
	vm.pc++; 
}
void PUSHFunc(const Instr& instr, VM& vm) {
	vm.valueStack.push_back(vm.regs[vm.bp + instr.a]);
vm.pc++; 
 }
void ANSFunc(const Instr& instr, VM& vm) {
	vm.answers.push_back(vm.regs[vm.bp + instr.a]);
vm.pc++; 
 }
void CALL_BUILTINFunc(const Instr& instr, VM& vm) {
	int argNum = static_cast<int>(vm.prog.doubleTable[instr.c]);
	std::vector<double> args(argNum);
	for (int i = argNum - 1; i >= 0; --i) {
		args[i] = vm.valueStack.back();
		vm.valueStack.pop_back();
	}
	builtinFunc[instr.b](vm, args);
	vm.regs[vm.bp + instr.a] = args[0];
	vm.pc++; 
}
void DEF_FUNCFunc(const Instr& instr, VM& vm) {
	vm.pc = vm.prog.uint32Table[instr.a];
 }
void CALL_FUNCFunc(const Instr& instr, VM& vm) {
	uint32_t startPos = vm.prog.uint32Table[instr.b];
	vm.callBacks.push_back({ instr.a,vm.pc + 1,vm.bp });
	vm.bp += (MAX_TEMP_REG_NUM + MAX_VAR_REG_NUM);
	int argNum = static_cast<int>(vm.prog.doubleTable[instr.c]);
	for (int i = argNum - 1; i >= 0; --i) {
		vm.regs[vm.bp + MAX_TEMP_REG_NUM + i] = vm.valueStack.back();
		vm.valueStack.pop_back();
	}
	vm.pc = startPos;
 }
void END_FUNCFunc(const Instr& instr, VM& vm) {
	const auto& result = vm.callBacks.back();
	vm.pc = result.backPos;
	vm.callBacks.pop_back();
	double ret = vm.regs[vm.bp + instr.a];
	vm.bp = result.beforeBp;
	vm.regs[vm.bp + result.returnReg] = ret;

}