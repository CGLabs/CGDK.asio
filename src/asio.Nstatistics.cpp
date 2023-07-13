#include "asio.h"


std::atomic<uint64_t> asio::Nstatistics::statistics_connect_keep{ 0 };
std::atomic<uint64_t> asio::Nstatistics::statistics_connect_try{ 0 };
std::atomic<uint64_t> asio::Nstatistics::statistics_connect_success{ 0 };
std::atomic<uint64_t> asio::Nstatistics::statistics_connect_disconnect{ 0 };
std::atomic<uint64_t> asio::Nstatistics::statistics_send_messages{ 0 };
std::atomic<uint64_t> asio::Nstatistics::statistics_send_bytes{ 0 };
std::atomic<uint64_t> asio::Nstatistics::statistics_send_error{ 0 };
std::atomic<uint64_t> asio::Nstatistics::statistics_receive_messages{ 0 };
std::atomic<uint64_t> asio::Nstatistics::statistics_receive_bytes{ 0 };
