﻿//*****************************************************************************
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

class CGDK::asio::Nsocket_tcp_async_gather : public Nsocket_tcp_async
{
public:
			Nsocket_tcp_async_gather();
	virtual ~Nsocket_tcp_async_gather() noexcept;

protected:
	virtual void process_closesocket(boost::system::error_code _error_code) noexcept override;
	virtual bool process_send(SEND_NODE&& _send_node) override;
			void process_send_gather_async(const SEND_NODE& _send_node);

			SEND_NODE m_sending;
};
