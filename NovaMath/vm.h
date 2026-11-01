#pragma once
#include <array>
#include <vector>
#include <string_view>
#include "program.h"
#include "instr.h"
#include "config.h"
#include "compiler.h"
struct CallBack {
	uint16_t returnReg = 0;
	uint32_t backPos = 0;
	uint16_t beforeBp = 0;
};
class VM {
public:
	std::array<double, MAX_TEMP_REG_NUM * 10 + MAX_VAR_REG_NUM * 10> regs;
	std::vector<double> valueStack;
	std::vector<double> answers;
	std::vector<CallBack> callBacks;
	std::vector<Instr> bin;
	uint32_t pc = 0;
	uint16_t bp = 0;
	Program prog;
	VM(std::string_view filePath) {
		regs.fill(0);
		valueStack.reserve(MAX_ARG_NUM);
		Compile(filePath, prog, &bin);
	}
	void Run();
	void PrintAnswers();
};