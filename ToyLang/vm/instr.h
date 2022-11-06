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
	kPop,
	kPopV,
	kAdd,
	kSub,
	kMul,
	kDiv,
	kCall,
	kRet,
	kNe,
	kEq,
	kGt,
	kGe,
	kLt,
	kLe,
	kJcf,
	kJmp,
};

struct InstrInfo {
	std::string str;
	std::vector<char> parSizeList;
};

extern std::map<OpcodeType, InstrInfo> g_instrSymbol;

class InstrSection {
public:
	uint8_t* GetPtr(uint32_t pc);

	uint32_t GetPc();

	OpcodeType GetOpcode(uint32_t pc);

	uint8_t GetU8(uint32_t pc);

	uint32_t GetU32(uint32_t pc);

	void EmitOpcode(OpcodeType opcode);

	void EmitU8(uint8_t val);

	void EmitU32(uint32_t val);

	void EmitStop();

	void EmitNop();

	void EmitPushK(uint32_t sk);

	void EmitPop();

	void EmitPushV(uint32_t sv);

	void EmitPopV(uint32_t dv);


	void EmitAdd();

	void EmitSub();

	void EmitMul();

	void EmitDiv();

	void EmitCall(uint32_t sv);

	void EmitRet();

	void EmitNe();

	void EmitEq();

	void EmitGt();

	void EmitGe();

	void EmitLt();

	void EmitLe();

	void EmitJcf(uint32_t i);

	void EmitJmp(uint32_t i);

public:
	std::vector<uint8_t> container;
};




} // namespace vm 

#endif // VM_INSTR_H_