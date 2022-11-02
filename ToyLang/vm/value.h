#ifndef VM_VALUE_H_
#define VM_VALUE_H_

#include <string>

namespace vm {

enum class ValueType {
	kNull = 0,
	kBool,
	kNumber,
	kString,
};


class Value {
public:
	virtual ValueType GetType() const noexcept = 0;

	bool operator<(const Value& value) const;
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


} // namespace vm

#endif // VM_VALUE_H_