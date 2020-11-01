#include <Listener.hpp>

#include <unistd.h>

Listener::Listener() : m_ID(socket(AF_INET, SOCK_STREAM, 0)), m_State(State::IDLE), m_ListeningAddress(), m_Listener() {
	//IF SOCKET ASSEGNATION FAILS, THROWS AN ERROR
	if (m_ID == -1) throw SocketError();
}

Listener::~Listener() {
	//STOP LISTENING
	stopListening();
	//DESTROY THE SOCKET
	if (m_ID != -1) close(m_ID);
}

Response Listener::startListening(in_port_t port, std::function<void(std::shared_ptr<EndPoint>)> func) {
	if (m_State != State::IDLE) return Response::ALREADY_LISTENING;
	//SET PORT AND BIND TO THE SOCKET
	m_ListeningAddress.setPort(port);
	bind(m_ID, reinterpret_cast<const sockaddr*>(&m_ListeningAddress), sizeof(m_ListeningAddress));
	//SET THE LISTENING STATE WITH N CONNECTIONS IN THE QUEUE
	if (listen(m_ID, 3) != 0) return Response::LISTENING_FAILED;
	//SET THE ACCEPT FUNCTION
	m_AcceptFunction = func;
	//START LISTENING
	m_Listener = std::thread(&Listener::_async_listen, this);
	//OK, EVERYTHING STARTED
	m_State = State::LISTENING;
	return Response::OK;
}

Response Listener::stopListening() {
	if (m_State != State::LISTENING) return Response::ALREADY_IDLE;
	//SET THE STATE TO IDLE
	m_State = State::IDLE;
	//SEND THE SHUTDOWN SIGNAL
	shutdown(m_ID, SHUT_RD);
	//JOIN THE THREAD
	if (m_Listener.joinable()) m_Listener.join();
	//REMOVE THE LISTENING ADDRESS AND THE ACCEPT FUNCTION
	m_ListeningAddress = {};
	m_AcceptFunction = {};
	//RETURN OK
	return Response::OK;
}

const State& Listener::getState() const noexcept { return m_State; }

void Listener::_async_listen() {
	Address new_address;
	socklen_t sockaddr_in_size = sizeof(new_address);
	while (m_State == State::LISTENING) {
		int new_socket = accept(m_ID, reinterpret_cast<sockaddr*>(&new_address), &sockaddr_in_size);
		if (new_socket > -1) { //AN ERROR HAPPENED, DO NOTHING
			m_AcceptFunction(std::make_shared<EndPoint>(new_socket, new_address));
		};
	}
	std::thread(&Listener::stopListening, this).detach(); //stopListening() HAS BEEN CALLED OR AN ERROR OCCURED
}