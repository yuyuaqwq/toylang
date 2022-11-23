#include "lexer/lexer.h"
#include "parser/parser.h"
#include "interpreter/interpreter.h"


int main() {
    using namespace lexer;
    using namespace parser;
    using namespace interpreter;

    Lexer lexer{ "1 + 33 - 0 * (33 / 999) + 123" };
    
    Parser parser(&lexer);

    auto exp = parser.ParseExp();

    Interpreter inter;
    
    int res = inter.InterpretExp(exp.get());

    printf("%d", res);
}
