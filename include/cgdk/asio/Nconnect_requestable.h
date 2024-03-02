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

class CGDK::asio::Nconnect_requestable : virtual public Isocket_tcp
{
public:
			Nconnect_requestable() {}
	virtual ~Nconnect_requestable() noexcept {}
public:
	void start(boost::asio::ip::tcp::endpoint _endpoint_connect);

protected:
	void process_connect_request_complete(const boost::system::error_code& _error);
};
