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

using lexer::TokenType;
using ast::Exp;
using ast::AddExp;
using ast::MulExp;
using ast::ParenExp;
using ast::NumExp;
using std::unique_ptr;

ParserException::ParserException(const char* t_msg) : std::exception(t_msg) {

}


Parser::Parser(lexer::Lexer* t_lexer) : m_lexer(t_lexer) {

}

unique_ptr<Exp> Parser::ParseExp() {
	auto exp = ParseAddExp();
	if (!m_lexer->LookAHead().Is(TokenType::kEof)) {
		throw ParserException("Incomplete parsing");
	}
	return exp;
}

unique_ptr<AddExp> Parser::ParseAddExp() {
	auto leftMulExp = ParseMulExp();

	std::vector<TokenType> operList;
	std::vector<unique_ptr<MulExp>> mulExpList;

	do {
		auto token = m_lexer->LookAHead();
		if (!token.Is(TokenType::kOpAdd) && !token.Is(TokenType::kOpSub)) {
			break;
		}

		m_lexer->NextToken();
		operList.push_back(token.type);
		mulExpList.push_back(ParseMulExp());

	} while (true);

	return std::make_unique<AddExp>(std::move(leftMulExp), operList, std::move(mulExpList));
}

unique_ptr<MulExp> Parser::ParseMulExp() {
	auto leftParenExp = ParseParenExp();

	std::vector<TokenType> operList;
	std::vector<unique_ptr<ParenExp>> parenExpList;

	do {
		auto token = m_lexer->LookAHead();
		if (!token.Is(TokenType::kOpMul) && !token.Is(TokenType::kOpDiv)) {
			break;
		}

		m_lexer->NextToken();
		operList.push_back(token.type);
		parenExpList.push_back(ParseParenExp());

	} while (true);

	return std::make_unique<MulExp>(std::move(leftParenExp), operList, std::move(parenExpList));
}

unique_ptr<ParenExp> Parser::ParseParenExp() {
	auto token = m_lexer->LookAHead();
	if (!token.Is(TokenType::kSepLParen)) {		// (
		return std::make_unique<ParenExp>(ParseNumExp());
	}
	m_lexer->NextToken();
	auto exp = ParseAddExp();
	m_lexer->MatchToken(TokenType::kSepRParen);		// )
	return std::make_unique<ParenExp>(std::move(exp));
}

unique_ptr<NumExp> Parser::ParseNumExp() {
	auto token = m_lexer->MatchToken(TokenType::kNumber);
	int num = atoi(token.str.c_str());
	return std::make_unique<NumExp>(num);
}

} // namespace parser
