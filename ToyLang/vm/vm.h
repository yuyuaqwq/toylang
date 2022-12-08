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

namespace toylang {

class VMException : public std::exception {
public:
	VMException(const char* t_msg);
};


class VM {
public:
	friend class codegener::CodeGener;

public:
	explicit VM(ValueSection* t_constSect);

public:
	std::string Disassembly();
	void Run();

private:
	Value* GetVar(uint32_t idx);
	std::unique_ptr<Value> GetVarCopy(uint32_t idx);
	void SetVar(uint32_t idx, std::unique_ptr<Value> var);
	void SetVar(uint32_t idx, Value* var);

private:
	uint32_t m_pc;
	FunctionBodyValue* m_curFunc;
	ValueSection* m_constSect;
	ValueSection m_stackSect;
	// std::vector<std::unique_ptr<Value>> m_stackSect;
	
};

} // namespace vm 

#endif // VM_VM_H_