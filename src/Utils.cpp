#include <Utils.hpp>

const char* ParsingError::what() const noexcept {
	return "Error parsing an IPv4 address from std::string to 4 byte integer!";
}

const char* SocketError::what() const noexcept {
	return "Error creating the socket!";
}

const char* ConnectionError::what() const noexcept {
	return "Error connecting to the socket!";
}