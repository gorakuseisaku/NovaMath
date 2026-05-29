#pragma once
#include <stdint.h>
#include <iostream>
#include <string>
enum class TokenType : uint8_t {
#define TYPE(name) name,
#include "token_type.inc"
#undef TYPE
	COUNT
};
inline std::string TokenTypeToString(TokenType type) {
	switch (type) {
#define TYPE(name) case (TokenType::name): return #name;
#include "token_type.inc"
#undef TYPE
	}
}