#ifndef AST_STAT_H_
#define AST_STAT_H_

#include <vector>
#include <memory>
#include <string>

#include "lexer/token.h"
#include "ast/block.h"
#include "ast/exp.h"

namespace ast {

enum class StatType {
	kExp,
	kFuncDef,
	kIf,
	kElif,
	kElse,
	kFor,
	kWhile,
	kContinue,
	kBreak,
	kReturn,
	kNewVar,
	kAssign,
};


struct Stat {
public:
	virtual StatType GetType() const noexcept = 0;
};

struct ExpStat : public Stat {
public:
	virtual StatType GetType() const noexcept;

	ExpStat(std::unique_ptr<Exp> t_exp);

public:
	std::unique_ptr<Exp> exp;
};


struct FuncDefStat : public Stat {
public:
	virtual StatType GetType() const noexcept;

	FuncDefStat(const std::string& t_funcName, const std::vector<std::string>& t_parList, std::unique_ptr<Block> t_block);

public:
	std::string funcName;
	std::vector<std::string> parList;
	std::unique_ptr<Block> block;
};

struct ElifStat;
struct ElseStat;
struct IfStat : public Stat {
public:
	virtual StatType GetType() const noexcept;

	IfStat(std::unique_ptr<Exp> t_exp, std::unique_ptr<Block> t_block, std::vector<std::unique_ptr<ElifStat>>&& t_elifStatList, std::unique_ptr<ElseStat> t_elseStat);

public:
	std::unique_ptr<Exp> exp;
	std::unique_ptr<Block> block;
	std::vector<std::unique_ptr<ElifStat>> elifStatList;
	std::unique_ptr<ElseStat> elseStat;
};


struct ElifStat : public Stat {
public:
	virtual StatType GetType() const noexcept;

	ElifStat(std::unique_ptr<Exp> t_exp, std::unique_ptr<Block> t_block);

public:
	std::unique_ptr<Exp> exp;
	std::unique_ptr<Block> block;
};


struct ElseStat : public Stat {
public:
	virtual StatType GetType() const noexcept;

	ElseStat(std::unique_ptr<Block> t_block);

public:
	std::unique_ptr<Block> block;
};


struct ForStat : public Stat {
public:
	virtual StatType GetType() const noexcept;

	ForStat(const std::string& t_varName, std::unique_ptr<Exp> t_exp ,std::unique_ptr<Block> t_block);

public:
	std::string varName;
	std::unique_ptr<Exp> exp;
	std::unique_ptr<Block> block;
};


struct WhileStat : public Stat {
public:
	virtual StatType GetType() const noexcept;

	WhileStat(std::unique_ptr<Exp> t_exp, std::unique_ptr<Block> t_block);

public:
	std::unique_ptr<Exp> exp;
	std::unique_ptr<Block> block;
};


struct ContinueStat : public Stat {
public:
	virtual StatType GetType() const noexcept;

public:

};


struct BreakStat : public Stat {
public:
	virtual StatType GetType() const noexcept;

public:

};


struct ReturnStat : public Stat {
public:
	virtual StatType GetType() const noexcept;

	ReturnStat(std::unique_ptr<Exp> t_exp);

public:
	std::unique_ptr<Exp> exp;
};


struct NewVarStat : public Stat {
public:
	virtual StatType GetType() const noexcept;

	NewVarStat(const std::string& t_varName, std::unique_ptr<Exp> t_exp);

public:
	std::string varName;
	std::unique_ptr<Exp> exp;
};


struct AssignStat : public Stat {
public:
	virtual StatType GetType() const noexcept;

	AssignStat(const std::string& t_varName, std::unique_ptr<Exp> t_exp);

public:
	std::string varName;
	std::unique_ptr<Exp> exp;
};


} // namespace ast

#endif // AST_STAT_H_