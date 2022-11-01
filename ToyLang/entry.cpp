#include "lexer/lexer.h"
#include "parser/parser.h"
#include "ast/exp.h"

int CalculationNumberExp(ast::NumExp* exp) {
    return exp->num;
}
int CalculationAddExp(ast::AddExp* exp);
int CalculationParenExp(ast::ParenExp* exp) {
    if (exp->exp->GetType() == ast::ExpType::kNum) {
        return CalculationNumberExp(static_cast<ast::NumExp*>(exp->exp.get()));
    }
    else {
        return CalculationAddExp(static_cast<ast::AddExp*>(exp->exp.get()));
    }
}
int CalculationMulExp(ast::MulExp* exp) {
    int res = CalculationParenExp(exp->leftParenExp.get());
    for (int i = 0; i < exp->operList.size(); i++) {
        if (exp->operList[i] == lexer::TokenType::kOpMul) {
            res *= CalculationParenExp(exp->parenExpList[i].get());
        }
        else {
            res /= CalculationParenExp(exp->parenExpList[i].get());
        }
    }
    return res;
}
int CalculationAddExp(ast::AddExp* exp) {
    int res = CalculationMulExp(exp->leftMulExp.get());
    for (int i = 0; i < exp->operList.size(); i++) {
        if (exp->operList[i] == lexer::TokenType::kOpAdd) {
            res += CalculationMulExp(exp->mulExpList[i].get());
        }
        else {
            res -= CalculationMulExp(exp->mulExpList[i].get());
        }
    }
    return res;
}
int CalculationExp(ast::Exp* exp) {
    return CalculationAddExp(static_cast<ast::AddExp*>(exp));
}


int main() {
    using namespace lexer;
    using namespace parser;

    Lexer lexer{ "1 + 33 - 0 * (33 / 999) - 123" };
    /*do {
        auto token = lexer.NextToken();
        if (token.Is(TokenType::kEof)) {
            break;
        }

        switch (token.type) {
        case TokenType::kNumber:{
            printf("%s\n", token.str.c_str()); break;
        }
        case TokenType::kOpAdd: {
            printf("+\n"); break;
        }
        case TokenType::kOpDiv: {
            printf("/\n"); break;
        }
        case TokenType::kOpMul: {
            printf("*\n"); break;
        }
        case TokenType::kOpSub: {
            printf("-\n"); break;
        }
        case TokenType::kSepLParen: {
            printf("(\n"); break;
        }
        case TokenType::kSepRParen: {
            printf(")\n"); break;
        }
        }
    } while (true);*/

    Parser parser(&lexer);

    auto exp = parser.ParseExp();

    int res = CalculationExp(exp.get());

    printf("%d", res);
}
