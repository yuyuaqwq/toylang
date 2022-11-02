#include "lexer/lexer.h"
#include "parser/parser.h"
#include "ast/exp.h"


int main() {

  

    using namespace lexer;
    using namespace parser;

    //Lexer lexer{ "1 + 33 - 0 * (33 / 999) - 123" };
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



    Lexer lexer{ R"(printf("hello");)" };
    Parser parser(&lexer);

    auto src = parser.ParseSource();

    

    //auto exp = parser.ParseExp();

    //int res = CalculationExp(exp.get());

    // printf("%d", res);
}
