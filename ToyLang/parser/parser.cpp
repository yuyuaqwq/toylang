#include "parser.h"

/* EBNF
exp = addexp
addexp = mulexp {oper2 mulexp}
oper2 = '+' | '-'
mulexp = number {oper1 number}
oper1 = '*' | '/'
*/



namespace parser {

ParserException::ParserException(const char* t_msg) : std::exception(t_msg) {

}

using lexer::TokenType;
using ast::Exp;
using ast::AddExp;
using ast::MulExp;
using std::unique_ptr;


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
	auto token = m_lexer->MatchToken(TokenType::kNumber);
	int leftNumber = atoi(token.str.c_str());

	std::vector<TokenType> operList;
	std::vector<int> numberList;

	do {
		auto token = m_lexer->LookAHead();
		if (!token.Is(TokenType::kOpMul) && !token.Is(TokenType::kOpDiv)) {
			break;
		}

		m_lexer->NextToken();
		operList.push_back(token.type);
		token = m_lexer->MatchToken(TokenType::kNumber);
		numberList.push_back(atoi(token.str.c_str()));

	} while (true);

	return std::make_unique<MulExp>(leftNumber, operList, numberList);
}

} // namespace parser
