#include "exp.h"

namespace ast {

using std::unique_ptr;


ExpType NullExp::GetType() const noexcept {
	return ExpType::kNull;
}


ExpType BoolExp::GetType() const noexcept {
	return ExpType::kBool;
}


ExpType NumberExp::GetType() const noexcept {
	return ExpType::kNumber;
}


ExpType StringExp::GetType() const noexcept {
	return ExpType::kString;
}


ExpType BinaOpExp::GetType() const noexcept {
	return ExpType::kBinaOp;
}

ExpType NameExp::GetType() const noexcept {
	return ExpType::kName;
}

ExpType FuncCallExp::GetType() const noexcept {
	return ExpType::kFuncCall;
}


} // namespace ast

