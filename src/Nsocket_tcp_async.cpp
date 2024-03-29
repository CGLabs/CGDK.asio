﻿#include "cgdk/asio.h"


CGDK::asio::Nsocket_tcp_async::Nsocket_tcp_async()
{
}

CGDK::asio::Nsocket_tcp_async::~Nsocket_tcp_async() noexcept
{
	// 1) complete close socket
	this->process_closesocket(boost::asio::error::connection_aborted);
}

void CGDK::asio::Nsocket_tcp_async::process_closesocket(boost::system::error_code _error_code) noexcept
{
	// 1) abortive close
	this->process_close_native_handle();

	// 2) set socket state ESOCKET_STATUE::CLOSING
	{
		// - desiged state
		ESOCKET_STATUE socket_state_old = ESOCKET_STATUE::ESTABLISHED;

		// - change state
		auto changed = this->m_socket_state.compare_exchange_weak(socket_state_old, ESOCKET_STATUE::CLOSING);

		// return) 
		if (changed == false)
			return;
	}

	// 3) clear
	{
		// lock) 
		std::unique_lock lock(this->m_lock_socket);

		// - clear buffers
		this->m_send_msgs.clear();
	}

	// statistics)
	++Nstatistics::statistics_connect_disconnect;
	--Nstatistics::statistics_connect_keep;

	// 4) call 'on disconnect'
	this->on_disconnect(_error_code);

	// 5) remove form acceptor
	this->process_connective_closesocket();

	// 6) set socket state ESOCKET_STATUE::NONE
	{
		// - ...
		auto old_socket_state = this->m_socket_state.exchange(ESOCKET_STATUE::NONE);

		// check)
		assert(old_socket_state == ESOCKET_STATUE::CLOSING);

		// check)
		if (old_socket_state != ESOCKET_STATUE::CLOSING)
			return;
	}
}

bool CGDK::asio::Nsocket_tcp_async::process_send(SEND_NODE&& _send_node)
{
	// check) 
	if (this->m_socket_state < ESOCKET_STATUE::CLOSING)
		return false;

	// statistics)
	this->m_time_last_send = std::chrono::system_clock::now();

	// declare)
	bool result = true;

	try
	{
		// lock) 
		std::unique_lock lock(this->m_lock_socket);

		// 3) push buffer
		this->m_send_msgs.push_back(std::move(_send_node));

		// check)
		if (this->m_send_msgs.size() != 1)
			return true;

		try
		{
			// 4) send
			this->process_send_async(this->m_send_msgs.front());
		}
		catch (...)
		{
			// - rollback
			this->m_send_msgs.pop_back();

			// - 
			result = false;
		}
	}
	catch (...)
	{
		// - 
		result = false;
	}

	// return)
	return result;
}

void CGDK::asio::Nsocket_tcp_async::process_send_async(const SEND_NODE& _send_node)
{
	// 1) hold self
	if (!this->m_hold_send)
		this->m_hold_send = this->shared_from_this();

	// 2) 전송을 위한 boost::asio::const_buffer를 설정ㄴ
	boost::asio::const_buffer buffer_transfer { _send_node.buf_send.data(), _send_node.buf_send.size()};

	// 3) send async
	this->m_socket.async_write_some(buffer_transfer,
        [=, this](boost::system::error_code ec, std::size_t length)
        {
			// check) 
			if (ec)
			{
				// - release 
				this->m_hold_send.reset();

				// return) 
				return;
			}

			// declare) 
			std::shared_ptr<Isocket_tcp> hold_this;

			// lock) 
			std::unique_lock lock(m_lock_socket);

			// - get message
			auto& msg_send = this->m_send_msgs.front();

			// - process
			msg_send.buf_send += offset(length);

			// check) ...
			if (msg_send.buf_send.size() == 0)
			{
				Nstatistics::statistics_send_messages += msg_send.message_count;;
				this->m_send_msgs.pop_front();
			}

			// statistics)
			Nstatistics::statistics_send_bytes += length;

			// check) 
			if (this->m_send_msgs.empty() || this->m_socket_state < ESOCKET_STATUE::CLOSING)
			{
				// - release 
				hold_this = std::move(this->m_hold_send);

				// return) 
				return;
			}


			// check) 
			if (this->m_send_msgs.empty())
				return;

			// - send~ 
			try
			{
				this->process_send_async(this->m_send_msgs.front());
			}
			catch (...)
			{
				// - closesocket
				this->closesocket();

				// - release 
				hold_this = std::move(this->m_hold_send);
			}
        });
}
