#include "exp.h"

namespace ast {

using std::unique_ptr;


ExpType NullExp::GetType() const noexcept {
	return ExpType::kNull;
}



ExpType BoolExp::GetType() const noexcept {
	return ExpType::kBool;
}
BoolExp::BoolExp(bool t_value) : value(t_value) {
}


ExpType NumberExp::GetType() const noexcept {
	return ExpType::kNumber;
}
NumberExp::NumberExp(int t_value) : value(t_value) {
}


ExpType StringExp::GetType() const noexcept {
	return ExpType::kString;
}
StringExp::StringExp(const std::string& t_value) : value(t_value) {
}


ExpType BinaOpExp::GetType() const noexcept {
	return ExpType::kBinaOp;
}

BinaOpExp::BinaOpExp(std::unique_ptr<Exp> t_leftExp, lexer::TokenType t_oper, std::unique_ptr<Exp> t_rightExp) : 
	leftExp(std::move(t_leftExp)), oper(t_oper), rightExp(std::move(t_rightExp)) {

}


ExpType NameExp::GetType() const noexcept {
	return ExpType::kName;
}

NameExp::NameExp(const std::string& t_name) :
	name(t_name) {
}




ExpType FuncCallExp::GetType() const noexcept {
	return ExpType::kFuncCall;
}

FuncCallExp::FuncCallExp(const std::string& t_name, std::vector<std::unique_ptr<ast::Exp>>&& t_parList) : name(t_name), parList(std::move(t_parList)){
}


} // namespace ast

