#pragma once
#include <unordered_map>
#include <string>
#include "error.h"
#include "string_hash.h"
class Constance {
public:
	Constance() {
#define BUILTIN(name) builtins.emplace(#name,builtins.size());
#include "builtin.inc"
#undef BUILTIN
#define CONST(name,value) constances.emplace(#name,value);
#include "constant.inc"
#undef CONST
	}
	bool GetBuiltin(std::string_view name,uint16_t& id) {
		auto it = builtins.find(name);
		if (it != builtins.end()) {
			id = it->second;
			return true;
		}
		return false;
	}
	bool GetConst(std::string_view name,double& value) {
		auto it = constances.find(name);
		if (it != constances.end()) {
			value = it->second;
			return true;
		}
		return false;
	}
private:
	std::unordered_map<std::string, uint16_t, StringViewHash, std::equal_to<>> builtins;
	std::unordered_map<std::string, double, StringViewHash, std::equal_to<>> constances;
};