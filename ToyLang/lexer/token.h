#ifndef LEXER_TOKEN_H_
#define LEXER_TOKEN_H_

#include <string>

namespace lexer {

// token类型常量
enum class TokenType {
	kNil = 0,

	kEof,
	kNumber,

	kOpAdd,    // +
	kOpSub,    // -
	kOpMul,    // *
	kOpDiv,    // /

	kSepLParen,  // (
	kSepRParen,  // )
};

// 描述token的结构体
struct Token {
	bool Is(TokenType t_type) const noexcept;

	int line;			// 行号
	TokenType type;		// token类型
	std::string str;	// 保存必要的信息
};

} // namespace lexer

#endif // LEXER_TOKEN_H_