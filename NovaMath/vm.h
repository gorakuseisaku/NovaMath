#pragma once
#include <array>
#include <vector>
#include <string_view>
#include "program.h"
#include "instr.h"
#include "config.h"
#include "compiler.h"
class VM {
public:
	std::array<double, MAX_TEMP_REG_NUM + MAX_VAR_REG_NUM> regs;
	std::vector<double> valueStack;
	Program prog;
	VM(std::string_view filePath) {
		regs.fill(0);
		valueStack.reserve(MAX_ARG_NUM);
		Compile(filePath, prog, &bin);
	}
	double Run();
private:
	std::vector<Instr> bin;
};