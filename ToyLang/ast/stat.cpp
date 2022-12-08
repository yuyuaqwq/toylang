#include "stat.h"

namespace toylang {

StatType ExpStat::GetType() const noexcept {
	return StatType::kExp;
}

StatType BlockStat::GetType() const noexcept {
	return StatType::kBlock;
}

BlockStat::BlockStat(std::vector<std::unique_ptr<Stat>>&& t_statList) : statList(std::move(t_statList)) {
}


ExpStat::ExpStat(std::unique_ptr<Exp> t_exp) : exp(std::move(t_exp)){
}


StatType FuncDefStat::GetType() const noexcept {
	return StatType::kFuncDef;
}

FuncDefStat::FuncDefStat(const std::string& t_funcName, const std::vector<std::string>& t_parList, std::unique_ptr<BlockStat> t_block):
	funcName(t_funcName), parList(t_parList), block(std::move(t_block)) {
}



StatType IfStat::GetType() const noexcept {
	return StatType::kIf;
}

IfStat::IfStat(std::unique_ptr<Exp> t_exp, std::unique_ptr<BlockStat> t_block, std::vector<std::unique_ptr<ElifStat>>&& t_elifStatList, std::unique_ptr<ElseStat> t_elseStat):
	exp(std::move(t_exp)), block(std::move(t_block)), elifStatList(std::move(t_elifStatList)), elseStat(std::move(t_elseStat)) {
}


StatType ElifStat::GetType() const noexcept {
	return StatType::kElif;
}

ElifStat::ElifStat(std::unique_ptr<Exp> t_exp, std::unique_ptr<BlockStat> t_block) :
	exp(std::move(t_exp)), block(std::move(t_block)) {
}


StatType ElseStat::GetType() const noexcept {
	return StatType::kElse;
}

ElseStat::ElseStat(std::unique_ptr<BlockStat> t_block) :
	block(std::move(t_block)) {
}


StatType ForStat::GetType() const noexcept {
	return StatType::kFor;
}

ForStat::ForStat(const std::string& t_varName, std::unique_ptr<Exp> t_exp, std::unique_ptr<BlockStat> t_block):
	varName(t_varName), exp(std::move(t_exp)), block(std::move(t_block)) {
}


StatType WhileStat::GetType() const noexcept {
	return StatType::kWhile;
}

WhileStat::WhileStat(std::unique_ptr<Exp> t_exp, std::unique_ptr<BlockStat> t_block) :
	exp(std::move(t_exp)), block(std::move(t_block)) {
}


StatType ContinueStat::GetType() const noexcept {
	return StatType::kContinue;
}

StatType BreakStat::GetType() const noexcept {
	return StatType::kBreak;
}

StatType ReturnStat::GetType() const noexcept {
	return StatType::kReturn;
}

ReturnStat::ReturnStat(std::unique_ptr<Exp> t_exp) : exp(std::move(t_exp)) {
}


StatType NewVarStat::GetType() const noexcept {
	return StatType::kNewVar;
}

NewVarStat::NewVarStat(const std::string& t_varName, std::unique_ptr<Exp> t_exp) : varName(t_varName), exp(std::move(t_exp)) {
}


StatType AssignStat::GetType() const noexcept {
	return StatType::kAssign;
}

AssignStat::AssignStat(const std::string& t_varName, std::unique_ptr<Exp> t_exp) : varName(t_varName), exp(std::move(t_exp)) {
}


} // namespace ast

