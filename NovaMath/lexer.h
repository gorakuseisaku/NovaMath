#pragma once
#include <vector>
#include <string_view>
#include <unordered_map>
#include "token.h"
#include "token_type.h"
#include "string_hash.h"
class Lexer {
public:
	Lexer(std::string_view code) : code(code) {
		length = code.size();
		currentChar = code[0];
	}
	void Tokenize();
	std::vector<Token> tokens;
private:
	void Advance();
	void Skip();
	void ReadNumber();
	void ReadIdent();
	static std::unordered_map<std::string, TokenType,StringViewHash, std::equal_to<>> KEYWORDS;
	std::string_view code;
	char currentChar;
	size_t pos = 0;
	size_t line = 0;
	size_t length = 0;
};