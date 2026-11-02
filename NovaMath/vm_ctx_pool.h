#pragma once
#include <array>
#include <bit>
#include <optional>
#include "vm_ctx.h"
constexpr size_t MAX_VM_CTX_NUM = 128;
class VMCtxPool {
private:
	std::array<VMCtx,MAX_VM_CTX_NUM> vmCtx;
	std::array<uint64_t, MAX_VM_CTX_NUM / 64> masks;
public:
	std::optional<size_t> Create() {
		for (int i = 0; i < masks.size(); ++i) {
			if (masks[i] == 0xFFFFFFFFFFFFFFFFULL) {
				continue;
			}
			int bitIdx = std::countr_zero(~masks[i]);
			masks[i] |= (1ULL << bitIdx);
			vmCtx[bitIdx].Init();
			return static_cast<size_t>(bitIdx);
		}
		return std::nullopt;
	}
	void Delete(size_t idx) {
		if (idx >= MAX_VM_CTX_NUM) return;

		size_t blockIdx = idx / 64;
		size_t bitIdx = idx % 64;

		masks[blockIdx] &= ~(1ULL << bitIdx);
	}
	VMCtx& Get(size_t idx) {
		return vmCtx[idx];
	}
};