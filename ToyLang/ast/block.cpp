#include "block.h"

#include "ast/stat.h"

namespace ast {

using std::unique_ptr;

Block::Block(std::vector<std::unique_ptr<Stat>>&& t_statList) : statList(std::move(t_statList)) {

}

} // namespace ast
