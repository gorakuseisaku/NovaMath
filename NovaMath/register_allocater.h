#pragma once
#include <stdint.h>
#include <vector>
#include "config.h"
class RegisterAllocater {
private:
	uint16_t nextReg = 0;
	uint64_t lockMask = 0;
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
	void Lock(uint16_t reg) {
		if (reg >= MAX_TEMP_REG_NUM) return;
		lockMask |= (1ULL << reg);
	}
	void Unlock(uint16_t reg) {
		if (reg >= MAX_TEMP_REG_NUM) return;
		lockMask &= ~(1ULL << reg);
	}
	void DeleteReg(uint16_t reg) {
		if (reg >= MAX_TEMP_REG_NUM) return;
		if ((lockMask & (1ULL << reg)) != 0) return;
		for (auto r : freeRegs) {
			if (r == reg) return;
		}
		freeRegs.push_back(reg);
	}
	void FlushRegs() {
		lockMask = 0;
		nextReg = 0;
		freeRegs.clear();
	}
};