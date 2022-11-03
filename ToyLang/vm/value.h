#ifndef VM_VALUE_H_
#define VM_VALUE_H_

#include <string>

namespace vm {

enum class ValueType {
	kNull = 0,
	kBool,
	kNumber,
	kString,
	kFunction,
};


class Value {
public:
	virtual ValueType GetType() const noexcept = 0;

	bool operator<(const Value& value) const;

	FunctionValue* GetFunction() {
		return static_cast<FunctionValue*>(this);
	}


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

class FunctionValue : public Value {
public:
	virtual ValueType GetType() const noexcept {
		return ValueType::kFunction;
	}

	FunctionValue(uint32_t t_addr, uint32_t t_parCount) : addr(t_addr), parCount(t_parCount) {

	}

public:
	uint32_t addr;
	uint32_t parCount;
};

} // namespace vm

#endif // VM_VALUE_H_