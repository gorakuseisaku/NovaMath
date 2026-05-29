#include "lexer.h"
#include "error.h"
#include <cctype>
#include "parser.h"
std::unordered_map<std::string, TokenType, StringViewHash, std::equal_to<>> Lexer::KEYWORDS = {
	{"@debug",TokenType::DEBUG}
};
void Lexer::Tokenize()
{
	if (code.empty()) Error("The file is empty!");
	tokens.clear();
	while (currentChar != '\0') {
		Skip();
		switch (currentChar)
		{
		case('*'):
			tokens.push_back(Token(TokenType::MULTI,"*",line));
			Advance();
			continue;
		case('+'):
			tokens.push_back(Token(TokenType::PLUS,"+",line));
			Advance();
			continue;
		case('-'):
			tokens.push_back(Token(TokenType::MINUS,"-",line));
			Advance();
			continue;
		case('/'):
			tokens.push_back(Token(TokenType::DIV,"/",line));
			Advance();
			continue;
		case('('):
			tokens.push_back(Token(TokenType::LPAREN,"(",line));
			Advance();
			continue;
		case(')'):
			tokens.push_back(Token(TokenType::RPAREN,")",line));
			Advance();
			continue;
		case('='):
			tokens.push_back(Token(TokenType::EQUAL,"=",line));
			Advance();
			continue;
		case(','):
			tokens.push_back(Token(TokenType::COMMA, ",",line));
			Advance();
			continue;
		}

		if (std::isdigit(static_cast<unsigned char>(currentChar))) {
			ReadNumber();
			continue;
		}
		if (isalpha(static_cast<unsigned char>(currentChar)) || currentChar == '_' || currentChar == '@') {
			ReadIdent();
			continue;
		}
		Advance();
	}
	tokens.emplace_back(Token(TokenType::END_OF_TOKEN, "\0",line));
}
void Lexer::Advance() {
	++pos;
	if (pos >= length) {
		currentChar = '\0';
	}
	else {
		currentChar = code[pos];
	}
}

void Lexer::Skip()
{
	while (pos < length) {
		char c = code[pos];
		if (c == '\n') {
			++line;
			Advance();
			continue;
		}
		if (c == ' ' || c == '\t' || c == '\r') {
			Advance();
			continue;
		}
		else if (c == '#') {
			Advance();
			while (pos < length && code[pos] != '\n') {
				Advance();
			}
			continue;
		}
		break;
	}
}

void Lexer::ReadNumber()
{
	size_t startPos = pos;
	double value = 0.0;
	while (std::isdigit(static_cast<unsigned char>(currentChar))) {
		value = value * 10.0 + (currentChar - '0');
		Advance();
	}
	if (currentChar == '.') {
		Advance();
		double base = 0.1;
		while (std::isdigit(static_cast<unsigned char>(currentChar))) {
			value += (currentChar - '0') * base;
			base *= 0.1;
			Advance();
		}
	}
	std::string_view text = code.substr(startPos, pos - startPos);
	tokens.push_back(Token(TokenType::NUMBER,text,value,line));
}
void Lexer::ReadIdent()
{
	int startPos = pos;
	while (isalnum(static_cast<unsigned char>(currentChar)) || currentChar == '_' || currentChar == '@') Advance();
	std::string_view text(&code[startPos], pos - startPos);
	auto it = KEYWORDS.find(text);
	if (it != KEYWORDS.end()) {
		tokens.push_back(Token(it->second, text,line));
		return;
	}
	tokens.push_back(Token(TokenType::IDENT, text,line));
}