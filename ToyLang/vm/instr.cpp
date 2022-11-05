#include "instr.h"

namespace vm {

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

} // namespace vm