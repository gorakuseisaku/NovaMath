#pragma once
#include <array>
#include "config.h"
class VMCtx {
public:
	VMCtx() = default;
	~VMCtx() = default;
	VMCtx& operator=(const VMCtx& other) {
		this->regs = other.regs;
		this->pc = other.pc;
		this->isFinished = other.isFinished;
	}
	void Init() {
		regs.fill(0.0);
		pc = 0;
		isFinished = false;
	}
	std::array<double, MAX_TEMP_REG_NUM + MAX_VAR_REG_NUM> regs;
	uint32_t pc = 0;
	bool isFinished = false;
};