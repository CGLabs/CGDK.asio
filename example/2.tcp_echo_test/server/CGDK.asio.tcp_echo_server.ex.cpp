#include "cgdk/asio.h"
#include <iostream>
#include <sstream>

using namespace std::literals;
extern bool g_disable_echo;

shared_buffer make_message(size_t _size)
{
	// check) 
	assert(_size % 4 == 0);

	// - alloc buffer
	auto buf_temp = alloc_shared_buffer(_size);

	// - make message
	auto buf_append = buf_temp;
	buf_append.append<uint32_t>(static_cast<uint32_t>(_size));
	buf_append.append<uint32_t>();
	for (size_t size = 8; size < _size; size += sizeof(uint32_t))
	{
		buf_append.append<uint32_t>(0);
	}

	return buf_temp;
}

void print_title()
{
	// 1) initialization
	std::ostringstream buf_output;
	buf_output.str().reserve(8192);
	buf_output << " ---------------------------------------------------\n"sv;
	buf_output << "\n"sv;
	buf_output << "     boost.asio tcp echo server.ex\n"sv;
	buf_output << "\n"sv;
	buf_output << "\n"sv;
	buf_output << "  [command]\n"sv;
	buf_output << "\n"sv;
	buf_output << "   \x1b[90m- close session\x1b[0m		'6'(-1), '7'(all)\n"sv;
	buf_output << "   \x1b[90m- disconnect session\x1b[0m		'8'(-1), '9'(all)\n"sv;
	buf_output << "   \x1b[90m- single send\x1b[0m		'u'(8byte), 'i'(1kbyte), 'o'(64kbyte)\n"sv;
	buf_output << "\n"sv;
	buf_output << "   echo test\n"sv;
	buf_output << "   \x1b[90m- echo on/off\x1b[0m		'a'\n"sv;
	buf_output << "\n"sv;
	buf_output << " ---------------------------------------------------\n"sv;
	buf_output << "\n"sv;
	std::cout << buf_output.str();
}

void _print_setting_info(std::ostringstream& _buffer_output)
{
	// 1) move cursor
	_buffer_output << "\x1b[5B"sv;

	// 2) ...
	_buffer_output << "\x1b[37m [echo test]   \x1b[0m"sv;
	_buffer_output << "\x1b[90mecho \x1b[0m"sv << ((g_disable_echo == false) ? "\x1b[47m\x1b[30m on \x1b[0m"sv : "\x1b[90m off\x1b[0m"sv);
	_buffer_output << "\x1b[K\n"sv;

	// 3) move cursor
	_buffer_output << "\x1b[6A"sv;
}

void print_setting_info()
{
	// 1) alloc buffer
	std::ostringstream buf_output;
	buf_output.str().reserve(8192);

	// 2) text
	_print_setting_info(buf_output);

	// 3) write
	std::cout << buf_output.str();
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
		if (!((pos - 3) % 4))
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

void print_statistics_info()
{
	// 1) 현재 Tick과 Tick Gap을 구한다.
	static auto tick_last = std::chrono::steady_clock::time_point(std::chrono::steady_clock::duration::zero());
	auto tick_now = std::chrono::steady_clock::now();
	auto tick_differ = std::chrono::duration_cast<std::chrono::milliseconds>(tick_now - tick_last);

	// check) tickdiffer가 10ms이하면 그냥 끝낸다.
	if (tick_differ < 1s)
		return;

	// 2) 최종 시간을 Update한다.
	tick_last = tick_now;

	// 3) alloc buffer
	std::ostringstream buf_output;
	buf_output.str().reserve(8192);

	// 4) connection info
	{
		// declare) 
		static uint64_t	last_count_try = 0;
		static uint64_t	last_count_success = 0;
		static uint64_t	last_count_disconnected = 0;

		// - 이전과 현재의 차이를 구한다. (Overflow가 났을 경우를 대비하여 계산해야 한다.)
		auto now_count_keep = asio::Nstatistics::statistics_connect_keep.load();
		auto now_count_try = asio::Nstatistics::statistics_connect_try.load();
		auto now_count_success = asio::Nstatistics::statistics_connect_success.load();
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
		buf_output << " [connection]"sv;
		buf_output << "\x1b[1m now \x1b[0m"sv << now_count_keep;
		buf_output << "\x1b[1m   try \x1b[0m"sv << now_count_try;
		buf_output << "\x1b[1m   try/s \x1b[0m"sv << total_try_per_sec;
		buf_output << "\x1b[1m   success/s \x1b[0m"sv << total_success_per_sec;
		buf_output << "\x1b[1m   dis/s \x1b[0m"sv << total_disconnected_per_sec << "\x1b[K\n\n"sv;
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
	#else
		char temp_string[256] = { 0, };
		sprintf(temp_string, "%-12s", to_string_scaled(now_count_sended).c_str());
		buf_output << "\x1b[90m   messages \x1b[0m"sv << temp_string;
		sprintf(temp_string, "%-12s", to_string_scaled(total_sended_message_per_sec).c_str());
		buf_output << "\x1b[90m   messages/s \x1b[0m"sv << temp_string;
	#endif
		buf_output << "\x1b[90m   bytes/s \x1b[0m"sv << std::format("{:>12s}"sv, to_string_scaled(static_cast<uint64_t>(total_sended_byte_per_sec)));
		buf_output << "\x1b[90m   failed \x1b[0m"sv << now_count_send_failed << "\x1b[K\n"sv;

		// - received
		buf_output << " [receive] ";
	#if defined(_WIN32)
		buf_output << "\x1b[90m   messages \x1b[0m"sv; buf_output << std::format("{:>12s}", to_string_scaled(now_count_received));
		buf_output << "\x1b[90m   messages/s \x1b[0m"sv; buf_output << std::format("{:>12s}", to_string_scaled(total_receive_message_per_sec));
	#else
		sprintf(temp_string, "%-12s", to_string_scaled(now_count_received).c_str());
		buf_output << "\x1b[90m   messages \x1b[0m"sv << temp_string;
		sprintf(temp_string, "%-12s", to_string_scaled(total_receive_message_per_sec).c_str());
		buf_output << "\x1b[90m   messages/s \x1b[0m"sv << temp_string;
	#endif
		buf_output << "\x1b[90m   bytes/s \x1b[0m"sv << std::format("{:>12s}", to_string_scaled(static_cast<uint64_t>(total_receive_byte_per_sec))) << "\x1b[K\n"sv;
		buf_output << "\n";
	}

	// 6) move cursor
	buf_output << "\x1b[5A"sv;

	// 7) setting info
	_print_setting_info(buf_output);

	// 8) output
	std::cout << buf_output.str();
}
