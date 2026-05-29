#include "parser.h"
#include "error.h"
#include "opcode.h"
#include <iostream>
#include <iomanip>
Parser::ParseRule Parser::rules[(uint8_t)TokenType::COUNT];
void Parser::InitRules()
{
	using enum Precedense;
	for (int i = 0; i < (uint8_t)TokenType::COUNT; i++) {
		rules[i] = { nullptr,nullptr,PREC_NONE };
	}
	rules[(uint8_t)TokenType::NUMBER] = { &Parser::NudNumber,nullptr,PREC_NONE };
	rules[(uint8_t)TokenType::IDENT] = { &Parser::NudIdent,nullptr,PREC_NONE };
	rules[(uint8_t)TokenType::LPAREN] = { &Parser::NudParen,&Parser::LedParen,PREC_CALL };
	rules[(uint8_t)TokenType::MINUS] = { &Parser::NudMinus,&Parser::LedMinus,PREC_SUM };
	rules[(uint8_t)TokenType::PLUS] = { nullptr,&Parser::LedPlus,PREC_SUM };
	rules[(uint8_t)TokenType::MULTI] = { nullptr,&Parser::LedMulti,PREC_PRODUCT };
	rules[(uint8_t)TokenType::DIV] = { nullptr,&Parser::LedDiv,PREC_PRODUCT };
	rules[(uint8_t)TokenType::EQUAL] = { nullptr,&Parser::LedAssign,PREC_ASSIGN };
}
void Parser::Parse() {
	ExprResult final;
	while (!IsAtEnd()) {
		final = ParseStatement();
	}
	uint16_t dst = regAlloc.Alloc();
	if (final.type == Type::CONST) {
		emitter.Emit4(OpCode::LOAD_CONST, dst, AddConst(final.value));
	}
	if (isDebug) {
		std::cout << "Parsing succeeded" << '\n';
	}
}
std::vector<Instr>& Parser::GetBin() {
	return emitter.code;
}
ExprResult Parser::ParseStatement() {
	ExprResult result;
	switch (currentToken->type) {
	case (TokenType::DEBUG):
		Expect(TokenType::DEBUG);
		isDebug = true;
		break;
	case(TokenType::IDENT):
		if (Peek()->type == TokenType::EQUAL) {
			Token* name = Expect(TokenType::IDENT);
			Expect(TokenType::EQUAL);
			ExprResult right = ParseExpression((int)Precedense::PREC_ASSIGN - 1);
			Local local;
			if (!localTable.IsDefined(name->text)) {
				local = localTable.DefineLocal(name->text);
			}
			if (right.type == Type::CONST) {
				emitter.Emit4(OpCode::LOAD_CONST, local.slot, AddConst(right.value));
			}
			else if (right.type == Type::REG) {
				emitter.Emit4(OpCode::MOV, local.slot, right.reg);
				regAlloc.DeleteReg(right.reg);
			}
		}
		else {
			result = ParseExpression();
		}
		break;
	case (TokenType::LPAREN):
	default:
		result = ParseExpression();
		break;
	}
	return result;
}
ExprResult Parser::ParseExpression(int precedense)
{ 
	ExprResult left = (this->*rules[(uint8_t)currentToken->type].nud)();
	while (precedense < (int)rules[(uint8_t)currentToken->type].lbp) {
		if (currentToken->line != Peek(-1)->line) break;
		left = (this->*rules[(uint8_t)currentToken->type].led)(left);
	}
	return left;
}
bool Parser::IsAtEnd() {
	return (currentToken == nullptr || pos >= tokens.size() || currentToken->type == TokenType::END_OF_TOKEN);
}
void Parser::Advance() {
	++pos;
	if (pos < tokens.size()) {
		if (isDebug) {
			std::cout << std::setw(8) << TokenTypeToString(currentToken->type) << std::setw(10) << " text: " << currentToken->text;
			std::cout << std::setw(10) << " line: " << currentToken->line << '\n';
		}
		currentToken = &tokens[pos];
	}
}
double Parser::ParseCall() {
	double argNum = 0.0;
	if (currentToken->type != TokenType::RPAREN) {
		do {
			ExprResult arg = ParseExpression();
			if (arg.type == Type::CONST) {
				uint16_t dst = regAlloc.Alloc();
				emitter.Emit4(OpCode::LOAD_CONST,dst, AddConst(arg.value));
				emitter.Emit4(OpCode::PUSH,dst);
			}
			else if (arg.type == Type::REG){
				emitter.Emit4(OpCode::PUSH, arg.reg);
			}
			argNum += 1.0;
		} while (Match(TokenType::COMMA));
	}
	Expect(TokenType::RPAREN);
	return argNum;
}

uint16_t Parser::AddConst(double value) {
	size_t idx = prog.doubleTable.size();
	for (int i = 0; i < idx; ++i) {
		if (value == prog.doubleTable[i]) return i;
	}
	prog.doubleTable.push_back(value);
	return static_cast<uint16_t>(idx);
}
Token* Parser::Expect(TokenType type) {
	if (currentToken->type == type) {
		Token* before = currentToken;
		Advance();
		return before;
	}
	std::string msg = TokenTypeToString(currentToken->type) + " is not " + TokenTypeToString(type);
	Error(msg);
	return nullptr;
}
ExprResult Parser::NudIdent() {
	Token* token = Expect(TokenType::IDENT);
	Local local;
	if (localTable.IsDefined(token->text)) {
		local = localTable.ResolveLocal(token->text);
		return ExprResult(local.slot);
	}
	double value = 0;
	if (constance.GetConst(token->text, value)) {
		return ExprResult(value);
	}
	return ExprResult();
}
ExprResult Parser::NudNumber()
{
	Token* token = Expect(TokenType::NUMBER);
	return ExprResult(token->value);
}
ExprResult Parser::NudParen() {
	Expect(TokenType::LPAREN);
	ExprResult expression = ParseExpression();
	Expect(TokenType::RPAREN);
	return expression;
}
ExprResult Parser::NudMinus() {
	Expect(TokenType::MINUS);
	ExprResult right = ParseExpression((int)Precedense::PREC_UNARY);
	if (right.type == Type::CONST) {
		if (right.value == 0.0) {
			return ExprResult(0.0);
		}
		right.value = -right.value;
		return right;
	}
	else if (right.type == Type::REG){
		uint16_t dst = regAlloc.Alloc();
		emitter.Emit4(OpCode::NEG, dst, right.reg);
		regAlloc.DeleteReg(right.reg);
		return ExprResult(dst);
	}
}
ExprResult Parser::LedPlus(ExprResult left) {
	Expect(TokenType::PLUS);
	ExprResult right = ParseExpression((int)Precedense::PREC_SUM);
	if (left.type == Type::CONST && right.type == Type::CONST) {
		left.value += right.value;
		return left;
	}
	if (left.type == Type::CONST) {
		uint16_t reg = regAlloc.Alloc();
		emitter.Emit4(OpCode::LOAD_CONST, reg, AddConst(left.value));
		left = ExprResult(reg);
	}
	if (right.type == Type::CONST) {
		uint16_t reg = regAlloc.Alloc();
		emitter.Emit4(OpCode::LOAD_CONST, reg, AddConst(right.value));
		right = ExprResult(reg);
	}
	uint16_t dst = regAlloc.Alloc();
	emitter.Emit4(OpCode::ADD, dst, left.reg,right.reg);
	regAlloc.DeleteReg(left.reg);
	regAlloc.DeleteReg(right.reg);
	return ExprResult(dst);
}
ExprResult Parser::LedMinus(ExprResult left) {
	Expect(TokenType::MINUS);
	ExprResult right = ParseExpression((int)Precedense::PREC_SUM);
	if (left.type == Type::CONST && right.type == Type::CONST) {
		left.value -= right.value;
		return left;
	}
	if (left.type == Type::CONST) {
		uint16_t reg = regAlloc.Alloc();
		emitter.Emit4(OpCode::LOAD_CONST, reg, AddConst(left.value));
		left = ExprResult(reg);
	}
	if (right.type == Type::CONST) {
		uint16_t reg = regAlloc.Alloc();
		emitter.Emit4(OpCode::LOAD_CONST, reg, AddConst(right.value));
		right = ExprResult(reg);
	}
	uint16_t dst = regAlloc.Alloc();
	emitter.Emit4(OpCode::SUB, dst, left.reg, right.reg);
	regAlloc.DeleteReg(left.reg);
	regAlloc.DeleteReg(right.reg);
	return ExprResult(dst);
}
ExprResult Parser::LedMulti(ExprResult left) {
	Expect(TokenType::MULTI);
	ExprResult right = ParseExpression((int)Precedense::PREC_PRODUCT);
	if (left.type == Type::CONST && right.type == Type::CONST) {
		left.value *= right.value;
		return left;
	}
	if (left.type == Type::CONST) {
		uint16_t reg = regAlloc.Alloc();
		emitter.Emit4(OpCode::LOAD_CONST, reg, AddConst(left.value));
		left = ExprResult(reg);
	}
	if (right.type == Type::CONST) {
		uint16_t reg = regAlloc.Alloc();
		emitter.Emit4(OpCode::LOAD_CONST, reg, AddConst(right.value));
		right = ExprResult(reg);
	}
	uint16_t dst = regAlloc.Alloc();
	emitter.Emit4(OpCode::MUL, dst, left.reg, right.reg);
	regAlloc.DeleteReg(left.reg);
	regAlloc.DeleteReg(right.reg);
	return ExprResult(dst);
}
ExprResult Parser::LedDiv(ExprResult left) {
	Expect(TokenType::DIV);
	ExprResult right = ParseExpression((int)Precedense::PREC_PRODUCT);
	if (left.type == Type::CONST && right.type == Type::CONST) {
		left.value /= right.value;
		return left;
	}
	if (left.type == Type::CONST) {
		uint16_t reg = regAlloc.Alloc();
		emitter.Emit4(OpCode::LOAD_CONST, reg, AddConst(left.value));
		left = ExprResult(reg);
	}
	if (right.type == Type::CONST) {
		uint16_t reg = regAlloc.Alloc();
		emitter.Emit4(OpCode::LOAD_CONST, reg, AddConst(right.value));
		right = ExprResult(reg);
	}
	uint16_t dst = regAlloc.Alloc();
	emitter.Emit4(OpCode::DIV, dst, left.reg, right.reg);
	regAlloc.DeleteReg(left.reg);
	regAlloc.DeleteReg(right.reg);
	return ExprResult(dst);
}
ExprResult Parser::LedParen(ExprResult left) {
	Token* name = Peek(-1);
	if (name->line != currentToken->line) {
		return NudParen();
	}
	Expect(TokenType::LPAREN);
	uint16_t funcId = 0;
	if (constance.GetBuiltin(name->text, funcId)) {
		uint16_t dst = regAlloc.Alloc();
		double argNum = ParseCall();
		emitter.Emit4(OpCode::CALL_BUILTIN,dst, funcId, AddConst(argNum));
		return ExprResult(dst);
	}
	else {
		Error("The function is not supported");
	}
	return ExprResult();
}
ExprResult Parser::LedAssign(ExprResult left) {
	Token* name = Expect(TokenType::IDENT);
	Expect(TokenType::EQUAL);
	ExprResult right = ParseExpression((int)Precedense::PREC_ASSIGN - 1);
	Local local;
	if (!localTable.IsDefined(name->text)) {
		local = localTable.DefineLocal(name->text);
	}
	if (right.type == Type::CONST) {
		emitter.Emit4(OpCode::LOAD_CONST, local.slot, AddConst(right.value));
	}
	else if (right.type == Type::REG) {
		emitter.Emit4(OpCode::MOV, local.slot, right.reg);
		regAlloc.DeleteReg(right.reg);
	}
	return ExprResult(left.reg);
}
Token* Parser::Peek(int offset) {
	return &tokens[pos + offset];
}
bool Parser::Match(TokenType type) {
	if (currentToken->type == type) {
		Advance();
		return true;
	}
	return false;
}