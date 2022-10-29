#include "token.h"

namespace lexer {

bool Token::Is(TokenType t_type) const noexcept {
	return t_type == type;
}

} // namespace lexer