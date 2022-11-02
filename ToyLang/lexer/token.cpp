#include "token.h"

namespace lexer {

bool Token::Is(TokenType t_type) const noexcept {
	return t_type == type;
}


std::map<std::string, TokenType> g_keywords = {
	{ "if", TokenType::kKwIf },
	{ "elif", TokenType::kKwElif },
	{ "else", TokenType::kKwElse },
	{ "func", TokenType::kKwFunc },
	{ "for", TokenType::kKwFor },
	{ "while", TokenType::kKwWhile },
	{ "continue", TokenType::kKwContinue },
	{ "break", TokenType::kKwBreak },
	{ "return", TokenType::kKwReturn },
};


} // namespace lexer