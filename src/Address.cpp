#include <Address.hpp>
#include <Utils.hpp>

Address::Address() : m_Address() {
	m_Address.sin_family = AF_INET;
	m_Address.sin_addr.s_addr = 0;
	m_Address.sin_port = 0;
}

Address::Address(const std::string& ip, uint16_t port) : m_Address() {
	m_Address.sin_family = AF_INET;
	if (inet_pton(AF_INET, ip.c_str(), &(m_Address.sin_addr)) != 1) throw ParsingError();
	m_Address.sin_port = port;
}

Address::Address(const sockaddr_in& address) : m_Address(address) {}

void Address::setAddress(const std::string& ip) {
	if (inet_pton(AF_INET, ip.c_str(), &(m_Address.sin_addr)) != 1) throw ParsingError();
}

void Address::setPort(uint16_t port) noexcept {
	m_Address.sin_port = port;
}

Address::operator sockaddr_in() const noexcept { return m_Address; }

const uint32_t Address::getRawAddress() const noexcept {
	return m_Address.sin_addr.s_addr;
}

const std::string Address::getAddress() const {
	std::string address;
	address.resize(INET_ADDRSTRLEN);
	if (!inet_ntop(AF_INET, &m_Address, const_cast<char*>(address.data()), INET_ADDRSTRLEN)) throw ParsingError();
	return address;
}

const uint16_t Address::getPort() const noexcept {
	return m_Address.sin_port;
}

/* GET THE IP ADDRESS BASED ON THE SOCKET */
Address Address::fromSocket(int socket) {
	Address address;
	socklen_t length = sizeof(sockaddr);
	if (getsockname(socket, reinterpret_cast<sockaddr*>(&address), &length) != 0) throw SocketError();
	return address;
}

/* OPTIMIZED COMPARISON */
bool operator==(const Address& lhs, const Address& rhs) {
	return (lhs.m_Address.sin_addr.s_addr == rhs.m_Address.sin_addr.s_addr) &&
		   (lhs.m_Address.sin_port == rhs.m_Address.sin_port);
}
bool operator!=(const Address& lhs, const Address& rhs) {
	return (lhs.m_Address.sin_addr.s_addr != rhs.m_Address.sin_addr.s_addr) ||
		   (lhs.m_Address.sin_port != rhs.m_Address.sin_port);
}