#ifndef VM_VALUE_H_
#define VM_VALUE_H_

#include <string>
#include <memory>

#include "vm/instr.h"

#include "value/section.h"

namespace value {

enum class ValueType {
	kNull = 0,
	kBool,
	kNumber,
	kString,
	kFunctionProto,
	kFunctionBody,
	kFunctionBridge,
	kUp,
};


class NullValue;
class BoolValue;
class NumberValue;
class StringValue;
class FunctionProtoValue;
class FunctionBodyValue;
class FunctionBridgeValue;
class UpValue;

class Value {
public:
	virtual ValueType GetType() const noexcept = 0;

	virtual std::unique_ptr<Value> Copy() const = 0;

	bool operator<(const Value& value) const;

	bool operator<=(const Value& value) const;

	bool operator==(const Value& value) const;



	BoolValue* GetBool();

	NumberValue* GetNumber();

	StringValue* GetString();

	FunctionProtoValue* GetFunctionProto();

	FunctionBodyValue* GetFunctionBody();

	FunctionBridgeValue* GetFunctionBirdge();

	UpValue* GetUp();

};

class NullValue :public Value {
public:
	virtual ValueType GetType() const noexcept;

	virtual std::unique_ptr<Value> Copy() const;
};


class BoolValue :public Value {
public:
	virtual ValueType GetType() const noexcept;

	virtual std::unique_ptr<Value> Copy() const;

	explicit BoolValue(bool t_value);

public:
	bool val;
};

class NumberValue :public Value {
public:
	virtual ValueType GetType() const noexcept;

	virtual std::unique_ptr<Value> Copy() const;

	explicit NumberValue(uint64_t t_value);

public:
	uint64_t val;
};


class StringValue :public Value {
public:
	virtual ValueType GetType() const noexcept;

	virtual std::unique_ptr<Value> Copy() const;

	explicit StringValue(const std::string& t_value);

public:
	std::string val;
};



// 关于函数的设计
// 函数体就是指令流的封装，只会存放在常量里，定义函数会在常量区创建函数
// 并且会在局部变量表中创建并函数原型，指向函数体，类似语法糖的想法
class FunctionBodyValue : public Value {
public:
	virtual ValueType GetType() const noexcept;

	virtual std::unique_ptr<Value> Copy() const;

	FunctionBodyValue(uint32_t t_parCount);

	std::string Disassembly();

public:
	uint32_t parCount;
	vm::InstrSection instrSect;
	ValueSection varSect;
};


typedef std::unique_ptr<Value>(*FunctionBridgeCall)(uint32_t parCount, ValueSection* stack);
class FunctionBridgeValue : public Value {
public:
	virtual ValueType GetType() const noexcept;

	std::unique_ptr<Value> Copy() const;

	FunctionBridgeValue(FunctionBridgeCall t_funcAddr);

public:
	FunctionBridgeCall funcAddr;
};

class FunctionProtoValue : public Value {
public:
	virtual ValueType GetType() const noexcept;

	virtual std::unique_ptr<Value> Copy() const;

	FunctionProtoValue(FunctionBodyValue* t_value);
	FunctionProtoValue(FunctionBridgeValue* t_value);

public:
	union
	{
		Value* val;
		FunctionBodyValue* bodyVal;
		FunctionBridgeValue* bridgeVal;
	};
	
};

class UpValue : public Value {
public:
	virtual ValueType GetType() const noexcept;

	virtual std::unique_ptr<Value> Copy() const;

	UpValue(uint32_t t_index, FunctionBodyValue* t_funcProto);

public:
	uint32_t index;
	FunctionBodyValue* funcProto;
};

} // namespace vm

#endif // VM_VALUE_H_