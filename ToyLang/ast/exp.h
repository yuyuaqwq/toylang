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

public:
	bool value;
};


struct NumberExp : public Exp {
public:
	virtual ExpType GetType() const noexcept;
public:
	int value;
};


struct StringExp : public Exp {
public:
	virtual ExpType GetType() const noexcept;

public:
	std::string value;

};

struct BinaOpExp : public Exp {
public:
	virtual ExpType GetType() const noexcept;

public:
	std::unique_ptr<Exp> leftExp;
	lexer::TokenType oper;
	std::unique_ptr<Exp> rightExp;
};

struct NameExp : public Exp {
public:
	virtual ExpType GetType() const noexcept;

public:
	std::string name;
};

struct FuncCallExp : public Exp {
public:
	virtual ExpType GetType() const noexcept;

public:
	std::string name;
};



} // namespace ast

#endif // AST_EXP_H_