#ifndef VM_INSTR_H_
#define VM_INSTR_H_

#include <vector>

namespace vm {

enum class OpcodeType {
	kStop = 0,
	kNop,
	kMovK,
	kMovR,
	kPushK,
	kPushR,
	kPopR,
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

	void EmitMovK(uint8_t dr, uint32_t sk) {
		Emit(OpcodeType::kMovK);
		EmitU8(dr);
		EmitU32(sk);
	}

	void EmitMovR(uint8_t dr, uint8_t sr) {
		Emit(OpcodeType::kMovR);
		EmitU8(dr);
		EmitU8(sr);
	}

	void EmitPushR(uint8_t sr) {
		Emit(OpcodeType::kPushR);
		EmitU8(sr);
	}

	void EmitPushK(uint32_t sk) {
		Emit(OpcodeType::kPushK);
		EmitU32(sk);
	}

	void EmitPopR(uint8_t dr) {
		Emit(OpcodeType::kPopR);
		EmitU8(dr);
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

	void EmitCall() {
		Emit(OpcodeType::kCall);
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

	void EmitJmp(uint32_t i) {
		Emit(OpcodeType::kJmp);
		EmitU32(i);
	}


private:
	std::vector<uint8_t> m_container;
};


} // namespace vm 

#endif // VM_INSTR_H_