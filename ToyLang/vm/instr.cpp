#include "instr.h"

namespace toylang {

std::map<OpcodeType, InstrInfo> g_instrSymbol {
	{OpcodeType::kStop, {"stop", {}}},
	{OpcodeType::kNop, {"nop", {}}},
	{OpcodeType::kPushK, {"pushk", {4}}},
	{OpcodeType::kPushV, {"pushv", {4}}},
	{OpcodeType::kPop, {"pop", {}}},
	{OpcodeType::kPopV, {"popv", {4}}},
	{OpcodeType::kAdd, {"add", {}}},
	{OpcodeType::kSub, {"sub", {}}},
	{OpcodeType::kMul, {"mul", {}}},
	{OpcodeType::kDiv, {"div", {}}},
	{OpcodeType::kCall, {"call", {4}}},
	{OpcodeType::kRet, {"ret", {}}},
	{OpcodeType::kNe, {"ne", {}}},
	{OpcodeType::kEq, {"eq", {}}},
	{OpcodeType::kGt, {"gt", {}}},
	{OpcodeType::kGe, {"ge", {}}},
	{OpcodeType::kLt, {"lt", {}}},
	{OpcodeType::kLe, {"le", {}}},
	{OpcodeType::kJcf, {"jcf", {4}}},
	{OpcodeType::kJmp, {"jmp", {4}}},

};



uint8_t* InstrSection::GetPtr(uint32_t pc) {
	return container.data() + pc;
}

uint32_t InstrSection::GetPc() const noexcept {
	return container.size();
}

OpcodeType InstrSection::GetOpcode(uint32_t pc) {
	return (OpcodeType)container[pc];
}

uint8_t InstrSection::GetU8(uint32_t pc) {
	return *(uint8_t*)&container[pc];
}

uint32_t InstrSection::GetU32(uint32_t pc) {
	return *(uint32_t*)&container[pc];
}

void InstrSection::EmitOpcode(OpcodeType opcode) {
	container.push_back((uint8_t)opcode);
}

void InstrSection::EmitU8(uint8_t val) {
	container.push_back(val);
}

void InstrSection::EmitU32(uint32_t val) {
	container.push_back(val);
	container.push_back(val >> 8);
	container.push_back(val >> 16);
	container.push_back(val >> 24);
}

void InstrSection::EmitStop() {
	EmitOpcode(OpcodeType::kStop);
}

void InstrSection::EmitNop() {
	EmitOpcode(OpcodeType::kNop);
}

void InstrSection::EmitPushK(uint32_t sk) {
	EmitOpcode(OpcodeType::kPushK);
	EmitU32(sk);
}

void InstrSection::EmitPop() {
	EmitOpcode(OpcodeType::kPop);
}

void InstrSection::EmitPushV(uint32_t sv) {
	EmitOpcode(OpcodeType::kPushV);
	EmitU32(sv);
}

void InstrSection::EmitPopV(uint32_t dv) {
	EmitOpcode(OpcodeType::kPopV);
	EmitU32(dv);
}


void InstrSection::EmitAdd() {
	EmitOpcode(OpcodeType::kAdd);
}

void InstrSection::EmitSub() {
	EmitOpcode(OpcodeType::kSub);
}

void InstrSection::EmitMul() {
	EmitOpcode(OpcodeType::kMul);
}

void InstrSection::EmitDiv() {
	EmitOpcode(OpcodeType::kDiv);
}

void InstrSection::EmitCall(uint32_t sv) {
	EmitOpcode(OpcodeType::kCall);
	EmitU32(sv);
}

void InstrSection::EmitRet() {
	EmitOpcode(OpcodeType::kRet);
}

void InstrSection::EmitNe() {
	EmitOpcode(OpcodeType::kNe);
}

void InstrSection::EmitEq() {
	EmitOpcode(OpcodeType::kEq);
}

void InstrSection::EmitGt() {
	EmitOpcode(OpcodeType::kGt);
}

void InstrSection::EmitGe() {
	EmitOpcode(OpcodeType::kGe);
}

void InstrSection::EmitLt() {
	EmitOpcode(OpcodeType::kLt);
}

void InstrSection::EmitLe() {
	EmitOpcode(OpcodeType::kLe);
}

void InstrSection::EmitJcf(uint32_t i) {
	EmitOpcode(OpcodeType::kJcf);
	EmitU32(i);
}

void InstrSection::EmitJmp(uint32_t i) {
	EmitOpcode(OpcodeType::kJmp);
	EmitU32(i);
}



} // namespace vm