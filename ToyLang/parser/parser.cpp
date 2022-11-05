#include "parser.h"

/* EBNF
exp = addexp
addexp = mulexp {oper2 mulexp}
oper2 = '+' | '-'
mulexp = parenexp {oper1 parenexp}
oper1 = '*' | '/'
parenexp = '(' addexp ')' | numexp
numexp = number
*/

namespace parser {

using std::unique_ptr;
using std::vector;
using std::string;
using std::move;
using std::make_unique;
using lexer::TokenType;

using namespace ast;


ParserException::ParserException(const char* t_msg) : std::exception(t_msg) {

}


Parser::Parser(lexer::Lexer* t_lexer) : m_lexer(t_lexer) {

}


unique_ptr<BlockStat> Parser::ParseSource() {
	vector<unique_ptr<Stat>> statList;

	while (!m_lexer->LookAHead().Is(TokenType::kEof)) {
		statList.push_back(ParseStat());
	}

	return std::make_unique<BlockStat>(std::move(statList));
}

unique_ptr<BlockStat> Parser::ParseBlockStat() {
	m_lexer->MatchToken(TokenType::kSepLCurly);

	vector<unique_ptr<Stat>> statList;

	while (!m_lexer->LookAHead().Is(TokenType::kSepRCurly)) {
		statList.push_back(ParseStat());
	}

	m_lexer->MatchToken(TokenType::kSepRCurly);

	return std::make_unique<BlockStat>(std::move(statList));
}

unique_ptr<Stat> Parser::ParseStat() {
	auto token = m_lexer->LookAHead();

	switch (token.type) {
		case TokenType::kKwFunc: {
			return ParseFuncDefStat();
		}
		case TokenType::kSepLCurly: {
			return ParseBlockStat();
		}
		case TokenType::kKwIf: {
			return ParseIfStat();
		}
		case TokenType::kKwFor: {
			return ParseForStat();
		}
		case TokenType::kKwWhile: {
			return ParseWhileStat();
		}
		case TokenType::kKwContinue: {
			return ParseContinueStat();
		}
		case TokenType::kKwBreak: {
			return ParseBreakStat();
		}
		case TokenType::kKwReturn: {
			return ParseReturnStat();
		}
		case TokenType::kIdent: {
			auto temoLexer = *m_lexer;
			temoLexer.NextToken();
			auto token = temoLexer.LookAHead();
			if (token.Is(TokenType::kOpNewVar)){
				return ParseNewVarStat();
			}
			if (token.Is(TokenType::kOpAssign)) {
				return ParseAssignStat();
			}
		}
		default: {
			return ParseExpStat();
		}
	}
	

	
}

unique_ptr<ExpStat> Parser::ParseExpStat() {
	if (m_lexer->LookAHead().type == TokenType::kSepSemi) {
		m_lexer->NextToken();
		return std::make_unique<ast::ExpStat>(nullptr);
	}
	auto exp = ParseExp();
	m_lexer->MatchToken(TokenType::kSepSemi);
	return std::make_unique<ast::ExpStat>(move(exp));
}

unique_ptr<FuncDefStat> Parser::ParseFuncDefStat() {
	m_lexer->MatchToken(TokenType::kKwFunc);
	auto funcName = m_lexer->MatchToken(TokenType::kIdent).str;
	auto parList = ParseParNameList();
	auto block = ParseBlockStat();
	return std::make_unique<ast::FuncDefStat>(funcName, parList, std::move(block));
}

vector<string> Parser::ParseParNameList() {
	m_lexer->MatchToken(TokenType::kSepLParen);
	std::vector<std::string> parList;
	if (!m_lexer->LookAHead().Is(TokenType::kSepRParen)) {
		do {
			parList.push_back(m_lexer->MatchToken(TokenType::kIdent).str);
			if (!m_lexer->LookAHead().Is(TokenType::kSepComma)) {
				break;
			}
			m_lexer->NextToken();
		} while (true);
	}
	m_lexer->MatchToken(TokenType::kSepRParen);
	return parList;
}

unique_ptr<IfStat> Parser::ParseIfStat() {
	m_lexer->NextToken();
	auto exp = ParseExp();
	auto block = ParseBlockStat();

	vector<unique_ptr<ElifStat>> elifStatList;

	auto token = m_lexer->LookAHead();
	while (token.Is(TokenType::kKwElif)) {
		elifStatList.push_back(ParseElifStat());
		token = m_lexer->LookAHead();
	}
	
	unique_ptr<ElseStat> elseStat;
	if (token.Is(TokenType::kKwElse)) {
		elseStat = ParseElseStat();
	}

	return make_unique<IfStat>(std::move(exp), std::move(block), std::move(elifStatList), std::move(elseStat));
}

unique_ptr<ElifStat> Parser::ParseElifStat() {
	m_lexer->MatchToken(TokenType::kKwElif);
	auto exp = ParseExp();
	auto block = ParseBlockStat();
	return make_unique<ElifStat>(std::move(exp), std::move(block));
}

unique_ptr<ElseStat> Parser::ParseElseStat() {
	m_lexer->MatchToken(TokenType::kKwElse);
	auto block = ParseBlockStat();
	return make_unique<ElseStat>(std::move(block));
}

unique_ptr<ForStat> Parser::ParseForStat() {
	m_lexer->MatchToken(TokenType::kKwFor);
	auto varName = m_lexer->MatchToken(TokenType::kIdent).str;
	m_lexer->MatchToken(TokenType::kSepColon);
	auto exp = ParseExp();
	auto block = ParseBlockStat();
	return  make_unique<ForStat>(varName, move(exp), move(block));
}

unique_ptr<WhileStat> Parser::ParseWhileStat() {
	m_lexer->MatchToken(TokenType::kKwWhile);
	auto exp = ParseExp();
	auto block = ParseBlockStat();
	return make_unique<WhileStat>(move(exp), move(block));
}

unique_ptr<ContinueStat> Parser::ParseContinueStat() {
	m_lexer->MatchToken(TokenType::kKwContinue);
	m_lexer->MatchToken(TokenType::kSepSemi);
	return make_unique<ContinueStat>();
}

unique_ptr<BreakStat> Parser::ParseBreakStat() {
	m_lexer->MatchToken(TokenType::kKwBreak);
	m_lexer->MatchToken(TokenType::kSepSemi);
	return make_unique<BreakStat>();
}

unique_ptr<ReturnStat> Parser::ParseReturnStat() {
	m_lexer->MatchToken(TokenType::kKwReturn);
	unique_ptr<Exp> exp;
	if (m_lexer->LookAHead().type != TokenType::kSepSemi) {
		exp = ParseExp();
	}
	m_lexer->MatchToken(TokenType::kSepSemi);
	return make_unique<ReturnStat>(move(exp));
}

unique_ptr<NewVarStat> Parser::ParseNewVarStat() {
	auto varName = m_lexer->MatchToken(TokenType::kIdent).str;
	m_lexer->MatchToken(TokenType::kOpNewVar);
	auto exp = ParseExp();
	m_lexer->MatchToken(TokenType::kSepSemi);
	return  make_unique<NewVarStat>(varName, move(exp));
}

unique_ptr<AssignStat> Parser::ParseAssignStat() {
	auto varName = m_lexer->MatchToken(TokenType::kIdent).str;
	m_lexer->MatchToken(TokenType::kOpAssign);
	auto exp = ParseExp();
	m_lexer->MatchToken(TokenType::kSepSemi);
	return  make_unique<AssignStat>(varName, move(exp));
}


unique_ptr<Exp> Parser::ParseExp() {
	return ParseExp3();
}

unique_ptr<Exp> Parser::ParseExp3() {
	auto exp = ParseExp2();
	do {
		auto type = m_lexer->LookAHead().type;
		if (type != TokenType::kOpNe && type != TokenType::kOpEq && type != TokenType::kOpLt && type != TokenType::kOpLe && type != TokenType::kOpGt && type != TokenType::kOpGe) {
			break;
		}
		m_lexer->NextToken();
		exp = make_unique<BinaOpExp>(move(exp), type, ParseExp2());
	} while (true);
	return exp;
}

unique_ptr<Exp> Parser::ParseExp2() {
	auto exp = ParseExp1();
	
	do {
		auto type = m_lexer->LookAHead().type;
		if (type != TokenType::kOpAdd && type != TokenType::kOpSub) {
			break;
		}
		m_lexer->NextToken();
		exp = make_unique<BinaOpExp>(move(exp), type, ParseExp1());
	} while (true);
	return exp;
}

unique_ptr<Exp> Parser::ParseExp1() {
	auto exp = ParseExp0();
	do {
		auto type = m_lexer->LookAHead().type;
		if (type != TokenType::kOpMul && type != TokenType::kOpDiv) {
			break;
		}
		m_lexer->NextToken();
		exp = make_unique<BinaOpExp>(move(exp), type, ParseExp0());
	} while (true);
	return exp;
}

unique_ptr<Exp> Parser::ParseExp0() {
	auto token = m_lexer->LookAHead();

	switch (token.type) {
	case TokenType::kNull: {
		m_lexer->NextToken();
		return make_unique<NullExp>();
	}
	case TokenType::kTrue: {
		m_lexer->NextToken();
		return make_unique<BoolExp>(true);
	}
	case TokenType::kFalse: {
		m_lexer->NextToken();
		return make_unique<BoolExp>(false);
	}
	case TokenType::kNumber: {
		m_lexer->NextToken();
		return make_unique<NumberExp>(atoi(token.str.c_str()));
	}
	case TokenType::kString: {
		m_lexer->NextToken();
		return make_unique<StringExp>(token.str);
	}
	case TokenType::kIdent: {
		m_lexer->NextToken();
		if (m_lexer->LookAHead().Is(TokenType::kSepLParen)) {
			auto funcName = token.str;
			auto parList = ParseParExpList();
			return make_unique<FuncCallExp>(funcName, move(parList));
		}
		return make_unique<NameExp>(token.str);
	}
	default: {
		throw ParserException("Unable to parse expression");
	}
	}

}

vector<unique_ptr<Exp>> Parser::ParseParExpList() {
	m_lexer->MatchToken(TokenType::kSepLParen);
	vector<unique_ptr<Exp>> parList;
	if (!m_lexer->LookAHead().Is(TokenType::kSepRParen)) {
		do {
			parList.push_back(ParseExp());
			if (!m_lexer->LookAHead().Is(TokenType::kSepComma)) {
				break;
			}
			m_lexer->NextToken();
		} while (true);
	}
	m_lexer->MatchToken(TokenType::kSepRParen);
	return parList;
}


} // namespace parser
