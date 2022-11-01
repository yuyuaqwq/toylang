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

MulExp::MulExp(unique_ptr<NumExp> t_leftNumExp, const std::vector<lexer::TokenType>& t_operList, std::vector<unique_ptr<NumExp>>&& t_numExoList) :
	leftNumExp(std::move(t_leftNumExp)), operList(t_operList), numExpList(std::move(t_numExoList)) {

}


ExpType NumExp::GetType() const noexcept {
	return ExpType::kNum;
}

NumExp::NumExp(int t_num) : num(t_num) {

}

} // namespace ast

