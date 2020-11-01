#pragma once

#include <thread>
#include <memory>
#include <functional>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "Address.hpp"
#include "EndPoint.hpp"
#include "Utils.hpp"

class Listener {

	public:
		Listener();
		~Listener();

		Listener(const Listener&) = delete;
		Listener& operator=(const Listener&) = delete;

		Listener(Listener&&) noexcept = delete;
		Listener& operator=(Listener&&) noexcept = delete;

		Response startListening(in_port_t, std::function<void(std::shared_ptr<EndPoint>)>);
		Response stopListening();

		const State& getState() const noexcept;

	private:
		void _async_listen();

		int m_ID;
		State m_State;
		Address m_ListeningAddress;
		std::thread m_Listener;
		std::function<void(std::shared_ptr<EndPoint>)> m_AcceptFunction;

};