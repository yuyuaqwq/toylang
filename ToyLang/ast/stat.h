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
	kIf,
	kElif,
	kElse,

};


struct Stat {
public:
	virtual StatType GetType() const noexcept = 0;
};

struct FuncDefStat : public Stat {
public:
	virtual StatType GetType() const noexcept;

public:
	std::string funcName;
	std::vector<std::string> parList;
	std::unique_ptr<Block> block;
};


struct IfStat : public Stat {
public:
	virtual StatType GetType() const noexcept;

public:
	std::unique_ptr<Exp> exp;
	std::unique_ptr<Block> block;
	std::vector<std::unique_ptr<ElifStat>> elifStatList;
	std::unique_ptr<ElseStat> elseStat;
};

struct ElifStat : public Stat {
public:
	virtual StatType GetType() const noexcept;

public:
	std::unique_ptr<Exp> exp;
	std::unique_ptr<Block> block;
};

struct ElseStat : public Stat {
public:
	virtual StatType GetType() const noexcept;

public:
	std::unique_ptr<Block> block;
};

} // namespace ast

#endif // AST_STAT_H_