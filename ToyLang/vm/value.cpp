#include "value.h"

namespace vm {

bool Value::operator<(const Value& value) const {
	if (GetType() == value.GetType()) {
		switch (GetType()) {
		case ValueType::kNull: {
			return false;
		}
		case ValueType::kBool: {
			auto a = static_cast<const BoolValue*>(this);
			auto b = static_cast<const BoolValue*>(&value);
			return a->value == false && b->value == true;
		}
		case ValueType::kNumber: {
			auto a = static_cast<const NumberValue*>(this);
			auto b = static_cast<const NumberValue*>(&value);
			return a->value < b->value;
		}
		case ValueType::kString: {
			auto a = static_cast<const StringValue*>(this);
			auto b = static_cast<const StringValue*>(&value);
			return a->value < b->value;
		}
		case ValueType::kFunctionProto:
		case ValueType::kFunctionBody: {
			return false;
		}
		}
	}
	return GetType() < value.GetType();
}


FunctionProtoValue* Value::GetFunctionProto() {
	return static_cast<FunctionProtoValue*>(this);
}

FunctionBodyValue* Value::GetFunctionBody() {
	return static_cast<FunctionBodyValue*>(this);
}

}