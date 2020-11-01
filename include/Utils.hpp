#pragma once

#include <cstdint>
#include <exception>

enum class State : uint32_t {
	IDLE, LISTENING, CONNECTED
};

enum class Response : uint32_t {
	OK, ALREADY_IDLE, ALREADY_LISTENING, ALREADY_CONNECTED, CONNECTION_FAILED, LISTENING_FAILED, NOT_CONNECTED
};

//TODO: BETTER EXCEPTION THROW WITH IP ADDRESSES AND MORE

class ParsingError : public std::exception {
	const char* what() const noexcept override;
};

class SocketError : public std::exception {
	const char* what() const noexcept override;
};

class ConnectionError : public std::exception {
	const char* what() const noexcept override;
};