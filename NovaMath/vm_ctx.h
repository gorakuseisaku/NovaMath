#pragma once
#include <vector>
#include "config.h"
class VMCtx {
public:
	VMCtx() : regs(MAX_TEMP_REG_NUM + MAX_VAR_REG_NUM) {};
	~VMCtx() = default;
	VMCtx& operator=(const VMCtx& other) {
		this->regs = other.regs;
		this->pc = other.pc;
		this->isFinished = other.isFinished;
	}
	void Init() {
		for (int i = 0; i < MAX_TEMP_REG_NUM + MAX_VAR_REG_NUM; ++i) {
			regs[i] = 0.0;
		}
		pc = 0;
		isFinished = false;
	}
	std::vector<double> regs;
	uint32_t pc = 0;
	bool isFinished = false;
};