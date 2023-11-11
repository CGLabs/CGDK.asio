#include "cgdk/asio.h"
#include "echo_client.h"
#include <functional>

using namespace std::literals;
bool test_tcp_echo_client::m_s_enable_relay_echo_test = false;
bool test_tcp_echo_client::m_s_enable_disconnect_on_response = false;
std::unique_ptr<test_tcp_echo_client> g_ptest_tcp_echo_client;

test_tcp_echo_client::test_tcp_echo_client()
{

}
test_tcp_echo_client::~test_tcp_echo_client() noexcept
{
	destroy();
}

void test_tcp_echo_client::add_connect_test_min(std::size_t _count) noexcept
{
	this->m_connect_test_min += _count;

	if (this->m_connect_test_min > this->m_connect_test_max)
		this->m_connect_test_max = this->m_connect_test_min;
}
void test_tcp_echo_client::sub_connect_test_min(std::size_t _count) noexcept
{
	if (this->m_connect_test_min < _count)
		this->m_connect_test_min = 0;
	else
		this->m_connect_test_min -= _count;
}
void test_tcp_echo_client::set_connect_test_min(std::size_t _count) noexcept
{
	this->m_connect_test_max = _count;
}

void test_tcp_echo_client::add_connect_test_max(std::size_t _count) noexcept
{
	this->m_connect_test_max += _count;
}
void test_tcp_echo_client::sub_connect_test_max(std::size_t _count) noexcept
{
	if (this->m_connect_test_max < 100 + _count)
		this->m_connect_test_max = 100;
	else
		this->m_connect_test_max -= _count;

	if (this->m_connect_test_max < this->m_connect_test_min)
	{
		this->m_connect_test_min = this->m_connect_test_max;
	}
}
void test_tcp_echo_client::set_connect_test_max(std::size_t _count) noexcept
{
	this->m_connect_test_max = _count;
}

void test_tcp_echo_client::set_endpoint(boost::asio::ip::tcp::endpoint _endpoint)
{
	this->m_endpoint_connect = _endpoint;
}

bool test_tcp_echo_client::set_endpoint(const std::string& _address, const std::string& _port)
{
	if (_address.empty() == false && _port.empty() == false)
	{
		// 1) ...
		auto result = boost::asio::ip::tcp::resolver(asio::system::get_io_service().get_executor()).resolve(_address, _port);

		// check)
		if(result.empty())
			return false;

		// 2) set endpoint
		this->m_endpoint_connect = *result.begin();
	}
	else if (_address.empty() == false)
	{
		// 1) ...
		auto result = boost::asio::ip::tcp::resolver(asio::system::get_io_service().get_executor()).resolve(_address);

		// check)
		if (result.empty())
			return false;

		// 2) set endpoint
		this->m_endpoint_connect.address(result.begin()->endpoint().address());
	}
	else if (_port.empty() == false)
	{
		this->m_endpoint_connect.port(static_cast<unsigned short>(std::stoi(_port)));
	}

	// return)
	return true;
}

void test_tcp_echo_client::enable_connect_test(bool _enable) noexcept
{
	// check)
	if (this->m_enable_connect_test == _enable)
		return;

	// 1) set flag
	this->m_enable_connect_test = _enable;
}
void test_tcp_echo_client::enable_traffic_test(bool _enable) noexcept
{
	// check)
	if (this->m_enable_traffic_test == _enable)
		return;

	// 1) set flag
	this->m_enable_traffic_test = _enable;

	if (_enable == false)
	{
		this->m_traffic_send_requested = 0;
	}
}

void test_tcp_echo_client::add_traffic_test_count_per_single(std::size_t _count) noexcept
{
	this->m_traffic_test_count_per_single += _count;
}

void test_tcp_echo_client::sub_traffic_test_count_per_single(std::size_t _count) noexcept
{
	if ((this->m_traffic_test_count_per_single + 1)  > _count)
	{
		this->m_traffic_test_count_per_single -= _count;
	}
	else
	{
		this->m_traffic_test_count_per_single  = 1;
	}
}

void test_tcp_echo_client::set_traffic_test_count_per_single(std::size_t _count) noexcept
{
	if (_count > 1)
	{
		this->m_traffic_test_count_per_single = _count;
	}
	else
	{
		this->m_traffic_test_count_per_single = 1;
	}
}

void test_tcp_echo_client::increase_traffic_test_message_size()
{
	if (this->m_traffic_test_selected >= 10)
		return;

	++this->m_traffic_test_selected;

	this->ready_send_test_message(this->m_traffic_test_selected);
}

void test_tcp_echo_client::decrease_traffic_test_message_size()
{
	if (this->m_traffic_test_selected == 0)
		return;

	--this->m_traffic_test_selected;

	this->ready_send_test_message(this->m_traffic_test_selected);
}
void test_tcp_echo_client::set_traffic_test_message_size(std::size_t _index)
{
	if(this->m_traffic_test_selected >= 10)
		return;

	if(this->m_traffic_test_selected == _index)
		return;

	this->m_traffic_test_selected = _index;

	this->ready_send_test_message(this->m_traffic_test_selected);
}

void test_tcp_echo_client::request_connect(std::size_t _count)
{
	for (std::size_t i = 0; i < _count; i++)
	{
		this->m_pconnector_socket->request_connect(this->m_endpoint_connect);
	}
}

void test_tcp_echo_client::request_closesocket(std::size_t _count)
{
	std::size_t disconnected = 0;

	{
		std::lock_guard cs(m_pconnector_socket->get_lockable());
	
		// 주의) closesocket을 할 경우 m_pconnector의 connectable_list에서 제거될수 있어
		//       iterator 무효화 현상이 발생할 수 있으므로 반드시 iterator 무효화에 대비해 처리해야 한다.
		auto iter = this->m_pconnector_socket->begin();
		auto iter_end = this->m_pconnector_socket->end();

		while (iter != iter_end)
		{
			auto iter_now = iter++;

			if ((*iter_now)->closesocket())
			{
				if((++disconnected) == _count)
					break;
			}
		}
	}
}

void test_tcp_echo_client::request_closesocket_all()
{
	this->m_pconnector_socket->close_connectable_all();
}

void test_tcp_echo_client::request_disconnect_socket(std::size_t _count)
{
	std::size_t disconnected = 0;

	{
		std::lock_guard cs(this->m_pconnector_socket->get_lockable());

		// 주의) disconnect을 할 경우 m_pconnector의 connectable_list에서 제거될수 있어
		//       iterator 무효화 현상이 발생할 수 있으므로 반드시 iterator 무효화에 대비해 처리해야 한다.
		auto iter = this->m_pconnector_socket->begin();
		auto iter_end = this->m_pconnector_socket->end();

		while (iter != iter_end)
		{
			auto iter_now = iter++;

			if ((*iter_now)->disconnect())
			{
				if((++disconnected) == _count)
					break;
			}
		}
	}
}

void test_tcp_echo_client::request_disconnect_socket_all()
{
	this->m_pconnector_socket->disconnect_connectable_all();
}

void test_tcp_echo_client::request_send()
{
	// 1) ...
	auto value = ++this->m_traffic_send_requested;

	// check) iPre가 1이면 Interlocked걸기.
	if (value == 1)
	{
		this->m_result_future = std::async(&test_tcp_echo_client::process_execute_send, this);
	}
}

void test_tcp_echo_client::request_send_immidiately(std::size_t _count)
{
	// 1) send
	this->process_send_message(m_traffic_test_selected, _count);
}

void test_tcp_echo_client::request_send_error_message_zero()
{
	auto tempBuffer = alloc_shared_buffer(256);
	tempBuffer.append<uint32_t>(0);
	tempBuffer.append<uint32_t>(1);

	this->process_send_message(tempBuffer);
}

void test_tcp_echo_client::start()
{
	// 1) connect test
	this->m_enable_connect_test = false;
	this->m_connect_test_min = 500;
	this->m_connect_test_max = 800;

	// 2) send test
	this->m_traffic_send_requested = 0;
	this->m_enable_traffic_test = false;
	this->m_traffic_test_selected = 0;
	this->m_traffic_test_count_per_single = 200;

	// 3) ready messges for send test
	this->ready_send_test_message(m_traffic_test_selected);

	// 4) Connector를 생성한다.
	this->m_pconnector_socket = std::make_shared<asio::connector<socket_tcp>>();
	this->m_endpoint_connect = boost::asio::ip::tcp::endpoint{ boost::asio::ip::address_v4::loopback(), 20000 };

	// 5) disable test
	this->enable_connect_test(false);
	this->enable_traffic_test(false);

	// 6) Connector를 시작한다.
	this->m_pconnector_socket->start();

	// 7) start schedulable
	this->m_is_thread_run = true;
	this->m_thread_process = std::make_unique<std::thread>([=,this]() { this->process_execute(); });
}

struct TEST_MESSAGE_TYPE
{
	std::size_t size;
	std::size_t count;
};

#define BUFFER_SIZE_SCALE 2
#define TEST_MESSAGE_TYPE_COUNT 12
const TEST_MESSAGE_TYPE g_test_message_info[TEST_MESSAGE_TYPE_COUNT]
{
	{                8, 1024 * 1024 * BUFFER_SIZE_SCALE}, // 0:   8 byte
	{               64,  256 * 1024 * BUFFER_SIZE_SCALE}, // 1:  64 byte
	{              256,   64 * 1024 * BUFFER_SIZE_SCALE}, // 2: 256 byte
	{             1024,   16 * 1024 * BUFFER_SIZE_SCALE}, // 3:  1K byte
	{         4 * 1024,    2 * 1024 * BUFFER_SIZE_SCALE}, // 4:  4K byte
	{        16 * 1024,    1 * 1024 * BUFFER_SIZE_SCALE}, // 5: 16K byte
	{        64 * 1024,         256 * BUFFER_SIZE_SCALE}, // 6: 64K byte
	{       256 * 1024,          64 * BUFFER_SIZE_SCALE}, // 6:256K byte
	{  1 * 1024 * 1024,          16 * BUFFER_SIZE_SCALE}, // 7:  1M byte
	{  4 * 1024 * 1024,           4 * BUFFER_SIZE_SCALE}, // 8:  4M byte
	{ 32 * 1024 * 1024,           1},                     // 9: 32M byte
	{256 * 1024 * 1024,           1}                      // 10:256M byte
};

CGDK::shared_buffer generate_message(size_t _message_size, size_t _count)
{
	#define SIZE_OF_CRC 4
	//#define	V_CRC crc()
	#define	V_CRC	0

	// check)
	assert(_message_size > 0);

	auto buffer_temp = alloc_shared_buffer(_message_size);
	buffer_temp.append<uint32_t>(static_cast<uint32_t>(_message_size));
	for (std::size_t k = 0; k < (_message_size - 8); k += sizeof(uint32_t))
		buffer_temp.append<uint32_t>(rand());
	buffer_temp.front<uint32_t>() = static_cast<uint32_t>(buffer_temp.size() + SIZE_OF_CRC);
	buffer_temp.append(V_CRC);

	auto buf_message = alloc_shared_buffer(_message_size * _count);
	for (std::size_t j = 0; j < _count; ++j)
	{
		buf_message += buffer_temp;
	}

	return buf_message;
}

void test_tcp_echo_client::ready_send_test_message(std::size_t _index)
{
	// check)
	assert(_index < TEST_MESSAGE_TYPE_COUNT);

	// check)
	if(_index >= TEST_MESSAGE_TYPE_COUNT)
		throw std::exception();

	// check)
	if (this->m_traffic_test_buf_message[_index].data() != nullptr)
		return;

	// check)
	if (this->m_s_enable_relay_echo_test == true)
		return;

	// 1) get message_buffer
	this->m_traffic_test_buf_message[_index] = generate_message(g_test_message_info[_index].size, g_test_message_info[_index].count);
}

void test_tcp_echo_client::destroy()
{
	// 1) disable test
	this->m_enable_connect_test = false;
	this->m_enable_traffic_test = false;

	// 2) unregister...
	this->m_is_thread_run = false;
	this->m_thread_process->join();

	// 3) destry
	this->m_pconnector_socket->close();
}

void test_tcp_echo_client::process_execute_send()
{
	// 1) send
	this->process_send_message(this->m_traffic_test_selected, this->m_traffic_test_count_per_single);

	// 2) ...
	auto value = --this->m_traffic_send_requested;

	// check)
	if (this->m_enable_traffic_test == false)
		return;

	// check) 0이 아닐 경우 또 걸기...
	if (value > 0)
	{
		auto result_future = std::async(&test_tcp_echo_client::process_execute_send, this);

		result_future.wait();
	}
}

void test_tcp_echo_client::process_execute()
{
	while (this->m_is_thread_run)
	{
		// 1) traffice test
		this->process_traffic_test();

		// 2) connection test
		this->process_connect_test();

		// 3) sleep
		std::this_thread::sleep_for(100ms);
	}
}

void test_tcp_echo_client::process_connect_test()
{
	// check) 
	if(this->m_enable_connect_test == false)
		return;

	// 1) disconnect
	size_t count_socket = 0;
	std::size_t disconnected = 0;

	{
		std::vector<std::shared_ptr<asio::Isocket_tcp>> vector_socket;
		{
			std::unique_lock lock(this->m_pconnector_socket->get_lockable());
			count_socket = this->m_pconnector_socket->count();
			vector_socket.reserve(count_socket);
			vector_socket.assign(this->m_pconnector_socket->begin(), this->m_pconnector_socket->end());
		}


		if (count_socket > m_connect_test_min)
		{
			std::size_t range = count_socket - m_connect_test_min;
			std::size_t disconnect = (range) ? rand() % range : 0;

			// 주의) closesocket을 할 경우 m_pconnector의 connectable_list에서 제거될수 있어
			//       iterator 무효화 현상이 발생할 수 있으므로 반드시 iterator 무효화에 대비해 처리해야 한다.
			for(auto& iter: vector_socket)
			{
				if (iter->closesocket())
				{
					if ((++disconnected) == disconnect)
						break;
				}
			}
		}
	}

	// 2) connect
	{
		count_socket -= disconnected;

		std::size_t nConnect;
		if (count_socket < this->m_connect_test_min)
		{
			// MinConnect보다 낮을 시 Min Connect만큼 한꺼번에 접속해버린다.
			nConnect = this->m_connect_test_min - count_socket;
		}
		else if (count_socket < this->m_connect_test_max)
		{
			auto range = this->m_connect_test_max - count_socket;

			// 여유최대 Remained/8(12.5%)의 값을 신규로 접속시도한다.
			nConnect = (range) ? rand() % range : 0;
		}
		else
		{
			nConnect = 0;
		}

		this->process_connect_request(nConnect);
	}
}

void test_tcp_echo_client::process_traffic_test()
{
	// check)
	if(this->m_enable_traffic_test == false)
		return;

	// 1) send 
	this->request_send();
}

void test_tcp_echo_client::process_connect_request(std::size_t _n)
{
	// 1) 접속한다.
	for (std::size_t i = 0; i < _n; i++)
	{
		this->m_pconnector_socket->request_connect(m_endpoint_connect);
	}
}

void test_tcp_echo_client::process_send_message(std::size_t _index, std::size_t _count)
{
	try
	{
		const auto message_info = g_test_message_info[_index];
		auto buf_message = m_traffic_test_buf_message[_index];

		{
			std::vector<std::shared_ptr<asio::Isocket_tcp>> vector_socket;
			{
				std::unique_lock lock(this->m_pconnector_socket->get_lockable());
				vector_socket.reserve(this->m_pconnector_socket->count());
				vector_socket.assign(this->m_pconnector_socket->begin(), this->m_pconnector_socket->end());
			}

			for (auto& iter : vector_socket)
			{
				std::size_t	nSend = _count;
				auto buf_send = buf_message;

				for (; nSend >= message_info.count; nSend -= message_info.count)
				{
					iter->send({ buf_send, message_info.count });
				}

				if (nSend != 0)
				{
					buf_send.set_size(message_info.size * nSend);
					iter->send({ buf_send, nSend });

				}
			}
		}
	}
	catch (...)
	{
	}
}

void test_tcp_echo_client::process_send_message(shared_buffer _buffer)
{
	std::vector<std::shared_ptr<asio::Isocket_tcp>> vector_socket;
	{
		std::unique_lock lock(this->m_pconnector_socket->get_lockable());
		vector_socket.reserve(this->m_pconnector_socket->count());
		vector_socket.assign(this->m_pconnector_socket->begin(), this->m_pconnector_socket->end());
	}

	for (auto& iter : vector_socket)
	{
		iter->send(_buffer);
	}
}