#include <EndPoint.hpp>

#include <iostream>
#include <thread>
#include <unistd.h>
#include <sys/socket.h>

constexpr time_t TIMEOUT = 10;

EndPoint::EndPoint() :
m_ID(socket(AF_INET, SOCK_STREAM, 0)), m_State(State::IDLE), m_EndPointAddress(),
m_InputAsync(), m_OutputAsync(),
m_InputLock(), m_OutputLock(), m_InputPackets(), m_OutputPackets()
{
	if (m_ID == -1) throw SocketError();
	timeval timeout;
	timeout.tv_sec = TIMEOUT;
	setsockopt(m_ID, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
}

EndPoint::EndPoint(int socket, Address& address) :
m_ID(socket), m_State(State::CONNECTED), m_EndPointAddress(address),
m_InputAsync(&EndPoint::_async_receive, this), m_OutputAsync(&EndPoint::_async_send, this),
m_InputLock(), m_OutputLock(), m_InputPackets(), m_OutputPackets()
{
	timeval timeout;
	timeout.tv_sec = TIMEOUT;
	setsockopt(m_ID, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
}

EndPoint::~EndPoint() {
	disconnect();
	if (m_ID != -1) close(m_ID);
}

Response EndPoint::connect(const Address& address) {
	if (m_State == State::CONNECTED) return Response::ALREADY_CONNECTED;
	//SET THE ADDRESS AND TRY TO CONNECT
	m_EndPointAddress = address;
	if (::connect(m_ID, reinterpret_cast<sockaddr*>(&m_EndPointAddress), sizeof(m_EndPointAddress)) == -1) {
		m_EndPointAddress = {}; //RESET THE ADDRESS
		return Response::CONNECTION_FAILED;
	}
	//SET THE NEW STATE
	m_State = State::CONNECTED;
	//ACTIVATE THE THREADS
	m_InputAsync = std::thread(&EndPoint::_async_receive, this);
	m_OutputAsync = std::thread(&EndPoint::_async_send, this);
	//RETURN OK
	return Response::OK;
}

Response EndPoint::disconnect() {
	if (m_State == State::IDLE) return Response::ALREADY_IDLE;
	//SET THE STATE TO IDLE (IMPORTANT TO DO THIS BEFORE STOPPING THE THREAD)
	m_State = State::IDLE;
	//SEND THE END-OF-CONNECTION PACKET
	shutdown(m_ID, SHUT_RD);
	//JOIN THE THREADS
	if (m_InputAsync.joinable()) m_InputAsync.join();
	if (m_OutputAsync.joinable()) m_OutputAsync.join();
	//SET THE ADDRESS TO ZERO
	m_EndPointAddress = {};
	//DELETE ALL PACKETS RECEIVED AND SENT
	{
		std::lock_guard<std::mutex> locker(m_InputLock);
		while (!m_InputPackets.empty()) m_InputPackets.pop();
	}
	{
		std::lock_guard<std::mutex> locker(m_OutputLock);
		while (!m_OutputPackets.empty()) m_OutputPackets.pop();
	}
	//RETURN OK
	return Response::OK;
}

Response EndPoint::send(const Packet& packet) {
	if (m_State != State::CONNECTED) return Response::NOT_CONNECTED;
	std::lock_guard<std::mutex> locker(m_OutputLock);
	m_OutputPackets.emplace(packet);
	return Response::OK;
}

Response EndPoint::send(Packet&& packet) {
	if (m_State != State::CONNECTED) return Response::NOT_CONNECTED;
	std::lock_guard<std::mutex> locker(m_OutputLock);
	m_OutputPackets.emplace(std::move(packet));
	return Response::OK;
}

void EndPoint::_async_send() {
	while (m_State == State::CONNECTED) {
		while (!m_OutputPackets.empty()) {
			m_OutputLock.lock();
			Packet& packet = m_OutputPackets.front();
			m_OutputLock.unlock();
			uint32_t packet_size = packet.size();
			::send(m_ID, &packet_size, sizeof(uint32_t), 0);
			::send(m_ID, packet.data(), packet.size(), 0);
			m_OutputLock.lock();
			m_OutputPackets.pop();
			m_OutputLock.unlock();
		}
		std::this_thread::sleep_for(std::chrono::microseconds(100));
	}
}

void EndPoint::_async_receive() {
	while (m_State == State::CONNECTED) {
		int packet_bytes; //NUMBER OF BYTES OF THE NEXT PACKET
		int error_flag = recv(m_ID, &packet_bytes, sizeof(uint32_t), MSG_WAITALL); //WAITALL ENSURES ALL THE BYTES ARE RECEIVED
		if (error_flag > 0) { //IF THERE ARE NO ERRORS, SO NO DISCONNECTIONS
			Packet packet(packet_bytes); //ALLOCATE ENOUGH MEMORY TO RECEIVE THE PACKET
			if (recv(m_ID, packet.data(), packet_bytes, MSG_WAITALL) == -1) { //WAIT FOR ALL THE MESSAGE TO BE RECEIVED
				break;
			}
			std::lock_guard<std::mutex> locker(m_InputLock);
			m_InputPackets.emplace(std::move(packet));
		} else break; //CALL THE DISCONNECTOR BECAUSE THE ENDPOINT DECIDED TO DISCONNECT
	}
	std::thread(&EndPoint::disconnect, this).detach(); //DISCONNECTOR
}

const State& EndPoint::getState() const noexcept { return m_State; }
const Address& EndPoint::getPeerAddress() const noexcept { return m_EndPointAddress; }

Packet& EndPoint::front() {
	std::lock_guard<std::mutex> locker(m_InputLock);
	return m_InputPackets.front();
}

void EndPoint::pop() {
	std::lock_guard<std::mutex> locker(m_InputLock);
	m_InputPackets.pop();
}

size_t EndPoint::size() const { return m_InputPackets.size(); }
bool EndPoint::empty() const { return m_InputPackets.empty(); }