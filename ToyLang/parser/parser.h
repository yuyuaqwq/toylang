#ifndef PARSER_PARSER_H_
#define PARSER_PARSER_H_

#include <memory>

#include "lexer/lexer.h"
#include "ast/exp.h"

namespace parser {

// 语法分析时发生的异常
class ParserException : public std::exception {
public:
	ParserException(const char* t_msg);
};

class Parser {
public:
	Parser(lexer::Lexer* t_lexer);

public:
	std::unique_ptr<ast::Exp> ParseExp();

	std::unique_ptr<ast::AddExp> ParseAddExp();

	std::unique_ptr<ast::MulExp> ParseMulExp();


private:
	lexer::Lexer* m_lexer;
};

} // namespace parser
#endif // PARSER_PARSER_H_