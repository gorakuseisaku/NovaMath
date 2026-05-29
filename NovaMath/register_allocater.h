#pragma once
#include <stdint.h>
#include <vector>
#include "config.h"
class RegisterAllocater {
private:
	uint16_t nextReg = 0;
	std::vector<uint16_t> freeRegs;
public:
	RegisterAllocater() { freeRegs.reserve(MAX_TEMP_REG_NUM); }
	uint16_t Alloc() {
		if (!freeRegs.empty()) {
			uint16_t reg = freeRegs.back();
			freeRegs.pop_back();
			return reg;
		}
		return nextReg++;
	}
	void DeleteReg(uint16_t reg) {
		if (reg < MAX_TEMP_REG_NUM) {
			freeRegs.push_back(reg);
		}
	}
	void FlushRegs() {
		nextReg = 0;
		freeRegs.clear();
	}
};