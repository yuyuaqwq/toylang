#ifndef VM_INSTR_H_
#define VM_INSTR_H_

#include <vector>

namespace vm {

enum class OpcodeType {
	kStop = 0,
	kNop,
	kPushK,
	kPushV,
	kPopV,
	kAdd,
	kSub,
	kMul,
	kDiv,
	kCall,
	kRet,
	kEq,
	kGt,
	kGe,
	kLt,
	kLe,
	kJc,
	kJmp,
};


class InstrSection {
public:
	void Emit(OpcodeType opcode) {
		m_container.push_back((uint8_t)opcode);
	}

	void EmitU8(uint8_t val) {
		m_container.push_back(val);
	}

	void EmitU32(uint32_t val) {
		m_container.push_back(val);
		m_container.push_back(val >> 8);
		m_container.push_back(val >> 16);
		m_container.push_back(val >> 24);
	}

	void EmitStop() {
		Emit(OpcodeType::kStop);
	}

	void EmitNop() {
		Emit(OpcodeType::kNop);
	}

	void EmitPushV(uint32_t sv) {
		Emit(OpcodeType::kPushV);
		EmitU32(sv);
	}

	void EmitPushK(uint32_t sk) {
		Emit(OpcodeType::kPushK);
		EmitU32(sk);
	}

	void EmitPopV(uint32_t dv) {
		Emit(OpcodeType::kPopV);
		EmitU32(dv);
	}

	void EmitAdd() {
		Emit(OpcodeType::kAdd);
	}

	void EmitSub() {
		Emit(OpcodeType::kSub);
	}

	void EmitMul() {
		Emit(OpcodeType::kMul);
	}

	void EmitDiv() {
		Emit(OpcodeType::kDiv);
	}

	void EmitCall(uint32_t sv) {
		Emit(OpcodeType::kCall);
		EmitU32(sv);
	}

	void EmitRet() {
		Emit(OpcodeType::kRet);
	}

	void EmitEq() {
		Emit(OpcodeType::kEq);
	}

	void EmitGt() {
		Emit(OpcodeType::kGt);
	}

	void EmitGe() {
		Emit(OpcodeType::kGe);
	}

	void EmitLt() {
		Emit(OpcodeType::kLt);
	}

	void EmitLe() {
		Emit(OpcodeType::kLe);
	}

	void EmitJc(uint32_t i) {
		Emit(OpcodeType::kJc);
		EmitU32(i);
	}

	void EmitJmp(uint32_t i) {
		Emit(OpcodeType::kJmp);
		EmitU32(i);
	}

private:
	std::vector<uint8_t> m_container;
};


} // namespace vm 

#endif // VM_INSTR_H_