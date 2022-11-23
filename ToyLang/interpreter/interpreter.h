#ifndef INTERPRETER_INTERPRETER_H_
#define INTERPRETER_INTERPRETER_H_

#include "ast/exp.h"

namespace interpreter {

class Interpreter{
public:
    int InterpretExp(ast::Exp* exp) const noexcept;
    int InterpretAddExp(ast::AddExp* exp) const noexcept;
    int InterpretMulExp(ast::MulExp* exp) const noexcept;
    int InterpretParenExp(ast::ParenExp* exp) const noexcept;
    int InterpretNumberExp(ast::NumExp* exp) const noexcept;
};

} // namespace interpreter

#endif // INTERPRETER_INTERPRETER_H