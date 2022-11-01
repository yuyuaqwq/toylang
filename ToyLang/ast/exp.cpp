#include "exp.h"

namespace ast {

using std::unique_ptr;


ExpType AddExp::GetType() const noexcept {
	return ExpType::kAdd;
}

AddExp::AddExp(unique_ptr<MulExp> t_leftMulExp, const std::vector<lexer::TokenType>& t_operList, std::vector<unique_ptr<MulExp>>&& t_mulExpList) :
	leftMulExp(std::move(t_leftMulExp)), operList(t_operList), mulExpList(std::move(t_mulExpList)) {

}


ExpType MulExp::GetType() const noexcept {
	return ExpType::kMul;
}

MulExp::MulExp(unique_ptr<ParenExp> t_leftParenExp, const std::vector<lexer::TokenType>& t_operList, std::vector<unique_ptr<ParenExp>>&& t_parenExoList) :
	leftParenExp(std::move(t_leftParenExp)), operList(t_operList), parenExpList(std::move(t_parenExoList)) {

}


ExpType ParenExp::GetType() const noexcept {
	return ExpType::kParen;
}

ParenExp::ParenExp(unique_ptr<Exp> t_exp) : exp(std::move(t_exp)) {

}


ExpType NumExp::GetType() const noexcept {
	return ExpType::kNum;
}

NumExp::NumExp(int t_num) : num(t_num) {

}

} // namespace ast

