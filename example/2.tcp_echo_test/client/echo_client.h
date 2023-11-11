#pragma once

#include "cgdk/asio.h"
#include <future>

class socket_tcp : public asio::Nsocket_tcp_async
{
protected:
	virtual void on_connect() {}
	virtual void on_disconnect() noexcept {}
	virtual int on_message(shared_buffer& /*_msg*/) { return 0; }
};

class test_tcp_echo_client
{
public:
			test_tcp_echo_client();
	virtual ~test_tcp_echo_client() noexcept;

public:
			void				enable_connect_test(bool _enable = true) noexcept;
			void				toggle_connect_test() noexcept { enable_connect_test(!m_enable_connect_test); }
			void				enable_traffic_test(bool _enable = true) noexcept;
			void				toggle_traffic_test() noexcept { enable_traffic_test(!m_enable_traffic_test); }
			void				enable_relay_echo_test(bool _enable = true) noexcept { m_s_enable_relay_echo_test = _enable; }
			void				toggle_relay_echo_test() noexcept { enable_relay_echo_test(!m_s_enable_relay_echo_test); }
			void				enable_disconnect_on_response(bool _enable = true) noexcept { m_s_enable_disconnect_on_response = _enable; }
			void				toggle_disconnect_on_response() noexcept { enable_disconnect_on_response(!m_s_enable_disconnect_on_response); }

			void				add_traffic_test_count_per_single(std::size_t _count) noexcept;
			void				sub_traffic_test_count_per_single(std::size_t _count) noexcept;
			void				set_traffic_test_count_per_single(std::size_t _count) noexcept;

			void				increase_traffic_test_message_size();
			void				decrease_traffic_test_message_size();
			void				set_traffic_test_message_size(std::size_t _index);

			void				add_connect_test_min(std::size_t _count) noexcept;
			void				sub_connect_test_min(std::size_t _count) noexcept;
			void				set_connect_test_min(std::size_t _count) noexcept;
			void				add_connect_test_max(std::size_t _count) noexcept;
			void				sub_connect_test_max(std::size_t _count) noexcept;
			void				set_connect_test_max(std::size_t _count) noexcept;

			void				set_endpoint(boost::asio::ip::tcp::endpoint _endpoint);
			bool				set_endpoint(const std::string& _address, const std::string& _port);
			const auto&			get_endpoint() const noexcept { return m_endpoint_connect; }

			void				request_connect(std::size_t _count);
			void				request_closesocket(std::size_t _count);
			void				request_closesocket_all();
			void				request_disconnect_socket(std::size_t _count);
			void				request_disconnect_socket_all();
			void				request_send();
			void				request_send_immidiately(std::size_t _count);
			void				request_send_error_message_zero();

public:
	// 1) connect test
			bool				m_enable_connect_test = false;
			std::size_t			m_connect_test_min = 0;
			std::size_t			m_connect_test_max = 0;

	// 2) send test
			std::atomic<uint64_t> m_traffic_send_requested = 0;
			bool				m_enable_traffic_test = false;
			std::size_t			m_traffic_test_selected = 0;
			std::size_t			m_traffic_test_count_per_single = 0;
			shared_buffer		m_traffic_test_buf_message[11]{};
			std::size_t			m_traffic_test_message_count[11]{};
			std::size_t			m_traffic_test_message_size[11]{};

			bool				m_is_thread_run = false;
			std::future<void>	m_result_future;
			std::unique_ptr<std::thread> m_thread_process;

	static	bool				m_s_enable_relay_echo_test;
	static	bool				m_s_enable_disconnect_on_response;

	// 3) connector
			boost::asio::ip::tcp::endpoint m_endpoint_connect;
			std::shared_ptr<asio::connector<socket_tcp>> m_pconnector_socket;

public:
	virtual	void				start();
	virtual	void				destroy();

public:
			void				process_execute();
			void				process_execute_send();

			void				process_connect_test();
			void				process_traffic_test();

			void				process_connect_request(std::size_t _n);
			void				process_send_message(std::size_t _index, std::size_t _count);
			void				process_send_message(shared_buffer _buffer);
			void				ready_send_test_message(std::size_t _index);
};
