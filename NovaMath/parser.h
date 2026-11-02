#pragma once
#include <vector>
#include "token.h"
#include "token_type.h"
#include "precedense.h"
#include "register_allocater.h"
#include "code_emitter.h"
#include "program.h"
#include "symbol_table.h"
#include "constance.h"
enum class Type : uint8_t {
	CONST,
	REG,
	NONE
};
struct ExprResult {
	Type type;
	union {
		double value;
		uint16_t reg;
	};
	ExprResult() : type(Type::NONE),value(0) {}
	ExprResult(double value) : type(Type::CONST), value(value) {}
	ExprResult(uint16_t reg) : type(Type::REG), reg(reg) {}
};
class Parser {
public:
	Parser(std::vector<Token>& tokens,Program& prog) : tokens(tokens),prog(prog),currentToken(&tokens[0]){
		InitRules();
		localsStack.push_back({});
	}
	void Parse();
	std::vector<Instr>& GetBin();
	bool isDebug = false;
private:
	using Nud = ExprResult(Parser::*)();
	using Led = ExprResult(Parser::*)(ExprResult);
	struct ParseRule {
		Nud nud;
		Led led;
		Precedense lbp;
	};
	static ParseRule rules[(uint8_t)TokenType::COUNT];
	ExprResult ParseStatement();
	ExprResult ParseExpression(int precedense = (int)Precedense::PREC_NONE);
	bool IsAtEnd();
	void InitRules();
	void Advance();
	Token* Consume();
	double ParseCall();
	void ParseIfElse();
	void ParseBlock();
	void ParseFor();
	uint16_t AddConst(double value);
	uint16_t Adduint32(uint32_t value);
	Token* Expect(TokenType type);
	ExprResult NudInc();
	ExprResult NudDec();
	ExprResult NudIdent();
	ExprResult NudNumber();
	ExprResult NudParen();
	ExprResult NudMinus();
	ExprResult NudNot();
	ExprResult LedInc(ExprResult left);
	ExprResult LedDec(ExprResult left);
	ExprResult LedAddEqual(ExprResult left);
	ExprResult LedSubEqual(ExprResult left);
	ExprResult LedMulEqual(ExprResult left);
	ExprResult LedDivEqual(ExprResult left);
	ExprResult LedHat(ExprResult left);
	ExprResult LedPlus(ExprResult left);
	ExprResult LedLess(ExprResult left);
	ExprResult LedGreater(ExprResult left);
	ExprResult LedLE(ExprResult left);
	ExprResult LedGE(ExprResult left);
	ExprResult LedEE(ExprResult left);
	ExprResult LedNE(ExprResult left);
	ExprResult LedMod(ExprResult left);
	ExprResult LedMinus(ExprResult left);
	ExprResult LedMulti(ExprResult left);
	ExprResult LedDiv(ExprResult left);
	ExprResult LedParen(ExprResult left);
	ExprResult LedAssign(ExprResult left);
	Token* Peek(int offset = 1);
	bool Match(TokenType type);
	Locals& CurrentLocals() {
		return localsStack.back();
	}
	Program& prog;
	std::vector<Locals> localsStack;
	Functions funcTable;
	RegisterAllocater regAlloc;
	Constance constance;
	CodeEmitter emitter;
	std::vector<Token>& tokens;
	Token* currentToken;
	size_t pos = 0;
};