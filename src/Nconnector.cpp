#include "cgdk/asio.h"


CGDK::asio::Nconnector::Nconnector()
{
}

CGDK::asio::Nconnector::~Nconnector() noexcept
{
}

void CGDK::asio::Nconnector::start(const boost::asio::any_io_executor& _executor)
{
	// 1) create socket
	this->m_executor = _executor;
}

void CGDK::asio::Nconnector::start()
{
	this->start(asio::system::get_io_service().get_executor());
}

void CGDK::asio::Nconnector::close() noexcept
{
}

void CGDK::asio::Nconnector::request_connect(boost::asio::ip::tcp::endpoint _endpoint_connect)
{
	// 1) allock new socket
	auto psocket_new = this->process_create_socket();

	// 2) 
	this->request_connect(psocket_new, _endpoint_connect);
}

void CGDK::asio::Nconnector::request_connect(std::shared_ptr<Isocket_tcp> _socket_new, boost::asio::ip::tcp::endpoint _endpoint_connect)
{
	// check) 
	assert(_socket_new);

	// check)
	if (!_socket_new)
		throw std::bad_alloc();

	// 1) set socket state ESOCKET_STATUE::ESTABLISHED
	{
		// - desiged state
		ESOCKET_STATUE socket_state_old = ESOCKET_STATUE::NONE;

		// - change state
		auto changed = _socket_new->m_socket_state.compare_exchange_weak(socket_state_old, ESOCKET_STATUE::SYN);

		// check)
		assert(changed == true);

		// return) 
		if (changed == false)
			throw std::runtime_error("socket aleady connected or tring connectiong");
	}

	try
	{
		// 2) register socket first
		this->process_register_socket(_socket_new);

		// statistics) 
		++Nstatistics::statistics_connect_try;

		// 3) process connect request 
		_socket_new->process_request_connect();

		// 4) request connect
		_socket_new->native_handle().async_connect(_endpoint_connect, [=, this](const boost::system::error_code& _error)
			{
				this->process_connect_completion(_socket_new, _error);
			});
	}
	catch (...)
	{
		// - rollback
		this->process_unregister_socket(_socket_new);

		// - rollback (set socket state ESOCKET_STATUE::NONE)
		_socket_new->m_socket_state.exchange(ESOCKET_STATUE::NONE);

		// reraise)
		throw;
	}
}

void CGDK::asio::Nconnector::process_connect_completion(std::shared_ptr<Isocket_tcp> _socket, const boost::system::error_code& _error)
{
	try
	{
		// check) 실패했을 경우 등록해제하고 소켓을 닫고 끝낸다.
		if (_error)
			throw std::runtime_error("connection failure");

		// 1) process connect socket
		_socket->process_complete_connect();

		// statistics) 
		++Nstatistics::statistics_connect_success;
		++Nstatistics::statistics_connect_keep;
	}
	catch (...)
	{
		// - rollback
		_socket->process_connective_closesocket();

		// - rollback (set socket state ESOCKET_STATUE::NONE)
		_socket->m_socket_state.exchange(ESOCKET_STATUE::NONE);
	}
}
