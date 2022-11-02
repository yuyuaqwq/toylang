#ifndef AST_BLOCK_H_
#define AST_BLOCK_H_

#include <vector>
#include <memory>

namespace ast {

struct Stat;
struct Block {
public:
	Block(std::vector<std::unique_ptr<Stat>>&& t_statList);

public:
	std::vector<std::unique_ptr<Stat>> statList;
};


} // namespace ast

#endif // AST_BLOCK_H_