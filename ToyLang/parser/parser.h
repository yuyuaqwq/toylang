#ifndef PARSER_PARSER_H_
#define PARSER_PARSER_H_

#include <memory>

#include "lexer/lexer.h"
#include "ast/stat.h"
#include "ast/exp.h"

namespace toylang {

// 语法分析时发生的异常
class ParserException : public std::exception {
public:
	ParserException(const char* t_msg);
};

class Parser {
public:
	Parser(Lexer* t_lexer);

public:
	std::unique_ptr<BlockStat> ParseSource();
	std::unique_ptr<BlockStat> ParseBlockStat();
	std::unique_ptr<Stat> ParseStat();

	std::unique_ptr<ExpStat> ParseExpStat();

	std::unique_ptr<FuncDefStat> ParseFuncDefStat();
	std::vector<std::string> ParseParNameList();
	std::unique_ptr<IfStat> ParseIfStat();
	std::unique_ptr<ElifStat> ParseElifStat();
	std::unique_ptr<ElseStat> ParseElseStat();

	std::unique_ptr<ForStat> ParseForStat();
	std::unique_ptr<WhileStat> ParseWhileStat();
	std::unique_ptr<ContinueStat> ParseContinueStat();
	std::unique_ptr<BreakStat> ParseBreakStat();
	std::unique_ptr<ReturnStat> ParseReturnStat();

	std::unique_ptr<NewVarStat> ParseNewVarStat();
	std::unique_ptr<AssignStat> ParseAssignStat();

	std::unique_ptr<Exp> ParseExp();
	std::unique_ptr<Exp> ParseExp3();
	std::unique_ptr<Exp> ParseExp2();
	std::unique_ptr<Exp> ParseExp1();
	std::unique_ptr<Exp> ParseExp0();
	std::vector<std::unique_ptr<Exp>> ParseParExpList();
private:
	Lexer* m_lexer;
};

} // namespace parser
#endif // PARSER_PARSER_H_