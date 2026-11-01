#pragma once
#include <string>
#include <vector>
struct Local{
	uint16_t slot = 0;
	std::string name = "";
};
struct Function {
	uint32_t pos = 0;
	std::string name = "";
	std::vector<Local> args{};
};
class Locals {
public:
	Locals() {};
	uint32_t GetSize() const { return static_cast<uint32_t>(locals.size()); }
	bool IsDefined(std::string_view name);
	Local DefineLocal(std::string_view name);
	void BeginScope() {
		scopeStack.push_back(locals.size());
	}
	void BeginFunction() {
		locals.clear();
		scopeStack.clear();
		scopeStack.push_back(0);
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
class Functions {
public:
	Functions() {};
	bool IsDefined(std::string_view name);
	Function& DefineFunction(std::string_view name,uint32_t pos);
	Function ResolveFunction(std::string_view name);
private:
	std::vector<Function> functions;
};