#include "vm.h"


namespace vm {



VMException::VMException(const char* t_msg) : std::exception(t_msg) {

}



VM::VM(value::ValueSection* t_constSect) : m_constSect{ t_constSect }, m_pc{ 0 } {
	m_curFunc = m_constSect->Get(0)->GetFunctionBody();
}

std::string VM::Disassembly() {
	return m_curFunc->Disassembly();
}

value::Value* VM::GetVar(uint32_t idx) {
	if (m_curFunc->varSect.Get(idx)->GetType() == value::ValueType::kUp) {
		// upvalue可能形成链表(代码生成阶段，根据从外层作用域名字找到了变量，但是该变量实际上也是upvalue)，因此要重复向上找直到不是upvalue
		// 有时间可以从代码生成那边优化，也是做循环向上找，直到不再指向upvalue
		auto func = m_curFunc;
		auto upvalue = func->varSect.Get(idx)->GetUp();
		while (upvalue->funcProto->varSect.Get(upvalue->index)->GetType() == value::ValueType::kUp) {
			func = upvalue->funcProto;
			upvalue = func->varSect.Get(upvalue->index)->GetUp();
		}
		return upvalue->funcProto->varSect.Get(upvalue->index).get();
	}
	return m_curFunc->varSect.Get(idx).get();
}

std::unique_ptr<value::Value> VM::GetVarCopy(uint32_t idx) {
	return GetVar(idx)->Copy();
}

void VM::SetVar(uint32_t idx, std::unique_ptr<value::Value> var) {
	if (idx >= m_curFunc->varSect.Size()) {
		m_curFunc->varSect.ReSize(idx + 1);
	}

	else if (m_curFunc->varSect.Get(idx).get() && m_curFunc->varSect.Get(idx)->GetType() == value::ValueType::kUp) {
		auto func = m_curFunc;
		auto upvalue = func->varSect.Get(idx)->GetUp();
		while (upvalue->funcProto->varSect.Get(upvalue->index)->GetType() == value::ValueType::kUp) {
			func = upvalue->funcProto;
			upvalue = func->varSect.Get(upvalue->index)->GetUp();
		}
		upvalue->funcProto->varSect.Set(upvalue->index, std::move(var));
		return;
	}

	m_curFunc->varSect.Set(idx, std::move(var));
}

void VM::SetVar(uint32_t idx, value::Value* var) {
	SetVar(idx, var->Copy());
}


void VM::Run() {
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
			m_stackSect.Push(m_constSect->Get(constIdx)->Copy());
			break;
		}
		case OpcodeType::kPushV: {
			auto varIdx = m_curFunc->instrSect.GetU32(m_pc);
			m_pc += 4;
			m_stackSect.Push(GetVarCopy(varIdx));
			break;
		}
		case OpcodeType::kPop: {
			m_stackSect.Pop();
			break;
		}
		case OpcodeType::kPopV: {
			auto varIdx = m_curFunc->instrSect.GetU32(m_pc);
			m_pc += 4;
			SetVar(varIdx, m_stackSect.Pop());
			break;
		}
		case OpcodeType::kAdd: {
			auto a = m_stackSect.Pop();
			auto& b = m_stackSect.Get(-1);
			b->GetNumber()->val += a->GetNumber()->val;
			break;
		}
		case OpcodeType::kSub: {
			auto a = m_stackSect.Pop();
			auto& b = m_stackSect.Get(-1);
			b->GetNumber()->val -= a->GetNumber()->val;
			break;
		}
		case OpcodeType::kMul: {
			auto a = m_stackSect.Pop();
			auto& b = m_stackSect.Get(-1);
			b->GetNumber()->val *= a->GetNumber()->val;
			break;
		}
		case OpcodeType::kDiv: {
			auto a = m_stackSect.Pop();
			auto& b = m_stackSect.Get(-1);
			b->GetNumber()->val /= a->GetNumber()->val;
			break;
		}
		case OpcodeType::kCall: {
			auto varIdx = m_curFunc->instrSect.GetU32(m_pc);
			m_pc += 4;

			auto value = GetVar(varIdx)->GetFunctionProto()->val;

			auto parCount = m_stackSect.Pop()->GetBool()->val;

			if (value->GetType() == value::ValueType::kFunctionBody) {

				auto callFunc = value->GetFunctionBody();

				//printf("%s\n", callFunc->Disassembly().c_str());

				if (parCount < callFunc->parCount) {
					throw VMException("Wrong number of parameters passed when calling the function");
				}

				auto saveFunc = m_curFunc;
				auto savePc = m_pc;

				// 切换环境
				m_curFunc = callFunc;
				m_pc = 0;

				// 移动栈上的参数到新函数的局部变量表
				//m_varIdxBase = m_varSect.size();
				for (int i = m_curFunc->parCount - 1; i >= 0; i--) {
					SetVar(i, std::move(m_stackSect.Pop()));
				}

				// 保存当前环境
				m_stackSect.Push(std::make_unique<value::NumberValue>((uint64_t)saveFunc));
				m_stackSect.Push(std::make_unique<value::NumberValue>(savePc));

			}
			else if (value->GetType() == value::ValueType::kFunctionBridge) {
				auto callFunc = value->GetFunctionBirdge();
				m_stackSect.Push(callFunc->funcAddr(parCount, &m_stackSect));
			}
			else {
				throw VMException("Wrong call type");
			}
			break;
		}
		case OpcodeType::kRet: {
			auto retValue = m_stackSect.Pop();
			auto pc = m_stackSect.Pop();
			auto& curFunc = m_stackSect.Get(-1);

			// 恢复环境
			m_curFunc = (value::FunctionBodyValue*)curFunc->GetNumber()->val;
			m_pc = pc->GetNumber()->val;

			curFunc = std::move(retValue);
			break;
		}
		case OpcodeType::kNe: {
			auto a = m_stackSect.Pop();
			auto& b = m_stackSect.Get(-1);
			bool res = !(*b == *a);
			b = std::move(std::make_unique<value::BoolValue>(res));
			break;
		}
		case OpcodeType::kEq: {
			auto a = m_stackSect.Pop();
			auto& b = m_stackSect.Get(-1);
			bool res = *b == *a;
			b = std::move(std::make_unique<value::BoolValue>(res));
			break;
		}
		case OpcodeType::kLt: {
			auto a = m_stackSect.Pop();
			auto& b = m_stackSect.Get(-1);
			bool res = *b < *a;
			b = std::move(std::make_unique<value::BoolValue>(res));
			break;
		}
		case OpcodeType::kLe: {
			auto a = m_stackSect.Pop();
			auto& b = m_stackSect.Get(-1);
			bool res = *b <= *a;
			b = std::move(std::make_unique<value::BoolValue>(res));
			break;
		}
		case OpcodeType::kGt: {
			auto a = m_stackSect.Pop();
			auto& b = m_stackSect.Get(-1);
			bool res = !(*b <= *a);
			b = std::move(std::make_unique<value::BoolValue>(res));
			break;
		}
		case OpcodeType::kGe: {
			auto a = m_stackSect.Pop();
			auto& b = m_stackSect.Get(-1);
			bool res = !(*b < *a);
			b = std::move(std::make_unique<value::BoolValue>(res));
			break;
		}
		case OpcodeType::kJcf: {
			auto newPc = m_curFunc->instrSect.GetU32(m_pc);
			m_pc += 4;
			auto boolValue = m_stackSect.Pop()->GetBool()->val;
			if (boolValue == false) {
				m_pc = newPc;
			}
			break;
		}
		case OpcodeType::kJmp: {
			auto newPc = m_curFunc->instrSect.GetU32(m_pc);
			m_pc = newPc;
			break;
		}
		default:
			throw VMException("Unknown instruction");
		}
	} while (true);
}



} // namespace vm