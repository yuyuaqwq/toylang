#include "exp.h"

namespace ast {

AddExp::AddExp(std::unique_ptr<MulExp> t_leftMulExp, const std::vector<lexer::TokenType>& t_operList, std::vector<std::unique_ptr<MulExp>>&& t_mulExpList) :
	leftMulExp(std::move(t_leftMulExp)), operList(t_operList), mulExpList(std::move(t_mulExpList)) {

}

ExpType AddExp::GetType() const noexcept {
	return ExpType::kAddExp;
}


MulExp::MulExp(int t_leftNumber, const std::vector<lexer::TokenType>& t_operList, const std::vector<int>& t_numberList) :
	leftNumber(t_leftNumber), operList(t_operList), numberList(t_numberList) {

}

ExpType MulExp::GetType() const noexcept {
	return ExpType::kMulExp;
}

} // namespace ast

