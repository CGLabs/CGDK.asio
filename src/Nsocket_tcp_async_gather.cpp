﻿#include "cgdk/asio.h"


CGDK::asio::Nsocket_tcp_async_gather::Nsocket_tcp_async_gather()
{
}

CGDK::asio::Nsocket_tcp_async_gather::~Nsocket_tcp_async_gather() noexcept
{
	// 1) complete close socket
	this->process_closesocket(boost::asio::error::connection_aborted);
}

void CGDK::asio::Nsocket_tcp_async_gather::process_closesocket(boost::system::error_code _error_code) noexcept
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
		this->m_sending = SEND_NODE();
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

bool CGDK::asio::Nsocket_tcp_async_gather::process_send(SEND_NODE&& _send_node)
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

		if (this->m_sending.buf_send.size() != 0)
		{
			// 3) push buffer
			this->m_send_msgs.push_back(std::move(_send_node));
		}
		else
		{
			this->m_sending = _send_node;

			try
			{
				// 4) send
				this->process_send_gather_async(std::move(_send_node));
			}
			catch (...)
			{
				// - rollback
				this->m_sending = SEND_NODE();

				// - 
				result = false;
			}
		}
	}
	catch (...)
	{
		// - set false 
		result = false;
	}

	// return)
	return result;
}

void CGDK::asio::Nsocket_tcp_async_gather::process_send_gather_async(const SEND_NODE& _send_node)
{
	// 1) hold self
	if (!this->m_hold_send)
		this->m_hold_send = this->shared_from_this();

	// 2) 전송을 위한 boost::asio::const_buffer
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

			// statistics)
			Nstatistics::statistics_send_bytes += this->m_sending.buf_send.size();

			// declare) 
			std::shared_ptr<Isocket_tcp> hold_this;

			// lock) 
			std::unique_lock lock(this->m_lock_socket);

			// - move
			auto temp_node = std::move(this->m_sending);

			// check)
			if (temp_node.buf_send.empty())
				return;

			// - proceding
			temp_node.buf_send += offset(length);

			// - send complete? 
			if (temp_node.buf_send.size() == 0)
			{
				Nstatistics::statistics_send_messages += temp_node.message_count;
			}
			else
			{
				this->m_send_msgs.push_front(std::move(temp_node));
			}

			// check) 그 사이에 queueing된 것이 있으면 전송하고 없으면 그냥 끝낸다. 
			if (this->m_send_msgs.empty() || this->m_socket_state < ESOCKET_STATUE::CLOSING)
			{
				// - release 
				hold_this = std::move(this->m_hold_send);

				// return) 
				return;
			}

			// - get queued size
			size_t size = 0;
			size_t message_count = 0;
			for (auto& iter : this->m_send_msgs)
			{
				size += iter.buf_send.size();
				message_count += iter.message_count;
			}

			// - send~ 
			try
			{
				// - alloc buffer
				auto buf_send = alloc_shared_buffer(size);

				// - gather buffers as one and clear buffers
				for (auto& iter : this->m_send_msgs)
				{
					buf_send.append(iter.buf_send.size(), iter.buf_send.data());
				}
				this->m_send_msgs.clear();

				// = set ...
				this->m_sending = SEND_NODE{ buf_send, message_count };

				// - send async again~
				this->process_send_gather_async(this->m_sending);
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