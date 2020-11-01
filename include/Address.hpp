#pragma once

#include <string>
#include <exception>
#include <sys/socket.h>
#include <arpa/inet.h>

class Address {

	public:
		Address();
		Address(const std::string&, uint16_t);
		Address(const sockaddr_in&);
		~Address() = default;

		Address(const Address&) = default;
		Address& operator=(const Address&) = default;

		Address(Address&&) noexcept = default;
		Address& operator=(Address&&) noexcept = default;

		void setAddress(const std::string&);
		void setPort(uint16_t) noexcept;

		operator sockaddr_in() const noexcept;

		const uint32_t getRawAddress() const noexcept;
		const std::string getAddress() const;
		const uint16_t getPort() const noexcept;

		static Address fromSocket(int);

		friend bool operator==(const Address&, const Address&);
		friend bool operator!=(const Address&, const Address&);
	
	private:
		sockaddr_in m_Address;
	
};