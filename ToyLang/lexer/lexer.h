#ifndef LEXER_LEXER_H_
#define LEXER_LEXER_H_

#include <string>
#include <exception>

#include <lexer/token.h>

namespace lexer {

// 词法分析时发生的异常
class LexerException : public std::exception {
public:
	LexerException(const char* t_msg);
};

// 词法分析器类
class Lexer {
public:
	Lexer(const char* t_src);
	~Lexer() noexcept;

private:
	char NextChar() noexcept;
	void SkipChar(int count) noexcept;

public:
	Token NextToken();

private:
	std::string m_src;
	size_t m_idx;
	Token m_reserve;
	int m_line;
};

} // namespace lexer

#endif // LEXER_LEXER_H_