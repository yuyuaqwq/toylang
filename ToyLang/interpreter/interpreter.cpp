#include "interpreter.h"

namespace interpreter {

int Interpreter::InterpretExp(ast::Exp* exp) const noexcept {
    return InterpretAddExp(static_cast<ast::AddExp*>(exp));
}

int Interpreter::InterpretAddExp(ast::AddExp* exp) const noexcept {
    int res = InterpretMulExp(exp->leftMulExp.get());
    for (int i = 0; i < exp->operList.size(); i++) {
        if (exp->operList[i] == lexer::TokenType::kOpAdd) {
            res += InterpretMulExp(exp->mulExpList[i].get());
        }
        else {
            res -= InterpretMulExp(exp->mulExpList[i].get());
        }
    }
    return res;
}

int Interpreter::InterpretMulExp(ast::MulExp* exp) const noexcept {
    int res = InterpretParenExp(exp->leftParenExp.get());
    for (int i = 0; i < exp->operList.size(); i++) {
        if (exp->operList[i] == lexer::TokenType::kOpMul) {
            res *= InterpretParenExp(exp->parenExpList[i].get());
        }
        else {
            res /= InterpretParenExp(exp->parenExpList[i].get());
        }
    }
    return res;
}
int Interpreter::InterpretParenExp(ast::ParenExp* exp) const noexcept {
    if (exp->exp->GetType() == ast::ExpType::kNum) {
        return InterpretNumberExp(static_cast<ast::NumExp*>(exp->exp.get()));
    }
    else {
        return InterpretAddExp(static_cast<ast::AddExp*>(exp->exp.get()));
    }
}

int Interpreter::InterpretNumberExp(ast::NumExp* exp) const noexcept {
    return exp->num;
}

} // namespace interpreter
