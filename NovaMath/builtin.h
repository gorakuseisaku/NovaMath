#pragma once
#include "vm.h"
#include <vector>
#include <stdint.h>
#define BUILTIN(name) void name##Func(VM& vm,std::vector<double>& args);
#include "builtin.inc"
#undef BUILTIN
using BuiltinFunc = void(*)(VM&, std::vector<double>&);
inline constexpr BuiltinFunc builtinFunc[] = {
#define BUILTIN(name) name##Func,
#include "builtin.inc"
#undef BUILTIN
};
void RoundDouble(double& value);