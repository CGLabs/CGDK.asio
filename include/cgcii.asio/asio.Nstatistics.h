//*****************************************************************************
//*                                                                           *
//*                      Cho sanghyun's Game Classes II                       *
//*                       Ver 10.0 / Release 2019.12.11                       *
//*                                                                           *
//*                           asio network classes                            *
//*                                                                           *
//*                                                                           *
//*                                                                           *
//*                                                                           *
//*  This Program is programmed by Cho SangHyun. sangduck@cgcii.co.kr         *
//*  Best for Game Developement and Optimized for Game Developement.          *
//*                                                                           *
//*                (c) 2003. Cho Sanghyun. All right reserved.                *
//*                          http://www.CGCII.co.kr                           *
//*                                                                           *
//*****************************************************************************
#pragma once

class asio::Nstatistics
{
public:
	static	std::atomic<uint64_t> statistics_connect_keep;
	static	std::atomic<uint64_t> statistics_connect_try;
	static	std::atomic<uint64_t> statistics_connect_success;
	static	std::atomic<uint64_t> statistics_connect_disconnect;
	static	std::atomic<uint64_t> statistics_send_messages;
	static	std::atomic<uint64_t> statistics_send_bytes;
	static	std::atomic<uint64_t> statistics_send_error;
	static	std::atomic<uint64_t> statistics_receive_messages;
	static	std::atomic<uint64_t> statistics_receive_bytes;
};