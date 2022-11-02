#ifndef AST_EXP_H_
#define AST_EXP_H_

#include <vector>
#include <memory>

#include "lexer/token.h"

namespace ast {

enum class ExpType {
	kNull,
	kBool,
	kNumber,
	kString,


	kBinaOp,
	kName,
	kFuncCall,
};


struct Exp {
public:
	virtual ExpType GetType() const noexcept = 0;
};


struct NullExp : public Exp {
public:
	virtual ExpType GetType() const noexcept;
};


struct BoolExp : public Exp {
public:
	virtual ExpType GetType() const noexcept;

	BoolExp(bool t_value);

public:
	bool value;
};


struct NumberExp : public Exp {
public:
	virtual ExpType GetType() const noexcept;

	NumberExp(int t_value);

public:
	int value;
};


struct StringExp : public Exp {
public:
	virtual ExpType GetType() const noexcept;

	StringExp(const std::string& t_value);

public:
	std::string value;

};

struct BinaOpExp : public Exp {
public:
	virtual ExpType GetType() const noexcept;

	BinaOpExp(std::unique_ptr<Exp> t_leftExp, lexer::TokenType t_oper, std::unique_ptr<Exp> t_rightExp);

public:
	std::unique_ptr<Exp> leftExp;
	lexer::TokenType oper;
	std::unique_ptr<Exp> rightExp;
};

struct NameExp : public Exp {
public:
	virtual ExpType GetType() const noexcept;

	NameExp(const std::string& t_name);

public:
	std::string name;
};

struct FuncCallExp : public Exp {
public:
	virtual ExpType GetType() const noexcept;

	FuncCallExp(const std::string& t_name, std::vector<std::unique_ptr<ast::Exp>>&& t_parList);

public:
	std::string name;

	std::vector<std::unique_ptr<ast::Exp>> parList;

};



} // namespace ast

#endif // AST_EXP_H_