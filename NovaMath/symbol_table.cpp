#include "symbol_table.h"
#include "config.h"
bool Locals::IsDefined(std::string_view name)
{
    size_t scopeStart = scopeStack.back();
    for (size_t i = scopeStart; i < locals.size(); ++i) {
        if (locals[i].name == name) return true;
    }
    return false;
}
bool Functions::IsDefined(std::string_view name)
{
    for (size_t i = functions.size(); i-- > 0;) {
        if (functions[i].name == name) return true;
    }
    return false;
}
Function& Functions::DefineFunction(std::string_view name, uint32_t pos)
{
    functions.push_back({ pos,std::string(name),{} });
    return functions.back();
}

Function Functions::ResolveFunction(std::string_view name)
{
    for (size_t i = functions.size(); i-- > 0;) {
        if (functions[i].name == name) return functions[i];
    }
    return Function();
}

Local Locals::DefineLocal(std::string_view name)
{
    uint16_t slot = MAX_VAR_REG_NUM + locals.size();
    Local local{ slot,std::string(name) };
    locals.push_back(local);
    return local;
}

Local Locals::ResolveLocal(std::string_view name)
{
    for (size_t i = locals.size(); i-- > 0;) {
        if (locals[i].name == name) return locals[i];
    }
    return Local();
}
