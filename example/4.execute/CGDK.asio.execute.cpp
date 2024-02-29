//*****************************************************************************
//*                                                                           *
//*                      Cho sanghyun's Game Classes II                       *
//*                       Ver 10.0 / Release 2019.12.11                       *
//*                                                                           *
//*                     asio.example.simle_chatting.server                    *
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

#include "cgdk/asio.h"
#include <iostream>

int main()
{
	// 1) post함수
	asio::system::post([]() { std::cout << "this is posted\n"; });

	// 2) dispatch 함수
	asio::system::dispatch([]() { std::cout << "this is dispatched\n"; });
}
