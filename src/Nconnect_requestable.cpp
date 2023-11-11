#include "cgdk/asio.h"


void CGDK::asio::Nconnect_requestable::start(boost::asio::ip::tcp::endpoint _endpoint_connect)
{
	// 1) set socket state ESOCKET_STATUE::ESTABLISHED
	{
		// - desiged state
		ESOCKET_STATUE socket_state_old = ESOCKET_STATUE::NONE;

		// - change state
		auto changed = this->m_socket_state.compare_exchange_weak(socket_state_old, ESOCKET_STATUE::SYN);

		// check)
		assert(changed == true);

		// return) 
		if (changed == false)
			throw std::runtime_error("socket aleady connected or tring connectiong");
	}

	// statistics) 
	++Nstatistics::statistics_connect_try;

	// 2) request connect
	this->m_socket.async_connect(_endpoint_connect, [=,this](const boost::system::error_code& _error)
		{
			this->process_connect_request_complete(_error);
		});
}

void CGDK::asio::Nconnect_requestable::process_connect_request_complete(const boost::system::error_code& _error)
{
	// check) 실패했을 경우 socket을 등록 해제후 close하고 끝낸다.
	if (_error)
	{
		// - rollback (set socket state ESOCKET_STATUE::NONE)
		this->m_socket_state.exchange(ESOCKET_STATUE::NONE);

		// return) 
		return;
	}

	try
	{
		// 1) process connect socket
		this->process_connect_complete();

		// statistics) 
		++Nstatistics::statistics_connect_success;
		++Nstatistics::statistics_connect_keep;
	}
	catch (...)
	{
		// - rollback (set socket state ESOCKET_STATUE::NONE)
		this->m_socket_state.exchange(ESOCKET_STATUE::NONE);
	}
}
