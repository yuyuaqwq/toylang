#include "parser.h"

/* EBNF
exp = addexp
addexp = mulexp {oper2 mulexp}
oper2 = '+' | '-'
mulexp = numexp {oper1 numexp}
oper1 = '*' | '/'
numexp = number
*/



namespace parser {

using lexer::TokenType;
using ast::Exp;
using ast::AddExp;
using ast::MulExp;
using ast::NumExp;
using std::unique_ptr;

ParserException::ParserException(const char* t_msg) : std::exception(t_msg) {

}


Parser::Parser(lexer::Lexer* t_lexer) : m_lexer(t_lexer) {

}

unique_ptr<Exp> Parser::ParseExp() {
	return ParseAddExp();
}

unique_ptr<AddExp> Parser::ParseAddExp() {
	auto leftMulExp = ParseMulExp();

	std::vector<TokenType> operList;
	std::vector<unique_ptr<MulExp>> mulExpList;

	do {
		auto token = m_lexer->LookAHead();
		if (token.Is(TokenType::kEof)) {
			break;
		}

		if (!token.Is(TokenType::kOpAdd) && !token.Is(TokenType::kOpSub)) {
			throw ParserException("Invalid symbol found when parsing addition expression");
		}

		m_lexer->NextToken();
		operList.push_back(token.type);
		mulExpList.push_back(ParseMulExp());

	} while (true);

	return std::make_unique<AddExp>(std::move(leftMulExp), operList, std::move(mulExpList));
}

unique_ptr<MulExp> Parser::ParseMulExp() {
	auto leftNumExp = ParseNumExp();

	std::vector<TokenType> operList;
	std::vector<unique_ptr<NumExp>> numExpList;

	do {
		auto token = m_lexer->LookAHead();
		if (!token.Is(TokenType::kOpMul) && !token.Is(TokenType::kOpDiv)) {
			break;
		}

		m_lexer->NextToken();
		operList.push_back(token.type);
		numExpList.push_back(ParseNumExp());

	} while (true);

	return std::make_unique<MulExp>(std::move(leftNumExp), operList, std::move(numExpList));
}

unique_ptr<NumExp> Parser::ParseNumExp() {
	auto token = m_lexer->MatchToken(TokenType::kNumber);
	int num = atoi(token.str.c_str());
	return std::make_unique<NumExp>(num);
}

} // namespace parser
