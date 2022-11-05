#ifndef CODEGENER_CODEGENER_H_
#define CODEGENER_CODEGENER_H_

#include <vector>
#include <map>
#include <set>
#include <string>

#include "vm/instr.h"
#include "vm/value.h"
#include "vm/vm.h"
#include "vm/section.h"

#include "ast/stat.h"
#include "ast/exp.h"

namespace codegener {

struct Scope {
	vm::FunctionBodyValue* m_func;		// 作用域所属函数
	uint32_t varCount;		// 当前函数在当前作用域中的有效变量计数
	std::map<std::string, uint32_t> m_varTable;		// 变量表，key为变量索引
};



// 代码生成时发生的异常
class CodeGenerException : public std::exception{
public:
	CodeGenerException(const char* t_msg) : std::exception(t_msg) {

	}
};


class CodeGener {
public:
	CodeGener(vm::VM* t_vm) : m_vm{ t_vm } {
		m_curFunc = &m_vm->m_mainFunc;
		m_scope.push_back(Scope{ m_curFunc });
	}

	void EntryScope(vm::FunctionBodyValue* subFunc = nullptr) {
		if (!subFunc) {
			// 进入的作用域不是新函数
			m_scope.push_back(Scope{ m_curFunc, m_scope[m_scope.size() - 1].varCount });
			return;
		}
		// 进入的作用域是新函数
		m_scope.push_back(Scope{ subFunc, 0 });
	}

	void ExitScope() {
		m_scope.pop_back();
	}

	uint32_t AllocConst(std::unique_ptr<vm::Value> value) {
		uint32_t constIdx;
		auto it = m_constMap.find(value.get());
		if (it == m_constMap.end()) {
			m_constSect.Push(std::move(value));
			constIdx = m_constSect.Size() - 1;
		}
		else {
			constIdx = it->second;
		}
		return constIdx;
	}

	uint32_t AllocVar(std::string varName) {
		auto& varTable = m_scope[m_scope.size() - 1].m_varTable;
		if (varTable.find(varName) != varTable.end()) {
			throw CodeGenerException("local var redefinition");
		}
		auto varIdx = m_scope[m_scope.size() - 1].varCount++;
		varTable.insert(std::make_pair(varName, varIdx));
		return varIdx;
	}

	uint32_t GetVar(std::string varName) {
		uint32_t varIdx = -1;
		// 就近找变量
		
		for (int i = m_scope.size() - 1; i >= 0; i--) {
			auto& varTable = m_scope[i].m_varTable;
			auto it = varTable.find(varName);
			if (it != varTable.end()) {
				if (m_scope[i].m_func == m_curFunc) {
					varIdx = it->second;
				}
				else {
					// 引用外部函数的变量，需要捕获，为当前函数加载upvalue变量
					auto constIdx = AllocConst(std::make_unique<vm::UpValue>(it->second, m_scope[i].m_func));
					m_curFunc->instrSect.EmitPushK(constIdx);
					varIdx = AllocVar(varName);
					m_curFunc->instrSect.EmitPopV(varIdx);
				}
				break;
			}
		}
		return varIdx;
	}


	void RegistryFunctionBridge(std::string funcName, vm::FunctionBridgeCall funcAddr) {
		auto varIdx = AllocVar(funcName);
		auto constIdx = AllocConst(std::make_unique<vm::FunctionBridgeValue>(funcAddr));


		// 生成将函数放到变量表中的代码
		// 交给虚拟机执行时去加载，虚拟机发现加载的常量是函数体，就会将函数原型赋给局部变量
		m_curFunc->instrSect.EmitPushK(constIdx);
		m_curFunc->instrSect.EmitPopV(varIdx);
	}


	void Generate(ast::BlockStat* block) {
		
		for (auto& stat : block->statList) {
			GenerateStat(stat.get());
		}
		m_vm->m_constSect = std::move(m_constSect);
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
			auto expStat = static_cast<ast::ExpStat*>(stat)->exp.get();

			// 抛弃纯表达式语句的最终结果
			 if (expStat) {
				 GenerateExp(expStat);
				 m_curFunc->instrSect.EmitPop();		
			 }
			
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
		case ast::StatType::kIf:
			GenerateIfStat(static_cast<ast::IfStat*>(stat));
			break;
		case ast::StatType::kReturn:
			GenerateReturnStat(static_cast<ast::ReturnStat*>(stat));
			break;
		default:
			throw CodeGenerException("Unknown statement type");
		}
	}

	void GenerateFuncDefStat(ast::FuncDefStat* stat) {
		auto varIdx = AllocVar(stat->funcName);
		auto constIdx = AllocConst(std::make_unique<vm::FunctionBodyValue>(stat->parList.size()));

		// 生成将函数放到变量表中的代码
		// 交给虚拟机执行时去加载，虚拟机发现加载的常量是函数体，就会将函数原型赋给局部变量
		m_curFunc->instrSect.EmitPushK(constIdx);
		m_curFunc->instrSect.EmitPopV(varIdx);
		
		
		// 保存环境，以生成新指令流
		auto savefunc = m_curFunc;

		// 切换环境
		EntryScope(m_constSect.Get(constIdx)->GetFunctionBody());
		m_curFunc = m_constSect.Get(constIdx)->GetFunctionBody();

		
		for (int i = 0; i < m_curFunc->parCount; i++) {
			AllocVar(stat->parList[i]);
		}

		auto block = stat->block.get();
		for (int i = 0; i < block->statList.size(); i++) {
			auto& stat = block->statList[i];
			GenerateStat(stat.get());
			if (i == block->statList.size() - 1) {
				if (stat->GetType() != ast::StatType::kReturn) {
					// 补全末尾的return
					m_curFunc->instrSect.EmitPushK(AllocConst(std::make_unique<vm::NullValue>()));
					m_curFunc->instrSect.EmitRet();
				}
			}
		}

		// 恢复环境
		ExitScope();
		m_curFunc = savefunc;
	}

	void GenerateReturnStat(ast::ReturnStat* stat) {
		if (stat->exp.get()) {
			GenerateExp(stat->exp.get());
		}
		else {
			m_curFunc->instrSect.EmitPushK(AllocConst(std::make_unique<vm::NullValue>()));
		}
		m_curFunc->instrSect.EmitRet();
	}

	// 局部变量在此无法提前得知总共需要多少，因此不能分配到栈上
	// 解决方案：
		// 另外提供变量表容器

	// 函数内指令流的变量索引在生成时，无法确定变量分配的索引
	// 原因：
		// 定义时无法提前得知call的位置，且call可能有多处，每次call时，变量表的状态可能都不一样
	// 解决方案：
		// 为每个函数提供一个自己的变量表，不放到虚拟机中，call时切换变量表
	    // 在代码生成过程中，需要获取变量时，如果发现使用的变量是当前函数之外的外部作用域的，就会在常量区中创建一个类型为upvalue的变量，并加载到当前函数的变量中
		// upvalue存储了外部函数的Body地址，以及对应的变量索引


	void GenerateNewVarStat(ast::NewVarStat* stat) {
		auto varIdx = AllocVar(stat->varName);
		GenerateExp(stat->exp.get());		// 生成表达式计算指令，最终结果会到栈顶
		m_curFunc->instrSect.EmitPopV(varIdx);	// 弹出到局部变量中
	}

	void GenerateAssignStat(ast::AssignStat* stat) {
		auto varIdx = GetVar(stat->varName);
		if (varIdx == -1) {
			throw CodeGenerException("var not defined");
		}
		GenerateExp(stat->exp.get());		// 表达式压栈
		m_curFunc->instrSect.EmitPopV(varIdx);	// 弹出到变量中
	}

	void GenerateIfStat(ast::IfStat* stat) {
		GenerateExp(stat->exp.get());		// 表达式结果压栈

		
		uint32_t ifpc = m_curFunc->instrSect.GetPc() + 1;		// 留给下一个elif/else修复
		m_curFunc->instrSect.EmitJcf(0);		// 提前写入条件为false时跳转的指令

		GenerateBlock(stat->block.get());

		
		
		// if
		
		// jcf end
		// ...
	// end:
		// ...


		// if
		// else

		// jcf else
		// ...
		// jmp end
	// else:
		// ...
	// end:
		// ....


		// if 
		// elif
		// else

		// jcf elif
		// ...
		// jmp end
	// elif:
		// jcf else
		// ...
		// jmp end
	// else:
		// ...
	// end:
		// ....


		// if 
		// elif
		// elif
		// else

		// jcf elif1
		// ...
		// jmp end
	// elif1:
		// jcf elif2
		// ...
		// jmp end
	// elif2:
		// jcf else
		// ...
		// jmp end
	// else:
		// ...
	// end:
		// ....

		std::vector<uint32_t> endPcList;
		for (auto& elifStat : stat->elifStatList) {
			endPcList.push_back(m_curFunc->instrSect.GetPc() + 1);
			m_curFunc->instrSect.EmitJmp(0);		// 提前写入上一分支退出if分支结构的jmp跳转

			// 修复条件为false时，跳转到if/elif块之后的地址
			*(uint32_t*)m_curFunc->instrSect.GetPtr(ifpc) = m_curFunc->instrSect.GetPc();

			GenerateExp(elifStat->exp.get());		// 表达式结果压栈
			ifpc = m_curFunc->instrSect.GetPc() + 1;		// 留给下一个elif/else修复
			m_curFunc->instrSect.EmitJcf(0);		// 提前写入条件为false时跳转的指令

			GenerateBlock(elifStat->block.get());
		}

		

		if (stat->elseStat.get()) {
			endPcList.push_back(m_curFunc->instrSect.GetPc() + 1);
			m_curFunc->instrSect.EmitJmp(0);		// 提前写入上一分支退出if分支结构的jmp跳转

			// 修复条件为false时，跳转到if/elif块之后的地址
			*(uint32_t*)m_curFunc->instrSect.GetPtr(ifpc) = m_curFunc->instrSect.GetPc();

			GenerateBlock(stat->elseStat->block.get());
		}
		else {
			// 修复条件为false时，跳转到if/elif块之后的地址
			*(uint32_t*)m_curFunc->instrSect.GetPtr(ifpc) = m_curFunc->instrSect.GetPc();
		}

		// 至此if分支结构结束，修复所有退出分支结构的地址
		for (auto endPc : endPcList) {
			*(uint32_t*)m_curFunc->instrSect.GetPtr(endPc) = m_curFunc->instrSect.GetPc();
		}
		


	}


	void GenerateExp(ast::Exp* exp) {
		switch (exp->GetType())
		{
		case ast::ExpType::kNull: {
			auto constIdx = AllocConst(std::make_unique<vm::NullValue>());
			m_curFunc->instrSect.EmitPushK(constIdx);
			break;
		}
		case ast::ExpType::kBool: {
			auto boolexp = static_cast<ast::BoolExp*>(exp);
			auto constIdx = AllocConst(std::make_unique<vm::BoolValue>(boolexp->value));
			m_curFunc->instrSect.EmitPushK(constIdx);
			break;
		}
		case ast::ExpType::kNumber: {
			auto numexp = static_cast<ast::NumberExp*>(exp);
			auto constIdx = AllocConst(std::make_unique<vm::NumberValue>(numexp->value));
			m_curFunc->instrSect.EmitPushK(constIdx);
			break;
		}
		case ast::ExpType::kString: {
			auto strexp = static_cast<ast::StringExp*>(exp);
			auto constIdx = AllocConst(std::make_unique<vm::StringValue>(strexp->value));
			m_curFunc->instrSect.EmitPushK(constIdx);
			break;
		}
		case ast::ExpType::kName: {
			// 是取变量值的话，查找到对应的变量编号，将其入栈
			auto nameExp = static_cast<ast::NameExp*>(exp);
			
			auto varIdx = GetVar(nameExp->name);
			if (varIdx == -1) {
				throw CodeGenerException("var not defined");
			}

			m_curFunc->instrSect.EmitPushV(varIdx);	// 从变量中获取

			
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
			case lexer::TokenType::kOpNe:
				m_curFunc->instrSect.EmitNe();
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

			auto varIdx = GetVar(funcCallExp->name);
			if (varIdx == -1) {
				throw CodeGenerException("Function not defined");
			}

			//if (funcCallExp->parList.size() < m_constTable[]->GetFunctionBody()->parCount) {
			//	throw CodeGenerException("Wrong number of parameters passed during function call");
			//}

			for (int i = funcCallExp->parList.size() - 1; i >= 0; i--) {
				// 参数逆序入栈，由call负责将栈中参数放到变量表中
				GenerateExp(funcCallExp->parList[i].get());
			}

			m_curFunc->instrSect.EmitPushK(AllocConst(std::make_unique<vm::NumberValue>(funcCallExp->parList.size())));

			// 函数原型存放在变量表中
			m_curFunc->instrSect.EmitCall(varIdx);
			break;
		}
		default:
			break;
		}
	}



private:

	vm::VM* m_vm;

	vm::FunctionBodyValue* m_curFunc;		// 当前生成函数

	// 暂时有问题，指针就没办法找重载<了
	std::map<vm::Value*, uint32_t> m_constMap;
	vm::ValueSection m_constSect;		// 全局常量区

	std::vector<Scope> m_scope;
};

} // namespace codegener

#endif // CODEGENER_CODEGENER_H_