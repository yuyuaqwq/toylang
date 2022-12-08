#include "section.h"

#include "value/value.h"

namespace toylang {

void ValueSection::Push(std::unique_ptr<Value>&& value) {
	m_container.push_back(std::move(value));
}

std::unique_ptr<Value> ValueSection::Pop() {
	auto value = std::move(m_container[m_container.size() - 1]);
	m_container.pop_back();
	return value;
}

// 负数表示从尾部索引起
std::unique_ptr<Value>& ValueSection::Get(int32_t index) {
	if (index >= 0) {
		return m_container[index];
	}
	else {
		return m_container[m_container.size() + index];
	}
}

void ValueSection::Set(int32_t index, std::unique_ptr<Value> value) {
	Get(index) = std::move(value);
}

size_t ValueSection::Size() const noexcept {
	return m_container.size();
}

void ValueSection::ReSize(size_t size) {
	return m_container.resize(size);
}

void ValueSection::Clear() noexcept {
	m_container.clear();
}

void ValueSection::operator=(ValueSection&& vs) noexcept {
	m_container = std::move(vs.m_container);
}

}