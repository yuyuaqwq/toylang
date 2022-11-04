#include "lexer.h"

namespace lexer {

LexerException::LexerException(const char* t_msg) : std::exception(t_msg) {

}


Lexer::Lexer(const char* t_src) : m_src{ t_src }, m_line{ 0 }, m_idx{ 0 }, m_save{ 0, TokenType::kNil } {
        
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

bool Lexer::TestStr(const std::string& str) {
    return !m_src.compare(m_idx, str.size(), str);
}

bool Lexer::TestChar(char c) {
    return m_src[m_idx] == c;
}


// 前瞻下一Token
Token Lexer::LookAHead() {
    if (m_save.type == TokenType::kNil) {        // 如果没有前瞻过
        m_save = NextToken();       // 获取
    }
    return m_save;
}


inline static bool IsDigit(char c) {
    return c >= '0' && c <= '9';
}
inline static bool IsAlpha(char c) {
    return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z';
}

// 获取下一Token
Token Lexer::NextToken() {
    Token token;
    if (!m_save.Is(TokenType::kNil)) {        // 如果有前瞻保存的token
        // 返回前瞻的结果
        token = m_save;
        m_save.type = TokenType::kNil;
        return token;
    }

    char c;

    // 跳过空格
    while ((c = NextChar()) && (c == ' ' || c == '\n'));

    token.line = m_line;

    if (c == 0) {
        token.type = TokenType::kEof;
        return token;
    }

    // 根据字符返回对应类型的Token
    switch (c) {
    case ';':
        token.type = TokenType::kSepSemi;
        return token;
    case ',':
        token.type = TokenType::kSepComma;
        return token;
    case ':':
        if (TestChar('=')) {
            SkipChar(1);
            token.type = TokenType::kOpNewVar;
            return token;
        }
        token.type = TokenType::kSepColon;
        return token;
    case '(':
        token.type = TokenType::kSepLParen;
        return token;
    case ')':
        token.type = TokenType::kSepRParen;
        return token;
    case '[':
        token.type = TokenType::kSepLBrack;
        return token;
    case ']':
        token.type = TokenType::kSepRBrack;
        return token;
    case '{':
        token.type = TokenType::kSepLCurly;
        return token;
    case '}':
        token.type = TokenType::kSepRCurly;
        return token;

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

    case '=':
        if (TestChar('=')) {
            SkipChar(1);
            token.type = TokenType::kOpEq;
            return token;
        }
        token.type = TokenType::kOpAssign;
        return token;
    case '<':
        if (TestChar('=')) {
            SkipChar(1);
            token.type = TokenType::kOpLe;
            return token;
        }
        token.type = TokenType::kOpLt;
        return token;

    case '>':
        if (TestChar('=')) {
            SkipChar(1);
            token.type = TokenType::kOpGe;
            return token;
        }
        token.type = TokenType::kOpGt;
        return token;

    }

    
    if (c == 'n' && TestStr("ull")) {
        SkipChar(2);
        token.type = TokenType::kNull;
    }
    if (c == 'f' && TestStr("alse")) {
        SkipChar(4);
        token.type = TokenType::kFalse;
    }
    if (c == 't' && TestStr("rue")) {
        SkipChar(3);
        token.type = TokenType::kFalse;
    }

    // Number
    if (IsDigit(c)) {
        token.type = TokenType::kNumber;
        token.str.push_back(c);
        while (c = NextChar()) {
            if (IsDigit(c)) {
                token.str.push_back(c);
            }
            else {
                SkipChar(-1);
                break;
            }
        }
        return token;
    }

    // String
    if (c == '\"') {
        size_t beginPos = m_idx;
        size_t endPos = -1;
        if (c == '\'') {
            endPos = m_src.find('\'', m_idx);
        }
        else if (c == '\"') {
            endPos = m_src.find('\"', m_idx);
        }

        if (endPos == -1) {
            throw LexerException("incorrect short string");
        }

        m_idx = endPos + 1;

        token.str = m_src.substr(beginPos, endPos - beginPos);
        token.type = TokenType::kString;
        return token;
    }

    // 标识符或关键字
    if (c == '_' || IsAlpha(c)) {
        
        std::string ident;
        size_t beginPos = m_idx - 1;
        char c = NextChar();
        while (c && (c == '_' || IsAlpha(c) || IsDigit(c))) {
            c = NextChar();
        }
        m_idx--;

        size_t endPos = m_idx;

        ident = m_src.substr(beginPos, endPos - beginPos);

        auto keyword = g_keywords.find(ident);
        if (keyword != g_keywords.end()) {
            token.type = keyword->second;
        }
        else {
            token.type = TokenType::kIdent;
            token.str = ident;
        }
        return token;

    }

    throw LexerException("cannot parse token");
}


// 匹配下一Token
Token Lexer::MatchToken(TokenType type) {
    auto token = NextToken();
    if (token.Is(type)) {
        return token;
    }
    throw LexerException("cannot match token");
}

} // namespace lexer