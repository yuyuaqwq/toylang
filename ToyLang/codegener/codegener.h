#ifndef CODEGENER_CODEGENER_H_
#define CODEGENER_CODEGENER_H_

#include <vector>
#include <map>
#include <set>
#include <string>

#include "vm/instr.h"
#include "vm/value.h"
#include "vm/vm.h"

#include "ast/stat.h"
#include "ast/exp.h"

namespace codegener {

// 代码生成时发生的异常
class CodeGenerException : public std::exception{
public:
	CodeGenerException(const char* t_msg) : std::exception(t_msg) {

	}
};


class CodeGener {
public:
	CodeGener(vm::VM* t_vm) : m_vm(t_vm), m_level(0), m_varCount(0), m_varTable(1) {
		m_curFunc = &m_vm->m_mainFunc;
	}


	void EntryScope() {
		m_level++;
		m_varTable.push_back(std::map<std::string, uint32_t>());
	}

	void ExitScope() {
		
		FreeMultVars(m_varTable[m_level].size());		// 回收作用域中使用的变量
		m_level--;
		m_varTable.pop_back();
	}

	uint32_t AllocVar() {
		return m_varCount++;
	}

	void FreeVar() {
		m_varCount--;
	}

	void FreeMultVars(uint32_t count) {
		m_varCount -= count;
	}

	uint32_t GetVar(std::string name) {
		uint32_t varIdx = -1;
		// 就近找变量
		auto i = m_level;
		do {
			auto& varScope = m_varTable[i];
			auto it = varScope.find(name);
			if (it != varScope.end()) {
				varIdx = it->second;
			}
		} while (i--);

		return varIdx;
	}


	void RegistryFunctionBridge(std::string funcName, vm::FunctionBridgeCall funcAddr) {
		auto& varScope = m_varTable[m_level];
		if (varScope.find(funcName) != varScope.end()) {
			throw CodeGenerException("function redefinition");
		}
		m_constTable.push_back(std::make_unique<vm::FunctionBridgeValue>(funcAddr));
		int constIdx = m_constTable.size() - 1;

		auto varIdx = AllocVar();
		varScope.insert(std::make_pair(funcName, varIdx));

		// 生成将函数放到变量表中的代码
		// 交给虚拟机执行时去加载，虚拟机发现加载的常量是函数体，就会将函数原型赋给局部变量
		m_curFunc->instrSect.EmitPushK(constIdx);
		m_curFunc->instrSect.EmitPopV(varIdx);
	}


	void Generate(ast::BlockStat* block) {
		
		for (auto& stat : block->statList) {
			GenerateStat(stat.get());
		}
		m_vm->m_constSect = std::move(m_constTable);
		m_curFunc->instrSect.EmitStop();
	}

	void GenerateBlock(ast::BlockStat* block) {
		EntryScope();
		for (auto& stat : block->statList) {
			GenerateStat(stat.get());
		}
		ExitScope();
	}

	void GenerateStat(ast::Stat* stat) {
		switch (stat->GetType())
		{
		case ast::StatType::kBlock: {
			GenerateBlock(static_cast<ast::BlockStat*>(stat));
			break;
		}
		case ast::StatType::kExp: {
			GenerateExp(static_cast<ast::ExpStat*>(stat)->exp.get());
			break;
		}
		case ast::StatType::kFuncDef: {
			GenerateFuncDefStat(static_cast<ast::FuncDefStat*>(stat));
			break;
		}
		case ast::StatType::kAssign: {
			GenerateAssignStat(static_cast<ast::AssignStat*>(stat));
			break;
		}
		case ast::StatType::kNewVar: {
			GenerateNewVarStat(static_cast<ast::NewVarStat*>(stat));
			break;
		}
		default:
			break;
		}
	}

	void GenerateFuncDefStat(ast::FuncDefStat* stat) {
		auto& varScope = m_varTable[m_level];
		if (varScope.find(stat->funcName) != varScope.end()) {
			throw CodeGenerException("function redefinition");
		}
		m_constTable.push_back(std::make_unique<vm::FunctionBodyValue>(stat->parList.size()));
		int constIdx = m_constTable.size() - 1;
		
		auto varIdx = AllocVar();
		varScope.insert(std::make_pair(stat->funcName, varIdx));

		// 生成将函数放到变量表中的代码
		// 交给虚拟机执行时去加载，虚拟机发现加载的常量是函数体，就会将函数原型赋给局部变量
		m_curFunc->instrSect.EmitPushK(constIdx);
		m_curFunc->instrSect.EmitPopV(varIdx);
		
		
		// 备份当前环境，以新环境去生成函数指令(函数是独立的作用域，不能捕获函数外作用域的符号)
		auto saveLevel = m_level;
		auto saveVarCount = m_varCount;
		auto saveVarTable = std::move(m_varTable);
		auto savefunc = m_curFunc;

		m_level = 0;
		m_varCount = 0;
		m_varTable.resize(1);
		m_curFunc = m_constTable[constIdx]->GetFunctionBody();

		for (int i = 0; i < m_curFunc->parCount; i++) {
			m_varTable[m_level].insert(std::make_pair(stat->parList[i], AllocVar()));
		}

		auto block = stat->block.get();
		for (auto& stat : block->statList) {
			GenerateStat(stat.get());
		}
		m_curFunc->instrSect.EmitRet();

		// 恢复环境
		m_curFunc = savefunc;
		m_varTable = std::move(saveVarTable);
		m_varCount = saveVarCount;
		m_level = saveLevel;
	}


	// 局部变量在此无法得知总共需要多少，因此不能分配到栈上

	// vm中需要有一个字段，存放当前函数的基变量索引，访问变量的指令都要加上这个
	// 每次call时，将当前该字段的值push到栈中，并且将该字段置为当前局部变量表的最顶部
	// ret时就从栈中恢复这个字段

	// 函数定义指令就暂时将m_varCount置0，这样子函数定义中的块被解析时，变量索引又是从0开始的，返回后恢复

	void GenerateNewVarStat(ast::NewVarStat* stat) {
		auto& varScope = m_varTable[m_level];
		if (varScope.find(stat->varName) != varScope.end()) {
			throw CodeGenerException("local var redefinition");
		}
		auto varIdx = AllocVar();
		varScope.insert(std::make_pair(stat->varName, varIdx));
		GenerateExp(stat->exp.get());		// 生成表达式计算指令，最终结果会到栈顶
		m_curFunc->instrSect.EmitPopV(varIdx);	// 弹出到局部变量中
	}

	void GenerateAssignStat(ast::AssignStat* stat) {
		auto varIdx = GetVar(stat->varName);
		if (varIdx == -1) {
			throw CodeGenerException("var not defined");
		}
		GenerateExp(stat->exp.get());		// 表达式压栈
		m_curFunc->instrSect.EmitPopV(varIdx);	// 弹出到局部变量中
	}

	void GenerateExp(ast::Exp* exp) {
		switch (exp->GetType())
		{
		case ast::ExpType::kNull: {
			vm::NullValue null;
			uint32_t idx;
			auto it = m_constMap.find(&null);
			if (it == m_constMap.end()) {
				m_constTable.push_back(std::make_unique<vm::NullValue>());
				idx = m_constTable.size() - 1;
			}
			else {
				idx = it->second;
			}
			m_curFunc->instrSect.EmitPushK(idx);
			break;
		}
		case ast::ExpType::kBool: {
			auto boolexp = static_cast<ast::BoolExp*>(exp);
			uint32_t idx;
			vm::BoolValue boolv(boolexp->value);
			auto it = m_constMap.find(&boolv);
			if (it == m_constMap.end()) {
				m_constTable.push_back(std::make_unique<vm::BoolValue>(boolexp->value));
				idx = m_constTable.size() - 1;
			}
			else {
				idx = it->second;
			}
			m_curFunc->instrSect.EmitPushK(idx);
			break;
		}
		case ast::ExpType::kNumber: {
			auto numexp = static_cast<ast::NumberExp*>(exp);
			uint32_t idx;
			vm::NumberValue numv(numexp->value);
			auto it = m_constMap.find(&numv);
			if (it == m_constMap.end()) {
				m_constTable.push_back(std::make_unique <vm::NumberValue> (numexp->value));
				idx = m_constTable.size() - 1;
			}
			else {
				idx = it->second;
			}
			m_curFunc->instrSect.EmitPushK(idx);
			break;
		}
		case ast::ExpType::kString: {
			auto strexp = static_cast<ast::StringExp*>(exp);
			uint32_t idx;
			vm::StringValue strv(strexp->value);
			auto it = m_constMap.find(&strv);
			if (it == m_constMap.end()) {
				m_constTable.push_back(std::make_unique <vm::StringValue>(strexp->value));
				idx = m_constTable.size() - 1;
			}
			else {
				idx = it->second;
			}
			m_curFunc->instrSect.EmitPushK(idx);
			break;
		}
		case ast::ExpType::kName: {
			// 是取变量值的话，查找到对应的变量编号，将其入栈
			auto nameExp = static_cast<ast::NameExp*>(exp);
			
			auto varIdx = GetVar(nameExp->name);
			if (varIdx == -1) {
				throw CodeGenerException("var not defined");
			}

			m_curFunc->instrSect.EmitPushV(varIdx);
			break;
		}
		case ast::ExpType::kBinaOp: {
			auto binaOpExp = static_cast<ast::BinaOpExp*>(exp);

			// 左右表达式的值入栈
			GenerateExp(binaOpExp->leftExp.get());
			GenerateExp(binaOpExp->rightExp.get());

			// 生成运算指令
			switch (binaOpExp->oper) {
			case lexer::TokenType::kOpAdd:
				m_curFunc->instrSect.EmitAdd();
				break;
			case lexer::TokenType::kOpSub:
				m_curFunc->instrSect.EmitSub();
				break;
			case lexer::TokenType::kOpMul:
				m_curFunc->instrSect.EmitMul();
				break;
			case lexer::TokenType::kOpDiv:
				m_curFunc->instrSect.EmitDiv();
				break;
			case lexer::TokenType::kOpEq:
				m_curFunc->instrSect.EmitEq();
				break;
			case lexer::TokenType::kOpLt:
				m_curFunc->instrSect.EmitLt();
				break;
			case lexer::TokenType::kOpLe:
				m_curFunc->instrSect.EmitLe();
				break;
			case lexer::TokenType::kOpGt:
				m_curFunc->instrSect.EmitGt();
				break;
			case lexer::TokenType::kOpGe:
				m_curFunc->instrSect.EmitGe();
				break;
			default:
				throw CodeGenerException("Unrecognized binary operator");
			}
			break;
		}
		case ast::ExpType::kFuncCall: {
			auto funcCallExp = static_cast<ast::FuncCallExp*>(exp);

			auto funcIdx = GetVar(funcCallExp->name);
			if (funcIdx == -1) {
				throw CodeGenerException("Function not defined");
			}

			//if (funcCallExp->parList.size() < m_constTable[]->GetFunctionBody()->parCount) {
			//	throw CodeGenerException("Wrong number of parameters passed during function call");
			//}

			for (auto& parexp : funcCallExp->parList) {
				// 参数入栈，由call负责将栈中参数放到变量表中
				GenerateExp(parexp.get());
			}

			

			// 函数在变量表的索引
			m_curFunc->instrSect.EmitCall(funcIdx);
			break;
		}
		default:
			break;
		}
	}



private:

	vm::VM* m_vm;

	vm::FunctionBodyValue* m_curFunc;		// 当前生成函数

	// 暂时有问题，指针就没办法找重载了
	std::map<vm::Value*, uint32_t> m_constMap;
	std::vector<std::unique_ptr<vm::Value>> m_constTable;		// 全局常量表

	uint32_t m_level;		// 作用域层级
	uint32_t m_varCount;			// 总分配局部变量计数
	std::vector<std::map<std::string, uint32_t>> m_varTable;		// 变量表，与作用域有关，key为局部变量索引
};

} // namespace codegener

#endif // CODEGENER_CODEGENER_H_