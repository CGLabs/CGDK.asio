﻿#include "cgdk/asio.h"


CGDK::asio::Isocket_tcp::Isocket_tcp() :
	m_socket(asio::system::get_io_service().get_executor()),
	m_socket_state(ESOCKET_STATUE::NONE)
{
}

CGDK::asio::Isocket_tcp::~Isocket_tcp() noexcept
{
}

bool CGDK::asio::Isocket_tcp::closesocket() noexcept
{
	// 1) set linger option (abortive close)
	{
		[[maybe_unused]] boost::system::error_code ec;
		boost::asio::socket_base::linger option(true, 0);
		this->m_socket.set_option(option, ec);
	}

	// 2) close native socket handle
	return this->process_close_native_handle();
}

bool CGDK::asio::Isocket_tcp::disconnect() noexcept
{
	// 1) set linger option (graceful close)
	{
		[[maybe_unused]] boost::system::error_code ec;
		boost::asio::socket_base::linger option(false, 0);
		this->m_socket.set_option(option, ec);
	}

	// declare) 
	boost::system::error_code ec;

	// 2) send shutdown
	this->m_socket.shutdown(boost::asio::socket_base::shutdown_send, ec);

	// return) result
	return !ec;
}

void CGDK::asio::Isocket_tcp::process_connective_closesocket() noexcept
{
	// declare) 
	std::shared_ptr<Nconnective> pconnective;

	// 1) get connective
	{
		// lock) 
		std::lock_guard cs(this->m_lock_socket);

		// check) 
		if (!this->m_pconnective)
			return;

		pconnective = this->m_pconnective;
	}

	// 2) clear acceptor
	pconnective->process_unregister_socket(this);
}

bool CGDK::asio::Isocket_tcp::process_close_native_handle() noexcept
{
	// lock) 
	std::lock_guard cs(this->m_lock_socket);

	// check) 
	if (this->m_socket.is_open() == false)
		return false;

	// declare) 
	boost::system::error_code ec;

	// 1) close socket
	this->m_socket.close(ec);

	// check)
	return !ec;
}
