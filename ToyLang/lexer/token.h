#ifndef LEXER_TOKEN_H_
#define LEXER_TOKEN_H_

#include <string>
#include <map>


namespace lexer {

// token类型常量
enum class TokenType {
	kNil = 0,

	kEof,
	kNull,
	kFalse,
	kTrue,
	kNumber,
	kString,
	kIdent,

	kSepSemi,		// ;
	kSepComma,		// ,
	kSepDot,		// .
	kSepColon,		// :

	kSepLParen,     // (
	kSepRParen,     // )
	kSepLBrack,		// [
	kSepRBrack,		// ]
	kSepLCurly,		// {
	kSepRCurly,		// }

	kOpNewVar,    // :=
	kOpAssign, // =
	kOpAdd,    // +
	kOpSub,    // -
	kOpMul,    // *
	kOpDiv,    // /

	kOpEq,     // ==
	kOpLt,     // <
	kOpLe,     // <=
	kOpGt,     // >
	kOpGe,     // >=


	kKwFunc,  // func
	kKwIf,    // if
	kKwElif,  // elif
	kKwElse,  // else
	kKwWhile, // while
	kKwFor,   // for
	kKwContinue,  // continue
	kKwBreak,   // break
	kKwReturn,  // return


};

extern std::map<std::string, TokenType> g_keywords;

// 描述token的结构体
struct Token {
	bool Is(TokenType t_type) const noexcept;

	int line;			// 行号
	TokenType type;		// token类型
	std::string str;	// 保存必要的信息
};

} // namespace lexer

#endif // LEXER_TOKEN_H_