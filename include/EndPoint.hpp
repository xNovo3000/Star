#pragma once

#include <queue>
#include <mutex>
#include <thread>
#include <utility>

#include "Address.hpp"
#include "Packet.hpp"
#include "Utils.hpp"

namespace Star {

	class EndPoint {

		public:
			EndPoint();
			EndPoint(int, Address&);
			~EndPoint();

			EndPoint(const EndPoint&) = delete;
			EndPoint& operator=(const EndPoint&) = delete;

			EndPoint(EndPoint&&) noexcept = delete;
			EndPoint& operator=(EndPoint&&) noexcept = delete;

			Response connect(const Address&);
			Response disconnect();

			Response send(const Packet&);
			Response send(Packet&&);

			const State& getState() const noexcept;
			const Address& getPeerAddress() const noexcept;

			Packet& front();
			void pop();
			size_t size() const;
			bool empty() const;
		
		private:
			void _async_send();
			void _async_receive();

			int m_ID;
			State m_State;
			Address m_EndPointAddress;
			std::thread m_InputAsync, m_OutputAsync;
			std::mutex m_InputLock, m_OutputLock;
			std::queue<Packet> m_InputPackets, m_OutputPackets;

	};

}