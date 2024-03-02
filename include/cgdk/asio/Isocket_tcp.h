//*****************************************************************************
//*                                                                           *
//*                      Cho sanghyun's Game Classes II                       *
//*                                                                           *
//*                          asio.ex network classes                          *
//*                                                                           *
//*                                                                           *
//*                                                                           *
//*                                                                           *
//*  This Program is programmed by Cho SangHyun. sangduck@cgcii.co.kr         *
//*  Best for Game Developement and Optimized for Game Developement.          *
//*                                                                           *
//*                (c) 2019. Cho Sanghyun. All right reserved.                *
//*                          http://www.CGCII.co.kr                           *
//*                                                                           *
//*****************************************************************************
#pragma once

class CGDK::asio::Isocket_tcp : public std::enable_shared_from_this<Isocket_tcp>
{
public:
	struct SEND_NODE { shared_buffer buf_send; size_t message_count{ 0 }; };

			Isocket_tcp();
	virtual ~Isocket_tcp() noexcept;

			bool send(const shared_buffer& _buffer) { return this->send({ shared_buffer(_buffer), 1 }); }
			bool send(shared_buffer&& _buffer) { return this->process_send({ std::move(_buffer), 1 }); }
			bool send(SEND_NODE&& _send) { return this->process_send(std::move(_send)); }
	virtual bool closesocket() noexcept;
	virtual bool disconnect() noexcept;
			auto& native_handle() noexcept { return m_socket; }

protected:
	virtual void process_connect_complete() = 0;
	virtual bool process_send(SEND_NODE&& _send_node) = 0;
	virtual void process_closesocket(boost::system::error_code _error_code) noexcept = 0;

			void process_connective_closesocket() noexcept;
			bool process_close_native_handle() noexcept;

			tcp::socket	m_socket;
			std::recursive_mutex m_lock_socket;
			std::shared_ptr<Nconnective> m_pconnective;
			std::atomic<ESOCKET_STATUE> m_socket_state;
			friend class Nconnective;
			friend class Nconnector;
			friend class Nacceptor;

			std::chrono::system_clock::time_point m_time_connect;
			std::chrono::system_clock::time_point m_time_disconnect;
			std::chrono::system_clock::time_point m_time_last_send;
			std::chrono::system_clock::time_point m_time_last_receives;
};
