#ifndef VM_VALUE_H_
#define VM_VALUE_H_

#include <string>

#include "vm/instr.h"

namespace vm {

enum class ValueType {
	kNull = 0,
	kBool,
	kNumber,
	kString,
	kFunctionBody,
	kFunctionProto,
};

class FunctionProtoValue;
class FunctionBodyValue;
class Value {
public:
	virtual ValueType GetType() const noexcept = 0;

	bool operator<(const Value& value) const;

	FunctionProtoValue* GetFunctionProto();

	FunctionBodyValue* GetFunctionBody();

};

class NullValue :public Value {
public:
	virtual ValueType GetType() const noexcept {
		return ValueType::kNull;
	}
};


class BoolValue :public Value {
public:
	virtual ValueType GetType() const noexcept {
		return ValueType::kBool;
	}

	explicit BoolValue(bool t_value) : value(t_value) {

	}

public:
	bool value;
};

class NumberValue :public Value {
public:
	virtual ValueType GetType() const noexcept {
		return ValueType::kNumber;
	}

	explicit NumberValue(int t_value) : value(t_value) {

	}

public:
	int value;
};

class StringValue :public Value {
public:
	virtual ValueType GetType() const noexcept {
		return ValueType::kString;
	}

	explicit StringValue(const std::string& t_value) : value(t_value) {

	}

public:
	std::string value;
};



// 关于函数的设计
// 函数体就是指令流的封装，只会存放在常量里，定义函数会在常量区创建函数
// 并且会在局部变量表中创建并函数原型，指向函数体，类似语法糖的想法
class FunctionBodyValue : public Value {
public:
	virtual ValueType GetType() const noexcept {
		return ValueType::kFunctionBody;
	}

	FunctionBodyValue(uint32_t t_parCount) : parCount(t_parCount) {

	}

public:
	uint32_t parCount;
	InstrSection instrSect;
};


class FunctionProtoValue : public Value {
public:
	virtual ValueType GetType() const noexcept {
		return ValueType::kFunctionProto;
	}
public:
	FunctionBodyValue* value;
};



} // namespace vm

#endif // VM_VALUE_H_