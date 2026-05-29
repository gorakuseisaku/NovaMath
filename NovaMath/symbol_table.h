#pragma once
#include <string>
#include <vector>
struct Local{
	uint16_t slot = 0;
	std::string name = "";
};
class Locals {
public:
	Locals() {};
	bool IsDefined(std::string_view name);
	Local DefineLocal(std::string_view name);
	void BeginScope() {
		scopeStack.push_back(locals.size());
	}
	void EndScope() {
		size_t scopeStart = scopeStack.back();
		locals.resize(scopeStart);
		scopeStack.pop_back();
	}
	Local ResolveLocal(std::string_view name);
private:
	std::vector<size_t> scopeStack = {0};
	std::vector<Local> locals;
};