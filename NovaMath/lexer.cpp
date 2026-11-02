#include "lexer.h"
#include "error.h"
#include <cctype>
#include "parser.h"
std::unordered_map<std::string, TokenType, StringViewHash, std::equal_to<void>> Lexer::KEYWORDS = {
	{"@debug",TokenType::DEBUG},
	{"@ans",TokenType::ANS},
	{"if",TokenType::IF},
	{"else",TokenType::ELSE},
	{"for",TokenType::FOR}
};
void Lexer::Tokenize()
{
	if (code.empty()) Error("The file is empty!");
	tokens.clear();
	while (currentChar != '\0') {
		Skip();
		if (pos + 1 < code.length()) {
			std::string_view op2 = code.substr(pos, 2);
			if (op2 == "++") {
				tokens.push_back(Token(TokenType::PLUS_PLUS, "++", line));
				Advance(); Advance();
				continue;
			}
			if (op2 == "--") {
				tokens.push_back(Token(TokenType::MINUS_MINUS, "--", line));
				Advance(); Advance();
				continue;
			}
			if (op2 == "+=") {
				tokens.push_back(Token(TokenType::PLUS_EQUAL, "+=", line));
				Advance(); Advance();
				continue;
			}
			if (op2 == "-=") {
				tokens.push_back(Token(TokenType::MINUS_EQUAL, "-=", line));
				Advance(); Advance();
				continue;
			}
			if (op2 == "*=") {
				tokens.push_back(Token(TokenType::STAR_EQUAL, "*=", line));
				Advance(); Advance();
				continue;
			}
			if (op2 == "/=") {
				tokens.push_back(Token(TokenType::SLASH_EQUAL, "/=", line));
				Advance(); Advance();
				continue;
			}
			if (op2 == "<=") { 
				tokens.push_back(Token(TokenType::LE, "<=", line)); 
				Advance(); Advance();
				continue; 
			}
			if (op2 == ">=") {
				tokens.push_back(Token(TokenType::GE, ">=", line));
				Advance(); Advance();
				continue;
			}
			if (op2 == "==") {
				tokens.push_back(Token(TokenType::EE, "==", line));
				Advance(); Advance();
				continue;
			}
			if (op2 == "!=") {
				tokens.push_back(Token(TokenType::NE, "^", line));
				Advance(); Advance();
				continue;
			}
		}
		switch (currentChar)
		{
		case ('^'):
			tokens.push_back(Token(TokenType::HAT, "^", line));
			Advance();
			continue;
		case('*'):
			tokens.push_back(Token(TokenType::MULTI,"*",line));
			Advance();
			continue;
		case('%'):
			tokens.push_back(Token(TokenType::PERCENT, "%", line));
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
		case('{'):
			tokens.push_back(Token(TokenType::LBRACE, "{", line));
			Advance();
			continue;
		case('}'):
			tokens.push_back(Token(TokenType::RBRACE, "}", line));
			Advance();
			continue;
		case('<'):
			tokens.push_back(Token(TokenType::LESS, "<", line));
			Advance();
			continue;
		case('>'):
			tokens.push_back(Token(TokenType::GREATER, ">", line));
			Advance();
			continue;
		case('='):
			tokens.push_back(Token(TokenType::EQUAL,"=",line));
			Advance();
			continue;
		case('!'):
			tokens.push_back(Token(TokenType::NOT, "!", line));
			Advance();
			continue;
		case(','):
			tokens.push_back(Token(TokenType::COMMA, ",",line));
			Advance();
			continue;
		case(';'):
			tokens.push_back(Token(TokenType::SEMICOLON, ";", line));
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
	while (pos < code.size() && (isalnum(static_cast<unsigned char>(currentChar)) || currentChar == '_' || currentChar == '@')) Advance();
	std::string_view text(&code[startPos], pos - startPos);
	auto it = KEYWORDS.find(text);
	if (it != KEYWORDS.end()) {
		tokens.push_back(Token(it->second, text,line));
		return;
	}
	tokens.push_back(Token(TokenType::IDENT, text,line));
}