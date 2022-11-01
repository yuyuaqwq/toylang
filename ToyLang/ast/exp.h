#ifndef AST_EXP_H_
#define AST_EXP_H_

#include <vector>
#include <memory>

#include "lexer/token.h"

namespace ast {

enum class ExpType {
	kAdd,
	kMul,
	kNum,
	kParen,
};


struct Exp {
public:
	virtual ExpType GetType() const noexcept = 0;
};


struct MulExp;
struct AddExp : public Exp {
public:
	virtual ExpType GetType() const noexcept;

	AddExp(std::unique_ptr<MulExp> t_leftMulExp, const std::vector<lexer::TokenType>& t_operList, std::vector<std::unique_ptr<MulExp>>&& t_mulExpList);

public:
	std::unique_ptr<MulExp> leftMulExp;
	std::vector<lexer::TokenType> operList;
	std::vector<std::unique_ptr<MulExp>> mulExpList;
};


struct ParenExp;
struct MulExp : public Exp {
public:
	virtual ExpType GetType() const noexcept;

	MulExp(std::unique_ptr<ParenExp> t_leftParenExp, const std::vector<lexer::TokenType>& t_operList, std::vector<std::unique_ptr<ParenExp>>&& t_parenExpList);

public:
	std::unique_ptr<ParenExp> leftParenExp;
	std::vector<lexer::TokenType> operList;
	std::vector<std::unique_ptr<ParenExp>> parenExpList;
};


struct ParenExp : public Exp {
public:
	virtual ExpType GetType() const noexcept;

	ParenExp(std::unique_ptr<Exp> texp);

public:
	std::unique_ptr<Exp> exp;

};

struct NumExp : public Exp {
public:
	virtual ExpType GetType() const noexcept;
	NumExp(int t_num);

public:
	int num;
};

} // namespace ast

#endif // AST_EXP_H_