#ifndef CODEGENER_CODEGENER_H_
#define CODEGENER_CODEGENER_H_

#include <vector>
#include <map>
#include <set>
#include <string>

#include "vm/instr.h"
#include "vm/value.h"

#include "ast/block.h"
#include "ast/stat.h"
#include "ast/exp.h"

namespace codegener {

class CodeGenerException : public std::exception{
	// 代码生成时发生的异常
public:
	CodeGenerException(const char* t_msg) : std::exception(t_msg) {

	}
};


class CodeGener {
public:
	CodeGener(vm::InstrSection* t_instrSection): m_instrSection(t_instrSection), m_level(0), m_varTable(1) {

	}


	void GenerateBlock(ast::Block* block) {
		for (auto& stat : block->statList) {
			GenerateStat(stat.get());
		}
	}

	void GenerateStat(ast::Stat* stat) {
		switch (stat->GetType())
		{
		case ast::StatType::kFuncDef:
			GenerateFuncDefStat(static_cast<ast::FuncDefStat*>(stat));
			break;
		default:
			break;
		}
	}

	void GenerateFuncDefStat(ast::FuncDefStat* stat) {
		auto& func = m_funcTable;
		if (func.find(stat->funcName) != func.end()) {
			throw CodeGenerException("function redefinition");
		}

		func.insert(std::make_pair(stat->funcName, stat));

	}

	void GenerateAssignStat(ast::AssignStat* stat) {

	}

	void GenerateExp(ast::Exp* exp) {
		switch (exp->GetType())
		{
		case ast::ExpType::kNull: {
			vm::NullValue null;
			uint32_t idx;
			auto it = m_constMap.find(null);
			if (it == m_constMap.end()) {
				m_constTable.push_back(std::make_unique<vm::NullValue>());
				idx = m_constTable.size() - 1;
			}
			else {
				idx = it->second;
			}
			m_instrSection->EmitPushK(idx);
			break;
		}
		case ast::ExpType::kBool: {
			auto boolexp = static_cast<ast::BoolExp*>(exp);
			uint32_t idx;
			vm::BoolValue boolv(boolexp->value);
			auto it = m_constMap.find(boolv);
			if (it == m_constMap.end()) {
				m_constTable.push_back(std::make_unique<vm::BoolValue>(boolexp->value));
				idx = m_constTable.size() - 1;
			}
			else {
				idx = it->second;
			}
			m_instrSection->EmitPushK(idx);
			break;
		}
		case ast::ExpType::kNumber: {
			auto numexp = static_cast<ast::NumberExp*>(exp);
			uint32_t idx;
			vm::NumberValue numv(numexp->value);
			auto it = m_constMap.find(numv);
			if (it == m_constMap.end()) {
				m_constTable.push_back(std::make_unique <vm::NumberValue> (numexp->value));
				idx = m_constTable.size() - 1;
			}
			else {
				idx = it->second;
			}
			m_instrSection->EmitPushK(idx);
			break;
		}
		case ast::ExpType::kString: {
			auto strexp = static_cast<ast::StringExp*>(exp);
			uint32_t idx;
			vm::StringValue strv(strexp->value);
			auto it = m_constMap.find(strv);
			if (it == m_constMap.end()) {
				m_constTable.push_back(std::make_unique <vm::StringValue>(strexp->value));
				idx = m_constTable.size() - 1;
			}
			else {
				idx = it->second;
			}
			m_instrSection->EmitPushK(idx);
			break;
		}
		case ast::ExpType::kName: {
			// 是取变量值的话，查找到变量对应的寄存器编号，将其入栈
			// m_instrSection->EmitPushR();
		}
		case ast::ExpType::kBinaOp: {
			// 处理二元运算表达式节点时，其左右节点的值已经生成了入栈指令，直接生成运算指令即可
			auto binaOpExp = static_cast<ast::BinaOpExp*>(exp);
			switch (binaOpExp->oper) {
			case lexer::TokenType::kOpAdd:
				m_instrSection->EmitAdd();
				break;
			case lexer::TokenType::kOpSub:
				m_instrSection->EmitSub();
				break;
			case lexer::TokenType::kOpMul:
				m_instrSection->EmitMul();
				break;
			case lexer::TokenType::kOpDiv:
				m_instrSection->EmitDiv();
				break;
			case lexer::TokenType::kOpEq:
				m_instrSection->EmitEq();
				break;
			case lexer::TokenType::kOpLt:
				m_instrSection->EmitLt();
				break;
			case lexer::TokenType::kOpLe:
				m_instrSection->EmitLe();
				break;
			case lexer::TokenType::kOpGt:
				m_instrSection->EmitGt();
				break;
			case lexer::TokenType::kOpGe:
				m_instrSection->EmitGe();
				break;
			default:
				throw CodeGenerException("Unrecognized binary operator");
			}
			break;
		}
		case ast::ExpType::kFuncCall: {
			auto funcCallExp = static_cast<ast::FuncCallExp*>(exp);
			auto it = m_funcTable.find(funcCallExp->name);
			if (it == m_funcTable.end()) {
				throw CodeGenerException("Function not defined");
			}

			for (auto& parexp : funcCallExp->parList) {
				// 生成将所有参数入栈的指令
				GenerateExp(parexp.get());
			}
			
			// 生成函数地址入栈指令，或者修改call为带参指令

			m_instrSection->EmitCall();
		}

		default:
			break;
		}
	}



private:
	vm::InstrSection* m_instrSection;
	uint32_t m_level;
	std::map<std::string, ast::FuncDefStat*> m_funcTable;

	std::vector<std::map<std::string, ast::NameExp*>> m_varTable;
	std::map<vm::Value&, int> m_constMap;
	std::vector<std::unique_ptr<vm::Value>> m_constTable;
};

} // namespace codegener

#endif // CODEGENER_CODEGENER_H_