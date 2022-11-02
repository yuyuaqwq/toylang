#include "stat.h"

namespace ast {



StatType IfStat::GetType() const noexcept {
	return StatType::kIf;
}


};



} // namespace ast

