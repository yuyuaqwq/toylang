#ifndef VM_VM_H_
#define VM_VM_H_

#include <stdint.h>

#include <vector>
#include <string>
#include <memory>


#include "vm/instr.h"
#include "vm/value.h"

namespace vm {

class VM {
public:
	void Run() {

	}

private:
	uint32_t m_pc;
	InstrSection m_instrSection;
	std::vector<std::unique_ptr<Value>> m_stackSection;
	std::vector<std::string> m_constSection;
};

} // namespace vm 

#endif // VM_VM_H_