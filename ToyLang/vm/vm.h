#ifndef VM_VM_H_
#define VM_VM_H_

#include <stdint.h>

#include <vector>
#include <string>
#include <memory>


#include "vm/instr.h"

#include "value/value.h"
#include "value/section.h"

namespace codegener{
class CodeGener;
}

namespace vm {

class VMException : public std::exception {
public:
	VMException(const char* t_msg);
};


class VM {
public:
	friend class codegener::CodeGener;

public:
	VM(value::ValueSection* t_constSect);

	std::string Disassembly();

	value::Value* GetVar(uint32_t idx);

	std::unique_ptr<value::Value> GetVarCopy(uint32_t idx);

	void SetVar(uint32_t idx, std::unique_ptr<value::Value> var);

	void SetVar(uint32_t idx, value::Value* var);


	void Run();

private:
	uint32_t m_pc;
	value::FunctionBodyValue* m_curFunc;
	value::ValueSection* m_constSect;

	value::ValueSection m_stackSect;
	// std::vector<std::unique_ptr<Value>> m_stackSect;
	
	
};

} // namespace vm 

#endif // VM_VM_H_