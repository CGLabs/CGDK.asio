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

namespace CGDK
{
	enum class ESOCKET_STATUE : int
	{
		NONE = 0,
		SYN = 1,
		CLOSING = 2,
		ESTABLISHED = 3,
	};

	constexpr size_t MIN_MESSAGE_BUFFER_ROOM = 256; // receive 버퍼의 최소 크기.
	constexpr size_t MIN_MESSAGE_SIZE = 4; // 메시지의 최대 크기(오류나 해킹으로 인해 너무 큰 메시지가 들어오면 문제가 될 수 있으므로 크기 제한을 해줍니다..)
	constexpr size_t MAX_MESSAGE_SIZE = 1024 * 1024; // 메시지의 최대 크기(오류나 해킹으로 인해 너무 큰 메시지가 들어오면 문제가 될 수 있으므로 크기 제한을 해줍니다..)

}