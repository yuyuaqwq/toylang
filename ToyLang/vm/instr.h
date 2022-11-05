#ifndef VM_INSTR_H_
#define VM_INSTR_H_

#include <map>
#include <vector>
#include <string>

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

struct InstrInfo {
	std::string str;
	std::vector<char> parSizeList;
};

extern std::map<OpcodeType, InstrInfo> g_instrSymbol;

class InstrSection {
public:
	OpcodeType GetOpcode(uint32_t pc) {
		return (OpcodeType)container[pc];
	}

	uint8_t GetU8(uint32_t pc) {
		return *(uint8_t*)&container[pc];
	}

	uint32_t GetU32(uint32_t pc) {
		return *(uint32_t*)&container[pc];
	}

	void EmitOpcode(OpcodeType opcode) {
		container.push_back((uint8_t)opcode);
	}

	void EmitU8(uint8_t val) {
		container.push_back(val);
	}

	void EmitU32(uint32_t val) {
		container.push_back(val);
		container.push_back(val >> 8);
		container.push_back(val >> 16);
		container.push_back(val >> 24);
	}

	void EmitStop() {
		EmitOpcode(OpcodeType::kStop);
	}

	void EmitNop() {
		EmitOpcode(OpcodeType::kNop);
	}

	void EmitPushK(uint32_t sk) {
		EmitOpcode(OpcodeType::kPushK);
		EmitU32(sk);
	}

	void EmitPushV(uint32_t sv) {
		EmitOpcode(OpcodeType::kPushV);
		EmitU32(sv);
	}

	void EmitPopV(uint32_t dv) {
		EmitOpcode(OpcodeType::kPopV);
		EmitU32(dv);
	}

	void EmitAdd() {
		EmitOpcode(OpcodeType::kAdd);
	}

	void EmitSub() {
		EmitOpcode(OpcodeType::kSub);
	}

	void EmitMul() {
		EmitOpcode(OpcodeType::kMul);
	}

	void EmitDiv() {
		EmitOpcode(OpcodeType::kDiv);
	}

	void EmitCall(uint32_t sv) {
		EmitOpcode(OpcodeType::kCall);
		EmitU32(sv);
	}

	void EmitRet() {
		EmitOpcode(OpcodeType::kRet);
	}

	void EmitEq() {
		EmitOpcode(OpcodeType::kEq);
	}

	void EmitGt() {
		EmitOpcode(OpcodeType::kGt);
	}

	void EmitGe() {
		EmitOpcode(OpcodeType::kGe);
	}

	void EmitLt() {
		EmitOpcode(OpcodeType::kLt);
	}

	void EmitLe() {
		EmitOpcode(OpcodeType::kLe);
	}

	void EmitJc(uint32_t i) {
		EmitOpcode(OpcodeType::kJc);
		EmitU32(i);
	}

	void EmitJmp(uint32_t i) {
		EmitOpcode(OpcodeType::kJmp);
		EmitU32(i);
	}

public:
	std::vector<uint8_t> container;
};




} // namespace vm 

#endif // VM_INSTR_H_