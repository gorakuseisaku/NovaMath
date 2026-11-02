#include "parser.h"
#include "error.h"
#include "opcode.h"
#include <iostream>
#include <iomanip>
Parser::ParseRule Parser::rules[(uint8_t)TokenType::COUNT];
void Parser::InitRules()
{
	using enum Precedense;
	for (int i = 0; i < (uint8_t)TokenType::COUNT; ++i) {
		rules[i] = { nullptr,nullptr,PREC_NONE };
	}
	rules[(uint8_t)TokenType::NUMBER] = { &Parser::NudNumber,nullptr,PREC_NONE };
	rules[(uint8_t)TokenType::IDENT] = { &Parser::NudIdent,nullptr,PREC_NONE };
	rules[(uint8_t)TokenType::LPAREN] = { &Parser::NudParen,&Parser::LedParen,PREC_CALL };
	rules[(uint8_t)TokenType::MINUS] = { &Parser::NudMinus,&Parser::LedMinus,PREC_SUM };
	rules[(uint8_t)TokenType::NOT] = { &Parser::NudNot,nullptr,PREC_PRODUCT };
	rules[(uint8_t)TokenType::PLUS] = { nullptr,&Parser::LedPlus,PREC_SUM };
	rules[(uint8_t)TokenType::MULTI] = { nullptr,&Parser::LedMulti,PREC_PRODUCT };
	rules[(uint8_t)TokenType::PERCENT] = { nullptr,&Parser::LedMod,PREC_PRODUCT };
	rules[(uint8_t)TokenType::HAT] = { nullptr,&Parser::LedHat,PREC_PRODUCT };
	rules[(uint8_t)TokenType::LESS] = { nullptr,&Parser::LedLess,PREC_COMPARE };
	rules[(uint8_t)TokenType::GREATER] = { nullptr,&Parser::LedGreater,PREC_COMPARE };
	rules[(uint8_t)TokenType::LE] = { nullptr,&Parser::LedLE,PREC_COMPARE };
	rules[(uint8_t)TokenType::GE] = { nullptr,&Parser::LedGE,PREC_COMPARE };
	rules[(uint8_t)TokenType::EE] = { nullptr,&Parser::LedEE,PREC_EQUALITY };
	rules[(uint8_t)TokenType::NE] = { nullptr,&Parser::LedNE,PREC_EQUALITY };
	rules[(uint8_t)TokenType::PLUS_PLUS] = { &Parser::NudInc,&Parser::LedInc,PREC_CALL };
	rules[(uint8_t)TokenType::MINUS_MINUS] = { &Parser::NudDec,&Parser::LedDec,PREC_CALL };
	rules[(uint8_t)TokenType::PLUS_EQUAL] = { nullptr,&Parser::LedAddEqual,PREC_SUM };
	rules[(uint8_t)TokenType::MINUS_EQUAL] = { nullptr,&Parser::LedSubEqual,PREC_SUM };
	rules[(uint8_t)TokenType::STAR_EQUAL] = { nullptr,&Parser::LedMulEqual,PREC_PRODUCT };
	rules[(uint8_t)TokenType::SLASH_EQUAL] = { nullptr,&Parser::LedDivEqual,PREC_PRODUCT };
	rules[(uint8_t)TokenType::DIV] = { nullptr,&Parser::LedDiv,PREC_PRODUCT };
	rules[(uint8_t)TokenType::EQUAL] = { nullptr,&Parser::LedAssign,PREC_ASSIGN };
}
void Parser::Parse() {
	while (!IsAtEnd()) {
		ParseStatement();
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
		isDebug = true;
		Expect(TokenType::DEBUG);
		break;
	case(TokenType::ANS):
	{
		Expect(TokenType::ANS);
		Expect(TokenType::EQUAL);
		ExprResult right = ParseExpression((int)Precedense::PREC_ASSIGN - 1);
		uint16_t dst = regAlloc.Alloc();
		if (right.type == Type::CONST) {
			emitter.Emit4(OpCode::MOVK, dst, AddConst(right.value));
		}
		else if (right.type == Type::REG) {
			emitter.Emit4(OpCode::MOVR, dst, right.reg);
			regAlloc.DeleteReg(right.reg);
		}
		emitter.Emit4(OpCode::ANS, dst);
		break;
	}
	case(TokenType::IDENT):
		if (Peek()->type == TokenType::EQUAL) {
			Token* name = Expect(TokenType::IDENT);
			Expect(TokenType::EQUAL);
			ExprResult right = ParseExpression((int)Precedense::PREC_ASSIGN - 1);
			Local local = CurrentLocals().ResolveLocal(name->text);
			if (local.name == "") {
				local = CurrentLocals().DefineLocal(name->text);
			}
			if (right.type == Type::CONST) {
				emitter.Emit4(OpCode::MOVK, local.slot, AddConst(right.value));
			}
			else if (right.type == Type::REG) {
				emitter.Emit4(OpCode::MOVR, local.slot, right.reg);
				regAlloc.DeleteReg(right.reg);
			}
		}
		else if (Peek()->type == TokenType::LPAREN) {
			Token* name = Expect(TokenType::IDENT);
			size_t defPos = emitter.Emit4(OpCode::DEF_FUNC, 0);
			Function& func = funcTable.DefineFunction(name->text, emitter.code.size());
			localsStack.push_back({});
			CurrentLocals().BeginFunction();
			regAlloc.FlushRegs();
			Expect(TokenType::LPAREN);
			if (currentToken->type != TokenType::RPAREN) {
				do {
					Token* name = Expect(TokenType::IDENT);
					func.args.push_back(CurrentLocals().DefineLocal(name->text));
				} while (Match(TokenType::COMMA));
			}
			Expect(TokenType::RPAREN);
			Expect(TokenType::EQUAL);
			uint16_t dst = regAlloc.Alloc();
			ExprResult right = ParseExpression((int)Precedense::PREC_ASSIGN - 1);
			if (right.type == Type::CONST) {
				emitter.Emit4(OpCode::MOVK, dst, AddConst(right.value));
			}
			else if (right.type == Type::REG) {
				emitter.Emit4(OpCode::MOVR, dst, right.reg);
				regAlloc.DeleteReg(right.reg);
			}
			localsStack.pop_back();
			emitter.Emit4(OpCode::END_FUNC,dst);
			emitter.code[defPos].a = Adduint32(emitter.code.size());
		}
		else {
			result = ParseExpression();
		}
		break;
	case (TokenType::FOR):
		ParseFor();
		break;
	case (TokenType::IF):
		ParseIfElse();
		break;
	case (TokenType::LBRACE):
		ParseBlock();
		break;
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
Token* Parser::Consume() {
	Token* token = currentToken;
	Advance();
	return token;
}
double Parser::ParseCall() {
	double argNum = 0.0;
	if (currentToken->type != TokenType::RPAREN) {
		do {
			ExprResult arg = ParseExpression();
			if (arg.type == Type::CONST) {
				emitter.Emit4(OpCode::PUSHK,AddConst(arg.value));
			}
			else if (arg.type == Type::REG){
				emitter.Emit4(OpCode::PUSHR, arg.reg);
			}
			argNum += 1.0;
		} while (Match(TokenType::COMMA));
	}
	Expect(TokenType::RPAREN);
	return argNum;
}
void Parser::ParseIfElse() {
	Expect(TokenType::IF);
	Expect(TokenType::LPAREN);
	ExprResult cond = ParseExpression();
	Expect(TokenType::RPAREN);
	size_t jumpToElse = emitter.Emit4(OpCode::JZ, cond.reg, 0);
	ParseStatement();
	size_t jumpToEnd = 0;
	if (Match(TokenType::ELSE)) {
		jumpToEnd = emitter.Emit4(OpCode::JM, 0);
		uint32_t elseTop = emitter.GetCurrentIp();
		emitter.code[jumpToElse].b = Adduint32(elseTop);
		ParseStatement();
		uint32_t endTop = emitter.GetCurrentIp();
		emitter.code[jumpToEnd].a = Adduint32(endTop);
	}
	else {
		uint32_t endTop = emitter.GetCurrentIp();
		emitter.code[jumpToElse].b = Adduint32(endTop);
	}

}
void Parser::ParseBlock()
{
	Expect(TokenType::LBRACE);
	CurrentLocals().BeginScope();
	while (currentToken->type != TokenType::RBRACE && !IsAtEnd()) {
		ParseStatement();
	}
	CurrentLocals().EndScope();
	Expect(TokenType::RBRACE);
}
void Parser::ParseFor() {
	Expect(TokenType::FOR);
	CurrentLocals().BeginScope();
	Expect(TokenType::LPAREN);
	const Token* name = Expect(TokenType::IDENT);
	Local counter = CurrentLocals().DefineLocal(name->text);
	Expect(TokenType::EQUAL);
	ExprResult left = ParseExpression();
	if (left.type == Type::CONST) {
		emitter.Emit4(OpCode::MOVK, counter.slot, AddConst(left.value));
	}
	else if (left.type == Type::REG) {
		emitter.Emit4(OpCode::MOVR, counter.slot, left.reg);
		regAlloc.DeleteReg(left.reg);
	}
	Expect(TokenType::SEMICOLON);
	uint32_t startIp = emitter.GetCurrentIp();
	ExprResult cond = ParseExpression();
	uint16_t condReg = regAlloc.Alloc();
	regAlloc.Lock(condReg);
	if (cond.type == Type::CONST) {
		emitter.Emit4(OpCode::MOVK, condReg, AddConst(cond.value));
	}
	else if (cond.type == Type::REG) {
		emitter.Emit4(OpCode::MOVR, condReg, cond.reg);
		regAlloc.DeleteReg(cond.reg);
	}
	size_t condDef = emitter.Emit4(OpCode::JZ, condReg, 0);
	size_t jmToBody = emitter.Emit4(OpCode::JM, 0);
	Expect(TokenType::SEMICOLON);
	uint32_t exprIp = emitter.GetCurrentIp();
	ExprResult expr = ParseExpression();
	emitter.Emit4(OpCode::JM, Adduint32(startIp));
	Expect(TokenType::RPAREN);
	uint32_t bodyStart = emitter.GetCurrentIp();
	emitter.code[jmToBody].a = Adduint32(bodyStart);
	ParseStatement();
	emitter.Emit4(OpCode::JM, Adduint32(exprIp));
	uint32_t endIp = emitter.GetCurrentIp();
	emitter.code[condDef].b = Adduint32(endIp);
	regAlloc.Unlock(condReg);
	regAlloc.DeleteReg(condReg);
	CurrentLocals().EndScope();

}
uint16_t Parser::AddConst(double value) {
	size_t idx = prog.doubleTable.size();
	for (int i = 0; i < idx; ++i) {
		if (value == prog.doubleTable[i]) return i;
	}
	prog.doubleTable.push_back(value);
	return static_cast<uint16_t>(idx);
}
uint16_t Parser::Adduint32(uint32_t value) {
	size_t idx = prog.uint32Table.size();
	for (int i = 0; i < idx; ++i) {
		if (value == prog.uint32Table[i]) return i;
	}
	prog.uint32Table.push_back(value);
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
ExprResult Parser::NudInc() {
	Expect(TokenType::PLUS_PLUS);
	Token* name = Expect(TokenType::IDENT);
	Local var = CurrentLocals().ResolveLocal(name->text);
	uint16_t dst = regAlloc.Alloc();
	emitter.Emit4(OpCode::INC, dst, var.slot, 0);
	return ExprResult(dst);
}
ExprResult Parser::NudDec() {
	Expect(TokenType::MINUS_MINUS);
	Token* name = Expect(TokenType::IDENT);
	Local var = CurrentLocals().ResolveLocal(name->text);
	uint16_t dst = regAlloc.Alloc();
	emitter.Emit4(OpCode::DEC, dst, var.slot, 0);
	return ExprResult(dst);
}
ExprResult Parser::NudIdent() {
	Token* token = Expect(TokenType::IDENT);
	Local local = CurrentLocals().ResolveLocal(token->text);
	if (local.name != "") {
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
		right.value = -right.value;
		return right;
	}
	else if (right.type == Type::REG){
		uint16_t dst = regAlloc.Alloc();
		emitter.Emit4(OpCode::NEG,dst, right.reg);
		regAlloc.DeleteReg(right.reg);
		return dst;
	}
}
ExprResult Parser::NudNot() {
	Expect(TokenType::NOT);
	ExprResult right = ParseExpression((int)Precedense::PREC_UNARY);
	if (right.type == Type::CONST) {
		right.value = (std::abs(right.value) < 1e-14) ? 1.0 : 0.0;
		return right;
	}
	else if (right.type == Type::REG) {
		uint16_t dst = regAlloc.Alloc();
		emitter.Emit4(OpCode::NEG, dst, right.reg);
		regAlloc.DeleteReg(right.reg);
		return dst;
	}
}
ExprResult Parser::LedInc(ExprResult left) {
	Expect(TokenType::PLUS_PLUS);
	uint16_t dst = regAlloc.Alloc();
	emitter.Emit4(OpCode::INC, dst, left.reg, 1);
	return ExprResult(dst);
}
ExprResult Parser::LedDec(ExprResult left) {
	Expect(TokenType::MINUS_MINUS);
	uint16_t dst = regAlloc.Alloc();
	emitter.Emit4(OpCode::DEC, dst, left.reg, 1);
	return ExprResult(dst);
}
ExprResult Parser::LedAddEqual(ExprResult left) {
	Expect(TokenType::PLUS_EQUAL);
	ExprResult right = ParseExpression((int)Precedense::PREC_SUM);
	if (right.type == Type::CONST) {
		emitter.Emit4(OpCode::ADDEQK, left.reg, AddConst(right.value));
	}
	else if (right.type == Type::REG) {
		emitter.Emit4(OpCode::ADDEQR, left.reg, right.reg);
	}
	return left;
}
ExprResult Parser::LedSubEqual(ExprResult left) {
	Expect(TokenType::MINUS_EQUAL);
	ExprResult right = ParseExpression((int)Precedense::PREC_SUM);
	if (right.type == Type::CONST) {
		emitter.Emit4(OpCode::SUBEQK, left.reg, AddConst(right.value));
	}
	else if (right.type == Type::REG) {
		emitter.Emit4(OpCode::SUBEQR, left.reg, right.reg);
	}
	return left;
}
ExprResult Parser::LedMulEqual(ExprResult left) {
	Expect(TokenType::STAR_EQUAL);
	ExprResult right = ParseExpression((int)Precedense::PREC_PRODUCT);
	if (right.type == Type::CONST) {
		emitter.Emit4(OpCode::MULEQK, left.reg, AddConst(right.value));
	}
	else if (right.type == Type::REG) {
		emitter.Emit4(OpCode::MULEQR, left.reg, right.reg);
	}
	return left;
}
ExprResult Parser::LedDivEqual(ExprResult left) {
	Expect(TokenType::SLASH_EQUAL);
	ExprResult right = ParseExpression((int)Precedense::PREC_PRODUCT);
	if (right.type == Type::CONST) {
		emitter.Emit4(OpCode::DIVEQK, left.reg, AddConst(right.value));
	}
	else if (right.type == Type::REG) {
		emitter.Emit4(OpCode::DIVEQR, left.reg, right.reg);
	}
	return left;
}
ExprResult Parser::LedHat(ExprResult left) {
	Expect(TokenType::HAT);
	ExprResult right = ParseExpression((int)Precedense::PREC_PRODUCT);
	if (left.type == Type::CONST && right.type == Type::CONST) {
		left.value = std::pow(left.value, right.value);
		return left;
	}
	uint16_t dst = regAlloc.Alloc();
	if (left.type == Type::CONST && right.type == Type::REG) {
		emitter.Emit4(OpCode::EXPKR, dst, AddConst(left.value), right.reg);
	}
	if (left.type == Type::REG && right.type == Type::CONST) {
		emitter.Emit4(OpCode::EXPRK, dst, left.reg, AddConst(right.value));
	}
	if (left.type == Type::REG && right.type == Type::REG) {
		emitter.Emit4(OpCode::EXPRR, dst, left.reg, right.reg);
	}
	regAlloc.DeleteReg(left.reg);
	regAlloc.DeleteReg(right.reg);
	return ExprResult(dst);
}
ExprResult Parser::LedMod(ExprResult left) {
	Expect(TokenType::PERCENT);
	ExprResult right = ParseExpression((int)Precedense::PREC_PRODUCT);
	if (left.type == Type::CONST && right.type == Type::CONST) {
		left.value = std::fmod(left.value,right.value);
		return left;
	}
	uint16_t dst = regAlloc.Alloc();
	if (left.type == Type::CONST && right.type == Type::REG) {
		emitter.Emit4(OpCode::MODKR, dst, AddConst(left.value), right.reg);
	}
	if (left.type == Type::REG && right.type == Type::CONST) {
		emitter.Emit4(OpCode::MODRK, dst, left.reg, AddConst(right.value));
	}
	if (left.type == Type::REG && right.type == Type::REG) {
		emitter.Emit4(OpCode::MODRR, dst, left.reg, right.reg);
	}
	regAlloc.DeleteReg(left.reg);
	regAlloc.DeleteReg(right.reg);
	return ExprResult(dst);
}
ExprResult Parser::LedPlus(ExprResult left) {
	Expect(TokenType::PLUS);
	ExprResult right = ParseExpression((int)Precedense::PREC_SUM);
	if (left.type == Type::CONST && right.type == Type::CONST) {
		left.value += right.value;
		return left;
	}
	uint16_t dst = regAlloc.Alloc();
	if (left.type == Type::CONST && right.type == Type::REG) {
		emitter.Emit4(OpCode::ADDKR, dst, AddConst(left.value), right.reg);
	}
	if (left.type == Type::REG && right.type == Type::CONST) {
		emitter.Emit4(OpCode::ADDKR, dst, AddConst(right.value), left.reg);
	}
	if (left.type == Type::REG && right.type == Type::REG) {
		emitter.Emit4(OpCode::ADDRR, dst, left.reg, right.reg);
	}
	regAlloc.DeleteReg(left.reg);
	regAlloc.DeleteReg(right.reg);
	return ExprResult(dst);
}
ExprResult Parser::LedLess(ExprResult left) {
	Expect(TokenType::LESS);
	ExprResult right = ParseExpression((int)Precedense::PREC_COMPARE);
	if (left.type == Type::CONST && right.type == Type::CONST) {
		left.value = static_cast<double>(left.value < right.value);
		return left;
	}
	uint16_t dst = regAlloc.Alloc();
	if (left.type == Type::CONST && right.type == Type::REG) {
		emitter.Emit4(OpCode::LESSKR, dst, AddConst(left.value), right.reg);
	}
	if (left.type == Type::REG && right.type == Type::CONST) {
		emitter.Emit4(OpCode::LESSRK, dst, left.reg, AddConst(right.value));
	}
	if (left.type == Type::REG && right.type == Type::REG) {
		emitter.Emit4(OpCode::LESSRR, dst, left.reg, right.reg);
	}
	regAlloc.DeleteReg(left.reg);
	regAlloc.DeleteReg(right.reg);
	return ExprResult(dst);
}
ExprResult Parser::LedGreater(ExprResult left) {
	Expect(TokenType::GREATER);
	ExprResult right = ParseExpression((int)Precedense::PREC_COMPARE);
	if (left.type == Type::CONST && right.type == Type::CONST) {
		left.value = static_cast<double>(left.value > right.value);
		return left;
	}
	uint16_t dst = regAlloc.Alloc();
	if (left.type == Type::CONST && right.type == Type::REG) {
		emitter.Emit4(OpCode::LESSRK, dst, right.reg, AddConst(left.value));
	}
	if (left.type == Type::REG && right.type == Type::CONST) {
		emitter.Emit4(OpCode::LESSKR, dst, AddConst(right.value), right.reg);
	}
	if (left.type == Type::REG && right.type == Type::REG) {
		emitter.Emit4(OpCode::LESSRR, dst, right.reg, left.reg);
	}
	regAlloc.DeleteReg(left.reg);
	regAlloc.DeleteReg(right.reg);
	return ExprResult(dst);
}
ExprResult Parser::LedLE(ExprResult left) {
	Expect(TokenType::LE);
	ExprResult right = ParseExpression((int)Precedense::PREC_COMPARE);
	if (left.type == Type::CONST && right.type == Type::CONST) {
		left.value = static_cast<double>(left.value <= right.value);
		return left;
	}
	uint16_t dst = regAlloc.Alloc();
	if (left.type == Type::CONST && right.type == Type::REG) {
		emitter.Emit4(OpCode::LESSRK, dst, right.reg, AddConst(left.value));
		emitter.Emit4(OpCode::NOT, dst);
	}
	if (left.type == Type::REG && right.type == Type::CONST) {
		emitter.Emit4(OpCode::LESSKR, dst,  AddConst(right.value),left.reg);
		emitter.Emit4(OpCode::NOT, dst);
	}
	if (left.type == Type::REG && right.type == Type::REG) {
		emitter.Emit4(OpCode::LESSRR, dst, right.reg, left.reg);
		emitter.Emit4(OpCode::NOT, dst);
	}
	regAlloc.DeleteReg(left.reg);
	regAlloc.DeleteReg(right.reg);
	return ExprResult(dst);
}
ExprResult Parser::LedGE(ExprResult left) {
	Expect(TokenType::GE);
	ExprResult right = ParseExpression((int)Precedense::PREC_COMPARE);
	if (left.type == Type::CONST && right.type == Type::CONST) {
		left.value = static_cast<double>(left.value >= right.value);
		return left;
	}
	uint16_t dst = regAlloc.Alloc();
	if (left.type == Type::CONST && right.type == Type::REG) {
		emitter.Emit4(OpCode::LESSKR, dst, AddConst(left.value), right.reg);
		emitter.Emit4(OpCode::NOT, dst);
	}
	if (left.type == Type::REG && right.type == Type::CONST) {
		emitter.Emit4(OpCode::LESSRK, dst, left.reg, AddConst(right.value));
		emitter.Emit4(OpCode::NOT, dst);
	}
	if (left.type == Type::REG && right.type == Type::REG) {
		emitter.Emit4(OpCode::LESSRR, dst, left.reg, right.reg);
		emitter.Emit4(OpCode::NOT, dst);
	}
	regAlloc.DeleteReg(left.reg);
	regAlloc.DeleteReg(right.reg);
	return ExprResult(dst);
}
ExprResult Parser::LedEE(ExprResult left) {
	Expect(TokenType::EE);
	ExprResult right = ParseExpression((int)Precedense::PREC_EQUALITY);
	if (left.type == Type::CONST && right.type == Type::CONST) {
		left.value = static_cast<double>(left.value - right.value <= 1e-14);
		return left;
	}
	uint16_t dst = regAlloc.Alloc();
	if (left.type == Type::CONST && right.type == Type::REG) {
		emitter.Emit4(OpCode::EERK, dst, right.reg, AddConst(left.value));
	}
	if (left.type == Type::REG && right.type == Type::CONST) {
		emitter.Emit4(OpCode::EERK, dst, left.reg, AddConst(right.value));
	}
	if (left.type == Type::REG && right.type == Type::REG) {
		emitter.Emit4(OpCode::EERR, dst, left.reg, right.reg);
	}
	regAlloc.DeleteReg(left.reg);
	regAlloc.DeleteReg(right.reg);
	return ExprResult(dst);
}
ExprResult Parser::LedNE(ExprResult left) {
	Expect(TokenType::NE);
	ExprResult right = ParseExpression((int)Precedense::PREC_EQUALITY);
	if (left.type == Type::CONST && right.type == Type::CONST) {
		left.value = static_cast<double>(!(left.value - right.value <= 1e-14));
		return left;
	}
	uint16_t dst = regAlloc.Alloc();
	if (left.type == Type::CONST && right.type == Type::REG) {
		emitter.Emit4(OpCode::EERK, dst, right.reg, AddConst(left.value));
		emitter.Emit4(OpCode::NOT, dst);
	}
	if (left.type == Type::REG && right.type == Type::CONST) {
		emitter.Emit4(OpCode::EERK, dst, left.reg, AddConst(right.value));
		emitter.Emit4(OpCode::NOT,dst);
	}
	if (left.type == Type::REG && right.type == Type::REG) {
		emitter.Emit4(OpCode::EERR, dst, left.reg, right.reg);
		emitter.Emit4(OpCode::NOT, dst);
	}
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
	uint16_t dst = regAlloc.Alloc();
	if (left.type == Type::CONST && right.type == Type::REG) {
		emitter.Emit4(OpCode::SUBKR, dst, AddConst(left.value), right.reg);
	}
	if (left.type == Type::REG && right.type == Type::CONST) {
		emitter.Emit4(OpCode::SUBRK,dst, AddConst(right.value), left.reg);
	}
	if (left.type == Type::REG && right.type == Type::REG) {
		emitter.Emit4(OpCode::SUBRR, dst, left.reg, right.reg);
	}
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
	uint16_t dst = regAlloc.Alloc();
	if (left.type == Type::CONST && right.type == Type::REG) {
		emitter.Emit4(OpCode::MULKR, dst, AddConst(left.value), right.reg);
	}
	if (left.type == Type::REG && right.type == Type::CONST) {
		emitter.Emit4(OpCode::MULKR, dst, AddConst(right.value), left.reg);
	}
	if (left.type == Type::REG && right.type == Type::REG) {
		emitter.Emit4(OpCode::MULRR, dst, left.reg, right.reg);
	}
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
	uint16_t dst = regAlloc.Alloc();
	if (left.type == Type::CONST && right.type == Type::REG) {
		emitter.Emit4(OpCode::DIVKR, dst, AddConst(left.value), right.reg);
	}
	if (left.type == Type::REG && right.type == Type::CONST) {
		emitter.Emit4(OpCode::DIVRK, dst, left.reg, AddConst(right.value));
	}
	if (left.type == Type::REG && right.type == Type::REG) {
		emitter.Emit4(OpCode::DIVRR, dst, left.reg, right.reg);
	}
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
		emitter.Emit4(OpCode::CALL_BUILTIN, dst, funcId, AddConst(argNum));
		return ExprResult(dst);
	}
	Function func = funcTable.ResolveFunction(name->text);
	if (func.name != "") {
		uint16_t dst = regAlloc.Alloc();
		double argNum = ParseCall();
		emitter.Emit4(OpCode::CALL_FUNC, dst,Adduint32(func.pos),AddConst(argNum));
		return ExprResult(dst);
	}
	Error("The function is not defined");
	return ExprResult();
}
ExprResult Parser::LedAssign(ExprResult left) {
	Token* name = Peek(-1);
	Expect(TokenType::EQUAL);
	ExprResult right = ParseExpression((int)Precedense::PREC_ASSIGN - 1);
	Local local = CurrentLocals().ResolveLocal(name->text);
	if (local.name == "") {
		local = CurrentLocals().DefineLocal(name->text);
	}
	if (right.type == Type::CONST) {
		emitter.Emit4(OpCode::MOVK, local.slot, AddConst(right.value));
	}
	else if (right.type == Type::REG) {
		emitter.Emit4(OpCode::MOVR, local.slot, right.reg);
		regAlloc.DeleteReg(right.reg);
	}
	return ExprResult(local.slot);
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