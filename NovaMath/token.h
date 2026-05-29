#pragma once
#include <string_view>
#include "token_type.h"
struct Token {
	Token(TokenType type, std::string_view text,size_t line) : type(type),text(text),line(line) {}
	Token(TokenType type, std::string_view text, double value,size_t line) : type(type),text(text), value(value),line(line) {}
	TokenType type;
	double value = 0.0;
	size_t line = 1;
	std::string_view text = {};
};