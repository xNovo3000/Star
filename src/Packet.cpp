#include <Packet.hpp>
#include <cstring>
#include <endian.h>

Packet::Packet() : m_Data(), m_Offset(0) {}
Packet::Packet(uint32_t initialSize) : m_Data(initialSize), m_Offset(0) {}

void Packet::append(const void* ptr, uint32_t size) {
	const uint8_t* _ptr = reinterpret_cast<const uint8_t*>(ptr);
	m_Data.insert(m_Data.end(), _ptr, _ptr + size);
}

void Packet::read(void* ptr, uint32_t size) const {
	memcpy(ptr, m_Data.data() + m_Offset, size);
	m_Offset += size;
}

Packet& Packet::operator<<(const uint32_t value) {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	value = htole32(value);
#endif
	append(&value, sizeof(uint32_t));
	return *this;
}

Packet& Packet::operator<<(const uint16_t value) {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	value = htole16(value);
#endif
	append(&value, sizeof(uint16_t));
	return *this;
}

Packet& Packet::operator<<(const uint8_t value) {
	m_Data.push_back(value);
	return *this;
}

Packet& Packet::operator<<(const float value) {
	append(&value, sizeof(float));
	return *this;
}

Packet& Packet::operator<<(const std::string& value) {
	m_Data.insert(m_Data.end(), value.begin(), value.end());
	m_Data.push_back(0);
	return *this;
}

const Packet& Packet::operator>>(uint32_t& value) const {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	value = htobe32(*reinterpret_cast<const uint32_t*>(&m_Data[m_Offset]));
#else
	value = *reinterpret_cast<const uint32_t*>(&m_Data[m_Offset]);
#endif
	m_Offset += sizeof(uint32_t);
	return *this;
}

const Packet& Packet::operator>>(uint16_t& value) const {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	value = htobe16(*reinterpret_cast<const uint32_t*>(&m_Data[m_Offset]));
#else
	value = *reinterpret_cast<const uint16_t*>(&m_Data[m_Offset]);
#endif
	m_Offset += sizeof(uint16_t);
	return *this;
}

const Packet& Packet::operator>>(uint8_t& value) const {
	value = m_Data[m_Offset];
	m_Offset += sizeof(uint8_t);
	return *this;
}

const Packet& Packet::operator>>(float& value) const {
	value = *reinterpret_cast<const float*>(&m_Data[m_Offset]);
	m_Offset += sizeof(float);
	return *this;
}

const Packet& Packet::operator>>(std::string& value) const {
	value = reinterpret_cast<const char*>(&m_Data[m_Offset]);
	m_Offset += value.size() + 1;
	return *this;
}

void Packet::reserve(uint32_t reserve) { m_Data.reserve(reserve); }
void Packet::resize(uint32_t resize) { m_Data.reserve(resize); }

const uint8_t* Packet::data() const noexcept { return m_Data.data(); }
uint8_t* Packet::data() noexcept { return m_Data.data(); }
uint32_t Packet::size() const noexcept { return m_Data.size(); }
bool Packet::empty() const noexcept { return m_Data.empty(); }