#ifndef VM_VM_H_
#define VM_VM_H_

#include <stdint.h>

#include <vector>
#include <string>
#include <memory>


#include "vm/instr.h"
#include "vm/value.h"

namespace codegener{
class CodeGener;
}

namespace vm {

class VMException : public std::exception {
public:
	VMException(const char* t_msg) : std::exception(t_msg) {

	}
};


class VM {
public:
	friend class codegener::CodeGener;

public:
	VM() : m_mainFunc{ 0 }, m_varIdxBase { 0 }, m_pc{ 0 }, m_curFunc{ 0 }{
		m_curFunc = &m_mainFunc;
	}

	std::string Disassembly() {
		return m_curFunc->Disassembly();
	}

	Value* GetVar(uint32_t idx) {
		idx += m_varIdxBase;
		return m_varSect[idx].get();
	}

	std::unique_ptr<Value> GetVarCopy(uint32_t idx) {
		idx += m_varIdxBase;
		return m_varSect[idx]->Copy();
	}

	void SetVar(uint32_t idx, std::unique_ptr<Value> var) {
		idx += m_varIdxBase;
		if (idx >= m_varSect.size()) {
			m_varSect.resize(idx + 1);
		}
		m_varSect[idx] = std::move(var);
	}

	void SetVar(uint32_t idx, Value* var) {
		idx += m_varIdxBase;
		if (idx >= m_varSect.size()) {
			m_varSect.resize(idx + 1);
		}
		m_varSect[idx] = var->Copy();
	}


	void Run() {
		do {
			auto opcode = m_curFunc->instrSect.GetOpcode(m_pc++);
			switch (opcode) {
			case OpcodeType::kStop:
				return;
			case OpcodeType::kNop:
				break;
			case OpcodeType::kPushK: {
				auto constIdx = m_curFunc->instrSect.GetU32(m_pc);
				m_pc += 4;
				m_stackSect.push_back(m_constSect[constIdx]->Copy());
				break;
			}
			case OpcodeType::kPushV: {
				auto varIdx = m_curFunc->instrSect.GetU32(m_pc);
				m_pc += 4;
				m_stackSect.push_back(GetVarCopy(varIdx));
				break;
			}
			case OpcodeType::kPopV: {
				auto& a = m_stackSect[m_stackSect.size() - 1];
				auto varIdx = m_curFunc->instrSect.GetU32(m_pc);
				m_pc += 4;
				SetVar(varIdx, std::move(a));
				m_stackSect.pop_back();
				break;
			}
			case OpcodeType::kAdd: {
				auto& a = m_stackSect[m_stackSect.size() - 1];
				auto& b = m_stackSect[m_stackSect.size() - 2];
				b->GetNumber()->value += a->GetNumber()->value;
				m_stackSect.pop_back();
				break;
			}
			case OpcodeType::kSub: {
				auto& a = m_stackSect[m_stackSect.size() - 1];
				auto& b = m_stackSect[m_stackSect.size() - 2];
				b->GetNumber()->value -= a->GetNumber()->value;
				m_stackSect.pop_back();
				break;
			}
			case OpcodeType::kMul: {
				auto& a = m_stackSect[m_stackSect.size() - 1];
				auto& b = m_stackSect[m_stackSect.size() - 2];
				b->GetNumber()->value *= a->GetNumber()->value;
				m_stackSect.pop_back();
				break;
			}
			case OpcodeType::kDiv: {
				auto& a = m_stackSect[m_stackSect.size() - 1];
				auto& b = m_stackSect[m_stackSect.size() - 2];
				b->GetNumber()->value /= a->GetNumber()->value;
				m_stackSect.pop_back();
				break;
			}
			case OpcodeType::kCall: {
				// 先将栈中的参数移动到变量表中

				auto varIdx = m_curFunc->instrSect.GetU32(m_pc);
				m_pc += 4;
				auto value = GetVar(varIdx)->GetFunctionProto()->value;

				if (value->GetType() == ValueType::kFunctionBody) {
					auto callFunc = value->GetFunctionBody();
					printf("%s", callFunc->Disassembly().c_str());
					auto saveVarIdxBase = m_varIdxBase;
					m_varIdxBase = m_varSect.size();
					for (int i = callFunc->parCount - 1; i >= 0; i--) {
						SetVar(i, std::move(m_stackSect[m_stackSect.size() - 1]));
						m_stackSect.pop_back();
					}

					// 保存当前环境
					m_stackSect.push_back(std::make_unique<NumberValue>(saveVarIdxBase));
					m_stackSect.push_back(std::make_unique<NumberValue>((uint64_t)m_curFunc));
					m_stackSect.push_back(std::make_unique<NumberValue>(m_pc));

					m_curFunc = callFunc;
					m_pc = 0;
					
				}
				else if (value->GetType() == ValueType::kFunctionBridge) {
					auto callFunc = value->GetFunctionBirdge();
					callFunc->funcAddr(1, &m_stackSect);
				}
				break;
			}
			case OpcodeType::kRet: {
				auto& pc = m_stackSect[m_stackSect.size() - 1];
				auto& curFunc = m_stackSect[m_stackSect.size() - 2];
				auto& varIdxBase = m_stackSect[m_stackSect.size() - 3];

				m_pc = pc->GetNumber()->value;
				m_curFunc = (FunctionBodyValue*)curFunc->GetNumber()->value;
				m_varIdxBase = varIdxBase->GetNumber()->value;

				m_stackSect.pop_back();
				m_stackSect.pop_back();
				m_stackSect.pop_back();
				break;
			}
			default:
				throw VMException("Unknown instruction");
			}
		} while (true);
	}

private:
	uint32_t m_pc;
	uint32_t m_varIdxBase;
	FunctionBodyValue m_mainFunc;
	FunctionBodyValue* m_curFunc;
	std::vector<std::unique_ptr<Value>> m_stackSect;
	std::vector<std::unique_ptr<Value>> m_constSect;
	std::vector<std::unique_ptr<Value>> m_varSect;
	
};

} // namespace vm 

#endif // VM_VM_H_