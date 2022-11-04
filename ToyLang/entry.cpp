#include <fstream>



#include "lexer/lexer.h"
#include "parser/parser.h"
#include "ast/exp.h"
#include "codegener/codegener.h"
#include "vm/vm.h"





int main() {

  

    using namespace lexer;
    using namespace parser;
    using namespace codegener;
    using namespace vm;

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

    std::fstream srcFile;
    srcFile.open(R"(C:\Users\GT1\Desktop\test.toy)");

    srcFile.seekg(0, std::ios::end);
    std::streampos length = srcFile.tellg();
    srcFile.seekg(0, std::ios::beg);
    std::vector<char> res(length);
    srcFile.read((char*)res.data(), length);

    Lexer lexer{ res.data() };
    Parser parser(&lexer);

    auto src = parser.ParseSource();

    
    VM vvm;
    CodeGener cg(&vvm);

    cg.RegistryFunctionBridge("println", 
        [](uint32_t parCount, std::vector<std::unique_ptr<Value>>* stack)->std::unique_ptr<Value> {       // Toy_Println
            auto val = stack->operator[](stack->size() - 1).get();
            if (val->GetType() == vm::ValueType::kString) {
                printf("%s\n", stack->operator[](stack->size() - 1)->GetString()->value.c_str());
            }
            else if (val->GetType() == vm::ValueType::kNumber) {
                printf("%lld\n", stack->operator[](stack->size() - 1)->GetNumber()->value);
            }
            stack->pop_back();
            return nullptr;
        }
    );


    cg.Generate(src.get());

    printf("%s\n", vvm.Disassembly().c_str());

    vvm.Run();

    //auto exp = parser.ParseExp();

    //int res = CalculationExp(exp.get());

    // printf("%d", res);
}
