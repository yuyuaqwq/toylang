#ifndef PARSER_PARSER_H_
#define PARSER_PARSER_H_

#include <memory>

#include "lexer/lexer.h"
#include "ast/block.h"
#include "ast/stat.h"
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
	std::unique_ptr<ast::Block> ParseSource();
	std::unique_ptr<ast::Block> ParseBlock();
	std::unique_ptr<ast::Stat> ParseStat();

	std::unique_ptr<ast::ExpStat> ParseExpStat();

	std::unique_ptr<ast::FuncDefStat> ParseFuncDefStat();
	std::vector<std::string> ParseParNameList();
	std::unique_ptr<ast::IfStat> ParseIfStat();
	std::unique_ptr<ast::ElifStat> ParseElifStat();
	std::unique_ptr<ast::ElseStat> ParseElseStat();

	std::unique_ptr<ast::ForStat> ParseForStat();
	std::unique_ptr<ast::WhileStat> ParseWhileStat();
	std::unique_ptr<ast::ContinueStat> ParseContinueStat();
	std::unique_ptr<ast::BreakStat> ParseBreakStat();
	std::unique_ptr<ast::ReturnStat> ParseReturnStat();

	std::unique_ptr<ast::NewVarStat> ParseNewVarStat();
	std::unique_ptr<ast::AssignStat> ParseAssignStat();

	std::unique_ptr<ast::Exp> ParseExp();
	std::unique_ptr<ast::Exp> ParseExp3();
	std::unique_ptr<ast::Exp> ParseExp2();
	std::unique_ptr<ast::Exp> ParseExp1();
	std::unique_ptr<ast::Exp> ParseExp0();
	std::vector<std::unique_ptr<ast::Exp>> ParseParExpList();
private:
	lexer::Lexer* m_lexer;
};

} // namespace parser
#endif // PARSER_PARSER_H_