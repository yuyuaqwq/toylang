#ifndef CODEGENER_CODEGENER_H_
#define CODEGENER_CODEGENER_H_

#include <vector>
#include <map>
#include <set>
#include <string>

#include "vm/instr.h"
#include "vm/vm.h"

#include "value/value.h"
#include "value/section.h"

#include "ast/stat.h"
#include "ast/exp.h"

namespace toylang {

struct Scope {
	FunctionBodyValue* m_func;		// 作用域所属函数
	uint32_t varCount;		// 当前函数在当前作用域中的有效变量计数
	std::map<std::string, uint32_t> m_varTable;		// 变量表，key为变量索引
};



// 代码生成时发生的异常
class CodeGenerException : public std::exception{
public:
	CodeGenerException(const char* t_msg);
};


class CodeGener {
public:
	CodeGener(ValueSection* t_constSect);

public:
	void EntryScope(FunctionBodyValue* subFunc = nullptr);
	void ExitScope();
	uint32_t AllocConst(std::unique_ptr<Value> value);
	uint32_t AllocVar(std::string varName);
	uint32_t GetVar(std::string varName);
	void RegistryFunctionBridge(std::string funcName, FunctionBridgeCall funcAddr);

	void Generate(BlockStat* block, ValueSection* constSect);
	void GenerateBlock(BlockStat* block);
	void GenerateStat(Stat* stat);
	void GenerateFuncDefStat(FuncDefStat* stat);
	void GenerateReturnStat(ReturnStat* stat);
	void GenerateNewVarStat(NewVarStat* stat);
	void GenerateAssignStat(AssignStat* stat);
	void GenerateIfStat(IfStat* stat);
	void GenerateWhileStat(WhileStat* stat);
	void GenerateContinueStat(ContinueStat* stat);
	void GenerateBreakStat(BreakStat* stat);
	void GenerateExp(Exp* exp);

private:

	// 函数相关
	FunctionBodyValue* m_curFunc;		// 当前生成函数

	// 常量相关
	std::map<Value*, uint32_t> m_constMap;		// 暂时有问题，指针就没办法找重载<了
	ValueSection* m_constSect;		// 全局常量区

	// 作用域相关
	std::vector<Scope> m_scope;

	// 循环相关
	uint32_t m_curLoopStartPc;
	std::vector<uint32_t>* m_curLoopRepairEndPcList;
};

} // namespace codegener

#endif // CODEGENER_CODEGENER_H_