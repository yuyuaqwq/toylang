#include "lexer.h"

namespace lexer {

LexerException::LexerException(const char* t_msg) : std::exception(t_msg) {

}


Lexer::Lexer(const char* t_src) : m_src{ t_src }, m_line{ 0 }, m_idx{ 0 }, m_reserve{ 0, TokenType::kNil } {
        
}

Lexer::~Lexer() noexcept {

}

// 获取下一字符
char Lexer::NextChar() noexcept {
    if (m_idx < m_src.size()) {
        return m_src[m_idx++];
    }
    return 0;
}

// 跳过指定字符数
void Lexer::SkipChar(int count) noexcept {
    m_idx += count;
}


// 获取下一Token
Token Lexer::NextToken() {
    Token token;
    if (!m_reserve.Is(TokenType::kNil)) {        // 如果有前瞻保存的token
        // 返回前瞻的结果
        token = m_reserve;
        m_reserve.type = TokenType::kNil;
        return token;
    }

    char c;

    // 跳过空格
    while ((c = NextChar()) && c == ' ');

    token.line = m_line;

    if (c == 0) {
        token.type = TokenType::kEof;
        return token;
    }

    // 根据字符返回对应类型的Token
    switch (c) {
    case '+':
        token.type = TokenType::kOpAdd;
        return token;
    case '-':
        token.type = TokenType::kOpSub;
        return token;
    case '*':
        token.type = TokenType::kOpMul;
        return token;
    case '/':
        token.type = TokenType::kOpDiv;
        return token;
    case '(':
        token.type = TokenType::kSepLParen;
        return token;
    case ')':
        token.type = TokenType::kSepRParen;
        return token;
    }

    if (c >= '0' || c <= '9') {
        token.type = TokenType::kNumber;
        token.str.push_back(c);
        while (c = NextChar()) {
            if (c >= '0' && c <= '9') {
                token.str.push_back(c);
            }
            else {
                SkipChar(-1);
                break;
            }
        }
        return token;
    }
        
    throw LexerException("cannot parse token");
}

} // namespace lexer