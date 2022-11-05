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
		case ValueType::kFunctionProto: {
			auto a = static_cast<const FunctionProtoValue*>(this);
			auto b = static_cast<const FunctionProtoValue*>(&value);
			return a->bodyValue < b->bodyValue;
		}
		case ValueType::kFunctionBridge:
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

FunctionBridgeValue* Value::GetFunctionBirdge() {
	return static_cast<FunctionBridgeValue*>(this);
}

NumberValue* Value::GetNumber() {
	return static_cast<NumberValue*>(this);
}

StringValue* Value::GetString() {
	return static_cast<StringValue*>(this);
}

UpValue* Value::GetUp() {
	return static_cast<UpValue*>(this);
}





ValueType NullValue::GetType() const noexcept {
	return ValueType::kNull;
}

std::unique_ptr<Value> NullValue::Copy() const {
	return std::make_unique<NullValue>();
}



ValueType BoolValue::GetType() const noexcept {
	return ValueType::kBool;
}

std::unique_ptr<Value> BoolValue::Copy() const {
	return std::make_unique<BoolValue>(value);
}

BoolValue::BoolValue(bool t_value) : value(t_value) {

}



ValueType NumberValue::GetType() const noexcept {
	return ValueType::kNumber;
}

std::unique_ptr<Value> NumberValue::Copy() const {
	return std::make_unique<NumberValue>(value);
}

NumberValue::NumberValue(uint64_t t_value) : value(t_value) {

}


ValueType StringValue::GetType() const noexcept {
	return ValueType::kString;
}

std::unique_ptr<Value> StringValue::Copy() const {
	return std::make_unique<StringValue>(value);
}

StringValue::StringValue(const std::string& t_value) : value(t_value) {

}


ValueType FunctionBridgeValue::GetType() const noexcept {
	return ValueType::kFunctionBridge;
}

std::unique_ptr<Value> FunctionBridgeValue::Copy() const {
	return std::make_unique<FunctionProtoValue>(const_cast<FunctionBridgeValue*>(this));
}

FunctionBridgeValue::FunctionBridgeValue(FunctionBridgeCall t_funcAddr) : funcAddr(t_funcAddr) {

}



ValueType FunctionBodyValue::GetType() const noexcept {
	return ValueType::kFunctionBody;
}

std::unique_ptr<Value> FunctionBodyValue::Copy() const {
	return std::make_unique<FunctionProtoValue>(const_cast<FunctionBodyValue*>(this));
}

FunctionBodyValue::FunctionBodyValue(uint32_t t_parCount) : parCount(t_parCount) {

}

std::string FunctionBodyValue::Disassembly() {
	std::string str;
	for (int pc = 0; pc < instrSect.container.size(); ) {
		const auto& info = g_instrSymbol.find(instrSect.GetOpcode(pc++));
		str += info->second.str + " ";
		for (const auto& parSize : info->second.parSizeList) {
			if (parSize == 4) {
				auto ki = instrSect.GetU32(pc);
				str += std::to_string(ki) + " ";
			}
			pc += parSize;

		}
		str += "\n";
	}
	return str;
}


ValueType FunctionProtoValue::GetType() const noexcept {
	return ValueType::kFunctionProto;
}

std::unique_ptr<Value> FunctionProtoValue::Copy() const {
	return std::make_unique<FunctionProtoValue>(bodyValue);
}

FunctionProtoValue::FunctionProtoValue(FunctionBodyValue* t_value) : bodyValue(t_value) {

}


FunctionProtoValue::FunctionProtoValue(FunctionBridgeValue* t_value) : bridgeValue(t_value) {

}




ValueType UpValue::GetType() const noexcept {
	return ValueType::kUp;
}

std::unique_ptr<Value> UpValue::UpValue::Copy() const {
	return std::make_unique<UpValue>(index, funcProto);
}

UpValue::UpValue(uint32_t t_index, FunctionBodyValue* t_funcProto): index(t_index), funcProto(t_funcProto) {

}




}