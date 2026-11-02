#include "op_func.h"
#include "builtin.h"
void ADDKRFunc(const Instr& instr,VMCtx& ctx, VM& vm) {
	ctx.regs[instr.a] = vm.prog.doubleTable[instr.b] + ctx.regs[instr.c];
	ctx.pc++; 
}
void ADDRRFunc(const Instr& instr,VMCtx& ctx, VM& vm) {
	ctx.regs[instr.a] = ctx.regs[instr.b] + ctx.regs[instr.c];
	ctx.pc++;
}
void ADDEQRFunc(const Instr& instr, VMCtx& ctx, VM& vm) {
	ctx.regs[instr.a] += ctx.regs[instr.b];
	ctx.pc++;
}
void ADDEQKFunc(const Instr& instr, VMCtx& ctx, VM& vm) {
	ctx.regs[instr.a] += vm.prog.doubleTable[instr.b];
	ctx.pc++;
}
void SUBRRFunc(const Instr& instr,VMCtx& ctx, VM& vm) {
	ctx.regs[instr.a] = ctx.regs[instr.b] - ctx.regs[instr.c];
	ctx.pc++;
}
void SUBRKFunc(const Instr& instr,VMCtx& ctx, VM& vm) {
	ctx.regs[instr.a] = ctx.regs[instr.b] - vm.prog.doubleTable[instr.c];
	ctx.pc++;
}
void SUBKRFunc(const Instr& instr,VMCtx& ctx, VM& vm) {
	ctx.regs[instr.a] = vm.prog.doubleTable[instr.b] - ctx.regs[instr.c];
	ctx.pc++;
}
void SUBEQRFunc(const Instr& instr, VMCtx& ctx, VM& vm) {
	ctx.regs[instr.a] -= ctx.regs[instr.b];
	ctx.pc++;
}
void SUBEQKFunc(const Instr& instr, VMCtx& ctx, VM& vm) {
	ctx.regs[instr.a] -= vm.prog.doubleTable[instr.b];
	ctx.pc++;
}
void DIVRRFunc(const Instr& instr,VMCtx& ctx, VM& vm) {
	ctx.regs[instr.a] = ctx.regs[instr.b] / ctx.regs[instr.c];
	ctx.pc++; 
}
void DIVRKFunc(const Instr& instr,VMCtx& ctx, VM& vm) {
	ctx.regs[instr.a] = ctx.regs[instr.b] / vm.prog.doubleTable[instr.c];
	ctx.pc++;
}
void DIVKRFunc(const Instr& instr,VMCtx& ctx, VM& vm) {
	ctx.regs[instr.a] = vm.prog.doubleTable[instr.b] / ctx.regs[instr.c];
	ctx.pc++;
}
void DIVEQRFunc(const Instr& instr, VMCtx& ctx, VM& vm) {
	ctx.regs[instr.a] /= ctx.regs[instr.b];
	ctx.pc++;
}
void DIVEQKFunc(const Instr& instr, VMCtx& ctx, VM& vm) {
	ctx.regs[instr.a] /= vm.prog.doubleTable[instr.b];
	ctx.pc++;
}
void MULKRFunc(const Instr& instr,VMCtx& ctx, VM& vm) {
	ctx.regs[instr.a] = vm.prog.doubleTable[instr.b] * ctx.regs[instr.c];
	ctx.pc++;
}
void MULRRFunc(const Instr& instr,VMCtx& ctx, VM& vm) {
	ctx.regs[instr.a] = ctx.regs[instr.b] * ctx.regs[instr.c];
	ctx.pc++; 
}
void MULEQRFunc(const Instr& instr, VMCtx& ctx, VM& vm) {
	ctx.regs[instr.a] *= ctx.regs[instr.b];
	ctx.pc++;
}
void MULEQKFunc(const Instr& instr, VMCtx& ctx, VM& vm) {
	ctx.regs[instr.a] *= vm.prog.doubleTable[instr.b];
	ctx.pc++;
}
void EXPRRFunc(const Instr& instr,VMCtx& ctx, VM& vm) {
	ctx.regs[instr.a] = std::pow(ctx.regs[instr.b],ctx.regs[instr.c]);
	ctx.pc++; 
}
void EXPRKFunc(const Instr& instr,VMCtx& ctx, VM& vm) {
	ctx.regs[instr.a] = std::pow(ctx.regs[instr.b], vm.prog.doubleTable[instr.c]);
	ctx.pc++;
}
void EXPKRFunc(const Instr& instr,VMCtx& ctx, VM& vm) {
	ctx.regs[instr.a] = std::pow(vm.prog.doubleTable[instr.b], ctx.regs[instr.c]);
	ctx.pc++;
}
void MODRRFunc(const Instr& instr,VMCtx& ctx, VM& vm) {
	ctx.regs[instr.a] = std::fmod(ctx.regs[instr.b], ctx.regs[instr.c]);
	ctx.pc++;
}
void MODRKFunc(const Instr& instr,VMCtx& ctx, VM& vm) {
	ctx.regs[instr.a] = std::fmod(ctx.regs[instr.b], vm.prog.doubleTable[instr.c]);
	ctx.pc++;
}
void MODKRFunc(const Instr& instr,VMCtx& ctx, VM& vm) {
	ctx.regs[instr.a] = std::fmod(vm.prog.doubleTable[instr.b], ctx.regs[instr.c]);
	ctx.pc++;
}
void LESSRRFunc(const Instr& instr,VMCtx& ctx, VM& vm) {
	ctx.regs[instr.a] = static_cast<double>(ctx.regs[instr.b] < ctx.regs[instr.c]);
	ctx.pc++;
}
void LESSRKFunc(const Instr& instr,VMCtx& ctx, VM& vm) {
	ctx.regs[instr.a] = static_cast<double>(ctx.regs[instr.b] < vm.prog.doubleTable[instr.c]);
	ctx.pc++;
}
void LESSKRFunc(const Instr& instr,VMCtx& ctx, VM& vm) {
	ctx.regs[instr.a] = static_cast<double>(vm.prog.doubleTable[instr.b] < ctx.regs[instr.c]);
	ctx.pc++;
}
void EERKFunc(const Instr& instr,VMCtx& ctx, VM& vm) {
	ctx.regs[instr.a] = static_cast<double>(ctx.regs[instr.b] - vm.prog.doubleTable[instr.c] < 1e-14);
	ctx.pc++;
}
void EERRFunc(const Instr& instr,VMCtx& ctx, VM& vm) {
	ctx.regs[instr.a] = static_cast<double>(ctx.regs[instr.b] - ctx.regs[instr.c] < 1e-14);
	ctx.pc++;
}
void NEGFunc(const Instr& instr,VMCtx& ctx, VM& vm) {
	ctx.regs[instr.a] = -ctx.regs[instr.a];
	ctx.pc++; 
}
void NOTFunc(const Instr& instr,VMCtx& ctx, VM& vm) {
	ctx.regs[instr.a] = (std::abs(ctx.regs[instr.a]) < 1e-14) ? 1.0 : 0.0;
	ctx.pc++;
}
void JZFunc(const Instr& instr,VMCtx& ctx, VM& vm) {
	if (ctx.regs[instr.a] < 1e-14) {
		ctx.pc = vm.prog.uint32Table[instr.b];
		return;
	}
	ctx.pc++;
}
void JMFunc(const Instr& instr,VMCtx& ctx, VM& vm) {
	ctx.pc = vm.prog.uint32Table[instr.a];
}
void INCFunc(const Instr& instr, VMCtx& ctx, VM& vm) {
	double oldValue = ctx.regs[instr.b];
	double newValue = oldValue + 1.0;
	ctx.regs[instr.b] = newValue;
	if (instr.c == 0) {
		ctx.regs[instr.a] = newValue;
	}
	else {
		ctx.regs[instr.a] = oldValue;
	}
	++ctx.pc;
}
void DECFunc(const Instr& instr, VMCtx& ctx, VM& vm) {
	double oldValue = ctx.regs[instr.b];
	double newValue = oldValue - 1.0;
	ctx.regs[instr.b] = newValue;
	if (instr.c == 0) {
		ctx.regs[instr.a] = newValue;
	}
	else {
		ctx.regs[instr.a] = oldValue;
	}
	++ctx.pc;
}
void LOAD_CONSTFunc(const Instr& instr,VMCtx& ctx, VM& vm) {
	ctx.regs[instr.a] = vm.prog.doubleTable[instr.b];
ctx.pc++; 
 }
void MOVRFunc(const Instr& instr,VMCtx& ctx, VM& vm) {
	ctx.regs[instr.a] = ctx.regs[instr.b];
	ctx.pc++; 
}
void MOVKFunc(const Instr& instr,VMCtx& ctx, VM& vm) {
	ctx.regs[instr.a] = vm.prog.doubleTable[instr.b];
	ctx.pc++;
}
void PUSHRFunc(const Instr& instr,VMCtx& ctx, VM& vm) {
	vm.valueStack.push_back(ctx.regs[instr.a]);
	ctx.pc++; 
}
void PUSHKFunc(const Instr& instr,VMCtx& ctx, VM& vm) {
	vm.valueStack.push_back(vm.prog.doubleTable[instr.a]);
	ctx.pc++;
}
void ANSFunc(const Instr& instr,VMCtx& ctx, VM& vm) {
	vm.answers.push_back(ctx.regs[instr.a]);
	ctx.pc++; 
}
void CALL_BUILTINFunc(const Instr& instr,VMCtx& ctx, VM& vm) {
	int argNum = static_cast<int>(vm.prog.doubleTable[instr.c]);
	std::vector<double> args(argNum);
	for (int i = argNum - 1; i >= 0; --i) {
		args[i] = vm.valueStack.back();
		vm.valueStack.pop_back();
	}
	builtinFunc[instr.b](vm, args);
	ctx.regs[instr.a] = args[0];
	ctx.pc++; 
}
void DEF_FUNCFunc(const Instr& instr,VMCtx& ctx, VM& vm) {
	ctx.pc = vm.prog.uint32Table[instr.a];
 }
void CALL_FUNCFunc(const Instr& instr,VMCtx& ctx, VM& vm) {
	uint32_t startPos = vm.prog.uint32Table[instr.b];
	auto ctxIdx = vm.vmCtxPool.Create();
	VMCtx& newCtx = vm.vmCtxPool.Get(*ctxIdx);
	int argNum = static_cast<int>(vm.prog.doubleTable[instr.c]);
	for (int i = argNum - 1; i >= 0; --i) {
		newCtx.regs[MAX_TEMP_REG_NUM + i] = vm.valueStack.back();
		vm.valueStack.pop_back();
	}
	newCtx.pc = startPos;
	vm.ExecuteVM(*ctxIdx);
	ctx.regs[instr.a] = newCtx.regs[0];
	++ctx.pc;
 }
void END_FUNCFunc(const Instr& instr,VMCtx& ctx, VM& vm) {
	double ret = ctx.regs[instr.a];
	ctx.regs[0] = ret;
	ctx.isFinished = true;
}