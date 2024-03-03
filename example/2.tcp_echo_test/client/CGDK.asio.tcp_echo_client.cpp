#include "cgdk/asio.h"
#include "echo_client.h"
#include <iostream>
#include <sstream>
#include <chrono>
using namespace std::literals;

#if defined(_WIN32)
	#include <conio.h>
#elif defined(__linux__)
	int _kbhit();
	int _getch();
#endif

extern std::unique_ptr<test_tcp_echo_client> g_ptest_tcp_echo_client;
void print_title();
void print_endpoint(test_tcp_echo_client* _ptest_tcp_echo_client);
void print_statistics_info(test_tcp_echo_client* _ptest_tcp_echo_client, bool _update_now = false);
void print_setting_info(test_tcp_echo_client* _ptest_tcp_echo_client);

#if defined(_WIN32)
	# define ASCII_CODE_ENTER		0x0d
	# define ASCII_CODE_BACKSPACE	0x08
	# define ASCII_CODE_ESCAPE		0x1b
#elif defined(__linux__)
	# define ASCII_CODE_ENTER		0x0a
	# define ASCII_CODE_BACKSPACE	0x7f
	# define ASCII_CODE_ESCAPE		0x1b
#else
	# define ASCII_CODE_ENTER		0x0d
	# define ASCII_CODE_BACKSPACE	0x08
	# define ASCII_CODE_ESCAPE		0x1b
#endif

std::string get_cin_line()
{
	std::string str_input;
	int ch;

	while ((ch = _getch()) != ASCII_CODE_ENTER && str_input.size() < 256)
	{
		if (ch == ASCII_CODE_ESCAPE)
		{
			str_input.clear();
			break;
		}

		if (ch == ASCII_CODE_BACKSPACE)
		{
			if (str_input.empty())
				continue;

			str_input.pop_back();
			std::cout << "\b \b" << std::flush;

			continue;
		}

		if (!((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'A') || (ch >= '0' && ch <= '9') || ch == '.'))
		{
			continue;
		}

		str_input += static_cast<char>(ch);
		std::putchar(ch);
		std::cout << std::flush;
	}
	std::cout << std::endl;
	std::cout << std::flush;

	return str_input;
}

int main()
{
	// trace) hide cursor
	std::cout << "\x1b[?25l"sv;

	// 1) print title
	print_title();

	// 2) create tcp echo client
	g_ptest_tcp_echo_client = std::make_unique<test_tcp_echo_client>();

	// 3) start
	g_ptest_tcp_echo_client->start();

	// 4) ...
	print_endpoint(g_ptest_tcp_echo_client.get());

	// 4) loop
	for (;;)
	{
		// - output traffic info
		print_statistics_info(g_ptest_tcp_echo_client.get());

		// - key 눌렀나?
		if (_kbhit())
		{
			// - Key를 읽는다.
			int	ch = _getch();

			// - ESC키를 누르면 접속을 종료한다.
			if (ch == 27)
			{
				break;
			}

			switch (ch)
			{
			// connect
			case	'1':
					g_ptest_tcp_echo_client->request_connect(1);
					break;
			case	'2':
					g_ptest_tcp_echo_client->request_connect(10);
					break;
			case	'3':
					g_ptest_tcp_echo_client->request_connect(100);
					break;
			case	'4':
					g_ptest_tcp_echo_client->request_connect(1000);
					break;
			case	'5':
					g_ptest_tcp_echo_client->request_connect(10000);
					break;

			// closesocket
			case	'6':
					g_ptest_tcp_echo_client->request_closesocket(1);
					break;
			case	'7':
					g_ptest_tcp_echo_client->request_closesocket_all();
					break;

			// disconnect
			case	'8':
					g_ptest_tcp_echo_client->request_disconnect_socket(1);
					break;
			case	'9':
					g_ptest_tcp_echo_client->request_disconnect_socket_all();
					break;

			// connection test
			case	'Q':
			case	'q':
					g_ptest_tcp_echo_client->toggle_connect_test();
					break;

			case	'W':
			case	'w':
					g_ptest_tcp_echo_client->add_connect_test_min(100);
					break;

			case	'E':
			case	'e':
					g_ptest_tcp_echo_client->sub_connect_test_min(100);
					break;

			case	'R':
			case	'r':
					g_ptest_tcp_echo_client->add_connect_test_max(100);
					break;

			case	'T':
			case	't':
					g_ptest_tcp_echo_client->sub_connect_test_max(100);
					break;

			// traffic test
			case	' ':
					g_ptest_tcp_echo_client->toggle_traffic_test();
					break;

			case	'A':
			case	'a':
					g_ptest_tcp_echo_client->add_traffic_test_count_per_single(1);
					break;

			case	'Z':
			case	'z':
					g_ptest_tcp_echo_client->sub_traffic_test_count_per_single(1);
					break;

			case	'S':
			case	's':
					g_ptest_tcp_echo_client->add_traffic_test_count_per_single(10);
					break;

			case	'X':
			case	'x':
					g_ptest_tcp_echo_client->sub_traffic_test_count_per_single(10);
					break;

			case	'D':
			case	'd':
					g_ptest_tcp_echo_client->add_traffic_test_count_per_single(100);
					break;

			case	'C':
			case	'c':
					g_ptest_tcp_echo_client->sub_traffic_test_count_per_single(100);
					break;

			case	'F':
			case	'f':
					g_ptest_tcp_echo_client->add_traffic_test_count_per_single(1000);
					break;

			case	'V':
			case	'v':
					g_ptest_tcp_echo_client->sub_traffic_test_count_per_single(1000);
					break;

			case	'G':
			case	'g':
					g_ptest_tcp_echo_client->add_traffic_test_count_per_single(10000);
					break;

			case	'B':
			case	'b':
					g_ptest_tcp_echo_client->sub_traffic_test_count_per_single(10000);
					break;

			case	'H':
			case	'h':
					g_ptest_tcp_echo_client->add_traffic_test_count_per_single(100000);
					break;

			case	'N':
			case	'n':
					g_ptest_tcp_echo_client->sub_traffic_test_count_per_single(100000);
					break;

			case	'j':
					g_ptest_tcp_echo_client->increase_traffic_test_message_size();
					break;

			case	'm':
					g_ptest_tcp_echo_client->decrease_traffic_test_message_size();
					break;

			case	'U':
			case	'u':
					g_ptest_tcp_echo_client->request_send_immidiately(1);
					break;

			case	'I':
			case	'i':
					g_ptest_tcp_echo_client->request_send_immidiately(10);
					break;

			case	'O':
			case	'o':
					g_ptest_tcp_echo_client->request_send_immidiately(100);
					break;

			case	'P':
			case	'p':
					g_ptest_tcp_echo_client->request_send_immidiately(1000);
					break;

			case	'[':
					g_ptest_tcp_echo_client->request_send_immidiately(100000);
					break;

			case	']':
					g_ptest_tcp_echo_client->request_send_immidiately(1000000);
					break;

			case	'/':
					g_ptest_tcp_echo_client->toggle_relay_echo_test();
					break;

			case	';':
					g_ptest_tcp_echo_client->toggle_disconnect_on_response();
					break;

		#if defined(_WIN32)
			case	0xe0:
					{
						// - extended key (for windows)
						/*auto key = */_getch();
					}
					break;
		#endif

			case	ASCII_CODE_ENTER:
					{
						// - get address
						std::cout << "\x1b[3A";
						std::cout << "\x1b[2K";
						std::cout << "\x1b[90m address : \x1b[0m" << std::flush;
						auto str_address = get_cin_line();

						// - get port
						std::cout << "\x1b[2K";
						std::cout << "\x1b[90m port    : \x1b[0m" << std::flush;
						auto str_port = get_cin_line();

						// - set endpoint
						g_ptest_tcp_echo_client->set_endpoint(str_address, str_port);

						std::cout << "\x1b[3A";
						print_endpoint(g_ptest_tcp_echo_client.get());
					}
					break;

			case	ASCII_CODE_BACKSPACE:
					{
						std::cout << "\033c";
						print_title();
						print_endpoint(g_ptest_tcp_echo_client.get());
						print_statistics_info(g_ptest_tcp_echo_client.get(), true);
					}
					break;
			}

			// output setting info
			print_setting_info(g_ptest_tcp_echo_client.get());
		}

		// - sleep
		std::this_thread::sleep_for(100ms);
	}

	// 5) destroy
	g_ptest_tcp_echo_client.reset();

	// 6) 
	std::cout << "\x1b[7B\n tcp echo client closed... \n"sv;

	// trace) show cursor
	std::cout << "\x1b[?25h"sv;
}

void _print_setting_info(std::stringstream& _buffer_output, test_tcp_echo_client* _ptest_tcp_echo_client)
{
	// 1) move cursor
	_buffer_output << "\x1b[5B"sv;

	// definitions)
	static std::string_view str_index_to_string[] = { "8 byte"sv, "64 byte"sv, "256 byte"sv, "1K byte"sv, "4K byte"sv, "16K byte"sv, "64K byte"sv, "512K byte"sv, "4M byte"sv, "32M byte"sv, "256M byte"sv };

	// 2) ...
	_buffer_output << "\x1b[37m [con test]   \x1b[0m"sv;
	_buffer_output << ((_ptest_tcp_echo_client->m_enable_connect_test) ? "\x1b[47m\x1b[30m on \x1b[0m "sv : "\x1b[90m off\x1b[0m "sv);
	_buffer_output << "\x1b[90mrange min \x1b[0m"sv; _buffer_output << _ptest_tcp_echo_client->m_connect_test_min;
	_buffer_output << "\x1b[90m ~ max \x1b[0m"sv; _buffer_output << _ptest_tcp_echo_client->m_connect_test_max; _buffer_output << ", "sv;
	_buffer_output << "\x1b[90minterval \x1b[0m"sv << "0"sv << "\x1b[K\n"sv;

	_buffer_output << "\x1b[37m [echo test]  \x1b[0m"sv;
	_buffer_output << ((_ptest_tcp_echo_client->m_enable_traffic_test) ? "\x1b[47m\x1b[30m on \x1b[0m "sv : "\x1b[90m off\x1b[0m "sv);
	_buffer_output << "\x1b[90mmessage size \x1b[0m"sv; _buffer_output << str_index_to_string[_ptest_tcp_echo_client->m_traffic_test_selected]; _buffer_output << ",  "sv;
	_buffer_output << "\x1b[90mtimes \x1b[0m"sv; _buffer_output << _ptest_tcp_echo_client->m_traffic_test_count_per_single; _buffer_output << "\x1b[K\n"sv;

	_buffer_output << "\x1b[37m [relay echo] \x1b[0m"sv;
	_buffer_output << ((_ptest_tcp_echo_client->m_s_enable_relay_echo_test) ? "\x1b[47m\x1b[30m on \x1b[0m "sv : "\x1b[90m off\x1b[0m "sv); _buffer_output << "\x1b[K"sv;
	_buffer_output << "              \x1b[37m [dis on res] \x1b[0m"sv;
	_buffer_output << ((_ptest_tcp_echo_client->m_s_enable_disconnect_on_response) ? "\x1b[47m\x1b[30m on \x1b[0m "sv : "\x1b[90m off\x1b[0m "sv); _buffer_output << "\x1b[K\n"sv;

	// 3) move cursor
	_buffer_output << "\x1b[8A"sv;
}

void print_title()
{
	std::stringstream buf_output;

	buf_output << " ---------------------------------------------------\n"sv;
	buf_output << "\n"sv;
	buf_output << "\x1b[97m    boost.asio tcp echo client \x1b[0m\n"sv;
	buf_output << "\n"sv;
	buf_output << "\n"sv;
	buf_output << "  [command]\n"sv;
	buf_output << "\n"sv;
	buf_output << "   \x1b[90m- change remote endpoint\x1b[0m		'ENTER' key\n"sv;
	buf_output << "   \x1b[90m- connect session\x1b[0m		'1'(+1), '2'(+10), '3'(+100), '4'(+1000), '5'(+10000)\n"sv;
	buf_output << "   \x1b[90m- close session\x1b[0m		'6'(-1), '7'(all)\n"sv;
	buf_output << "   \x1b[90m- disconnect session\x1b[0m	'8'(-1), '9'(all)\n"sv;
	buf_output << "\n"sv;
	buf_output << "   connection test\n"sv;
	buf_output << "   \x1b[90m- start/stop\x1b[0m			'q'\n"sv;
	buf_output << "   \x1b[90m- increase min range\x1b[0m		'w'\n"sv;
	buf_output << "   \x1b[90m- decrease min range\x1b[0m		'e'\n"sv;
	buf_output << "   \x1b[90m- increase max range\x1b[0m		'r'\n"sv;
	buf_output << "   \x1b[90m- decrease max range\x1b[0m		't'\n"sv;
	buf_output << "\n"sv;
	buf_output << "   traffic test\n"sv;
	buf_output << "   \x1b[90m- start/stop\x1b[0m			space key\n"sv;
	buf_output << "   \x1b[90m- relay echo on/off\x1b[0m		'/'\n"sv;
	buf_output << "   \x1b[90m- disconnect on response\x1b[0m	';'\n"sv;
	buf_output << "   \x1b[90m- increase traffic\x1b[0m		'a'(+1), 's'(+10), 'd'(+100), 'f'(+1000), 'g'(+10000), 'h'(+100000)\n"sv;
	buf_output << "   \x1b[90m- decrease traffic\x1b[0m		'z'(-1), 'x'(-10), 'c'(-100), 'v'(-1000), 'b'(-10000), 'n'(-100000)\n"sv;
	buf_output << "   \x1b[90m- change message size\x1b[0m	'm'(+), 'j'(-)\n"sv;
	buf_output << "   \x1b[90m- single send\x1b[0m		'u'(1), 'i'(10), 'o'(100), 'p'(1000), '['(100000), ']'(1000000)\n"sv;
	buf_output << "\n"sv;
	buf_output << " ---------------------------------------------------\n"sv;
	buf_output << "\n"sv;
	fwrite(buf_output.str().c_str(), 1, buf_output.str().size(), stdout);
}

void print_setting_info(test_tcp_echo_client* _ptest_tcp_echo_client)
{
	// 1) alloc buffer
	std::stringstream buf_output;

	// 2) text
	_print_setting_info(buf_output, _ptest_tcp_echo_client);

	// 3) get str
	const auto& str_output = buf_output.str();

	// 4) write
	fwrite(str_output.c_str(), 1, str_output.size(), stdout);
}

void print_endpoint(test_tcp_echo_client* _ptest_tcp_echo_client)
{
	const auto& end_point = _ptest_tcp_echo_client->get_endpoint();

	std::cout << " [remote endpoint]\n";
	std::cout << "\x1b[90m address : \x1b[0m" << end_point.address().to_string() << "\x1b[K\n";
	std::cout << "\x1b[90m port    : \x1b[0m" << end_point.port() << "\x1b[K\n\n";
}

std::string to_string_dot_seperated(uint64_t _value)
{
	// check)
	if (_value == 0)
		return std::string("0");

	char result[128];
	int pos = 127;
	result[pos--] = 0;
	while (_value)
	{
		if (!((pos-3) % 4))
			result[pos--] = ',';
		result[pos--] = (_value % 10) + '0';
		_value /= 10;
	}

	return std::string(result + pos + 1);
}

std::string to_string_scaled(uint64_t _value)
{
	//constexpr uint64_t SCALE_1 = 0x0000'0000'0000'03ff;
	//constexpr uint64_t SCALE_2 = 0x0000'0000'000f'fc00;
	constexpr uint64_t SCALE_3 = 0x0000'0000'3ff0'0000;
	constexpr uint64_t SCALE_4 = 0x0000'00ff'c000'0000;
	constexpr uint64_t SCALE_5 = 0x0003'ff00'0000'0000;
	constexpr uint64_t SCALE_6 = 0xfffc'0000'0000'0000;

	if (_value & SCALE_6)
		return to_string_dot_seperated(_value >> 30) + "G";
	else if (_value & SCALE_5)
		return to_string_dot_seperated(_value >> 30) + "G";
	else if (_value & SCALE_4)
		return to_string_dot_seperated(_value >> 20) + "M";
	else if (_value & SCALE_3)
		return to_string_dot_seperated(_value >> 10) + "K";
	else
		return to_string_dot_seperated(_value);
}

std::string to_string_scaled(float _value)
{
	//constexpr uint64_t SCALE_1 = 0x0000'0000'0000'03ff;
	//constexpr uint64_t SCALE_2 = 0x0000'0000'000f'fc00;
	constexpr uint64_t SCALE_3 = 0x0000'0000'3ff0'0000;
	constexpr uint64_t SCALE_4 = 0x0000'00ff'c000'0000;
	constexpr uint64_t SCALE_5 = 0x0003'ff00'0000'0000;
	constexpr uint64_t SCALE_6 = 0xfffc'0000'0000'0000;

	uint64_t value = static_cast<uint64_t>(_value);

	if (value & SCALE_6)
		return to_string_dot_seperated(value >> 30) + "G";
	else if (value & SCALE_5)
		return to_string_dot_seperated(value >> 30) + "G";
	else if (value & SCALE_4)
		return to_string_dot_seperated(value >> 20) + "M";
	else if (value & SCALE_3)
		return to_string_dot_seperated(value >> 10) + "K";
	else
		return to_string_dot_seperated(value);
}

void print_statistics_info(test_tcp_echo_client* _ptest_tcp_echo_client, bool _update_now)
{
	// 1) 현재 Tick과 Tick Gap을 구한다.
	static auto tick_last = std::chrono::steady_clock::time_point(std::chrono::steady_clock::duration::zero());
	auto tick_now = std::chrono::steady_clock::now();

	// 2) get tick differ
	auto tick_differ = std::chrono::duration_cast<std::chrono::milliseconds>(tick_now - tick_last);

	// check) tickdiffer가 1s이하면 그냥 끝낸다.
	if(_update_now == false && tick_differ < 1s)
		return;

	// 3) 최종 시간을 Update한다.
	tick_last = tick_now;

	// 4) alloc buffer
	std::stringstream buf_output;

	// 5) connection info
	{
		// declare) 
		static uint64_t	last_count_try = 0;
		static uint64_t	last_count_success = 0;
		static uint64_t	last_count_disconnected = 0;

		// - 이전과 현재의 차이를 구한다. (Overflow가 났을 경우를 대비하여 계산해야 한다.)
		auto now_count_keep = asio::Nstatistics::statistics_connect_keep.load();
		auto now_count_try = asio::Nstatistics::statistics_connect_try.load();
		auto now_count_success = asio::Nstatistics::statistics_connect_success.load();
		auto now_count_fail = asio::Nstatistics::statistics_connect_try.load() - asio::Nstatistics::statistics_connect_success.load();
		auto now_count_disconnected = asio::Nstatistics::statistics_connect_disconnect.load();

		// - 초당 전송률을 구한다.
		auto total_try_per_sec = ((float)(now_count_try - last_count_try)) * 1000.0f / tick_differ.count();
		auto total_success_per_sec = ((float)(now_count_success - last_count_success)) * 1000.0f / tick_differ.count();
		auto total_disconnected_per_sec = ((float)(now_count_disconnected - last_count_disconnected)) * 1000.0f / tick_differ.count();

		// - 이전 값을 현재 값으로 Update한다.(다음 Frame에서의 계산을 위해)
		last_count_try = now_count_try;
		last_count_success = now_count_success;
		last_count_disconnected = now_count_disconnected;

		// - sended
		buf_output << " [connection]";
		buf_output << "\x1b[90m now \x1b[0m"; buf_output << now_count_keep;
		buf_output << "\x1b[90m   try \x1b[0m"; buf_output << now_count_try;
		buf_output << "\x1b[90m   success \x1b[0m"; buf_output << now_count_success;
		buf_output << "\x1b[90m   fail \x1b[0m"; buf_output << now_count_fail;
		buf_output << "\x1b[90m   dis \x1b[0m"; buf_output << now_count_disconnected;
		buf_output << "\x1b[90m   try/s \x1b[0m"; buf_output << total_try_per_sec;
		buf_output << "\x1b[90m   success/s \x1b[0m"; buf_output << total_success_per_sec;
		buf_output << "\x1b[90m   dis/s \x1b[0m"; buf_output << total_disconnected_per_sec; buf_output << "\x1b[K\n\n";
	}

	// 5) traffic info
	{
		// declare) 
		static uint64_t	last_count_sended = 0;
		static uint64_t	last_count_received = 0;
		static uint64_t	last_count_sendedByte = 0;
		static uint64_t	last_count_received_byte = 0;

		// - 이전과 현재의 차이를 구한다. (Overflow가 났을 경우를 대비하여 계산해야 한다.)
		auto now_count_sended = asio::Nstatistics::statistics_send_messages.load();
		auto now_count_sendedByte = asio::Nstatistics::statistics_send_bytes.load();
		auto now_count_send_failed = asio::Nstatistics::statistics_send_error.load();
		auto now_count_received = asio::Nstatistics::statistics_receive_messages.load();
		auto now_count_received_byte = asio::Nstatistics::statistics_receive_bytes.load();

		// - 초당 전송률을 구한다.
		auto total_sended_message_per_sec = ((float)(now_count_sended - last_count_sended)) * 1000.0f / tick_differ.count();
		auto total_sended_byte_per_sec = ((float)(now_count_sendedByte - last_count_sendedByte)) * 1000.0f / tick_differ.count();
		auto total_receive_message_per_sec = ((float)(now_count_received - last_count_received)) * 1000.0f / tick_differ.count();
		auto total_receive_byte_per_sec = ((float)(now_count_received_byte - last_count_received_byte)) * 1000.0f / tick_differ.count();

		// - 이전 값을 현재 값으로 Update한다.(다음 Frame에서의 계산을 위해)
		last_count_sended = now_count_sended;
		last_count_sendedByte = now_count_sendedByte;
		last_count_received = now_count_received;
		last_count_received_byte = now_count_received_byte;

		// - sended
		buf_output << " [send]    ";
	#if defined(_WIN32)
		buf_output << "\x1b[90m   messages \x1b[0m"sv; buf_output << std::format("{:>12s}"sv, to_string_scaled(now_count_sended));
		buf_output << "\x1b[90m   messages/s \x1b[0m"sv; buf_output << std::format("{:>12s}"sv, to_string_scaled(total_sended_message_per_sec));
		buf_output << "\x1b[90m   bytes/s \x1b[0m"sv << std::format("{:>12s}"sv, to_string_scaled(static_cast<uint64_t>(total_sended_byte_per_sec)));
	#else
		char temp_string[256] = { 0, };
		sprintf(temp_string, "%-12s", to_string_scaled(now_count_sended).c_str());
		buf_output << "\x1b[90m   messages \x1b[0m"sv << temp_string;
		sprintf(temp_string, "%-12s", to_string_scaled(total_sended_message_per_sec).c_str());
		buf_output << "\x1b[90m   messages/s \x1b[0m"sv << temp_string;
		sprintf(temp_string, "%-12s", to_string_scaled(static_cast<uint64_t>(total_sended_byte_per_sec).c_str());
		buf_output << "\x1b[90m   messages/s \x1b[0m"sv << temp_string;
	#endif
		buf_output << "\x1b[90m   failed \x1b[0m"sv << now_count_send_failed << "\x1b[K\n"sv;

		// - received
		buf_output << " [receive] ";
	#if defined(_WIN32)
		buf_output << "\x1b[90m   messages \x1b[0m"sv; buf_output << std::format("{:>12s}", to_string_scaled(now_count_received));
		buf_output << "\x1b[90m   messages/s \x1b[0m"sv; buf_output << std::format("{:>12s}", to_string_scaled(total_receive_message_per_sec));
		buf_output << "\x1b[90m   bytes/s \x1b[0m"sv << std::format("{:>12s}", to_string_scaled(static_cast<uint64_t>(total_receive_byte_per_sec))) << "\x1b[K\n"sv;
	#else
		sprintf(temp_string, "%-12s", to_string_scaled(now_count_received).c_str());
		buf_output << "\x1b[90m   messages \x1b[0m"sv << temp_string;
		sprintf(temp_string, "%-12s", to_string_scaled(total_receive_message_per_sec).c_str());
		buf_output << "\x1b[90m   messages/s \x1b[0m"sv << temp_string;
		sprintf(temp_string, "%-12s", to_string_scaled(static_cast<uint64_t>(total_receive_byte_per_sec)).c_str());
		buf_output << "\x1b[90m   messages/s \x1b[0m"sv << temp_string;

	#endif
		buf_output << "\n";
	}

	// 6) move cursor
	buf_output << "\x1b[5A";

	// 7) setting info
	_print_setting_info(buf_output, _ptest_tcp_echo_client);

	// 8) get str
	const auto& str_output = buf_output.str();

	// 9) write
	fwrite(str_output.c_str(), 1, str_output.size(), stdout);
}

