#pragma once
#include <string>
struct StringViewHash {
    using is_transparent = void;
    size_t operator()(std::string_view txt) const {
        return std::hash<std::string_view>{}(txt);
    }
};