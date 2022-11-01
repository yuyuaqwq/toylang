#ifndef AST_EXP_H_
#define AST_EXP_H_

#include <vector>
#include <memory>

#include "lexer/token.h"

namespace ast {

enum class ExpType {
	kAddExp,
	kMulExp
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


struct MulExp : public Exp {
public:
	virtual ExpType GetType() const noexcept;

	MulExp(int t_leftNumber, const std::vector<lexer::TokenType>& t_operList, const std::vector<int>& t_numberList);

public:
	int leftNumber;
	std::vector<lexer::TokenType> operList;
	std::vector<int> numberList;
};

} // namespace ast

#endif // AST_EXP_H_