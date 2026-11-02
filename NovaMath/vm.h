#pragma once
#include <array>
#include <vector>
#include <string_view>
#include "program.h"
#include "instr.h"
#include "config.h"
#include "compiler.h"
#include "vm_ctx_pool.h"
struct CallBack {
	uint16_t returnReg = 0;
	uint32_t backPos = 0;
	uint16_t beforeBp = 0;
};
class VM {
public:
	std::vector<double> valueStack;
	std::vector<double> answers;
	std::vector<Instr> bin;
	Program prog;
	VMCtxPool vmCtxPool;
	VM() = default;
	~VM() = default;
	void Init(std::string_view filePath);
	void Run();
	void PrintAnswers();
	void ExecuteVM(size_t ctxIdx);
};