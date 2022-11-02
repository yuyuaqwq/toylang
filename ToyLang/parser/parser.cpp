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

using lexer::TokenType;

using namespace ast;


ParserException::ParserException(const char* t_msg) : std::exception(t_msg) {

}


Parser::Parser(lexer::Lexer* t_lexer) : m_lexer(t_lexer) {

}

unique_ptr<Block> Parser::ParseSource() {
	return ParseBlock();
}

unique_ptr<Block> Parser::ParseBlock() {
	m_lexer->MatchToken(TokenType::kSepLCurly);

	vector<unique_ptr<Stat>> statList;

	auto token = m_lexer->LookAHead();
	while (!token.Is(TokenType::kSepRCurly)) {
		statList.push_back(ParseStat());
	}

	m_lexer->MatchToken(TokenType::kSepRCurly);

	return std::make_unique<Block>(std::move(statList));
}

unique_ptr<Stat> Parser::ParseStat() {
	auto token = m_lexer->LookAHead();

	switch (token.type) {
		case TokenType::kKwFunc: {
			return ParseFuncDefStat();
		}
		case TokenType::kKwIf: {
			return ParseIfStat();
		}
	}
	

	
}

unique_ptr<FuncDefStat> Parser::ParseFuncDefStat() {
	m_lexer->NextToken();
	auto funcName = m_lexer->MatchToken(TokenType::kIdent).str;
	auto parList = ParseParList();
	auto block = ParseBlock();
	return std::make_unique<ast::FuncDefStat>(funcName, parList, block);
}

std::vector<std::string> Parser::ParseParList() {
	std::vector<std::string> parList;
	if (m_lexer->LookAHead().Is(TokenType::kIdent)) {
		do {
			if (!m_lexer->LookAHead().Is(TokenType::kSepComma)) {
				break;
			}
			m_lexer->NextToken();
			parList.push_back(m_lexer->MatchToken(TokenType::kIdent).str);
		} while (true);
	}
	
	return parList;
}

unique_ptr<IfStat> Parser::ParseIfStat() {
	m_lexer->NextToken();
	auto exp = ParseExp();
	auto block = ParseBlock();

	vector<unique_ptr<ElifStat>> elifStatList;

	auto token = m_lexer->LookAHead();
	while (token.Is(TokenType::kKwElif)) {
		elifStatList.push_back(ParseElifStat());
	}
	
	unique_ptr<ElseStat> elseStat;
	if (token.Is(TokenType::kKwElse)) {
		elseStat = std::move(ParseElseStat());
	}

	return std::make_unique<ast::IfStat>();
}


unique_ptr<Exp> Parser::ParseExp() {

}


} // namespace parser
