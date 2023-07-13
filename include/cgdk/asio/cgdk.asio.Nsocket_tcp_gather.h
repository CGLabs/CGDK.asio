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

class asio::Nsocket_tcp_gather : public Nsocket_tcp
{
public:
			Nsocket_tcp_gather();
	virtual ~Nsocket_tcp_gather() noexcept;

protected:
	virtual void process_closesocket(boost::system::error_code _error_code) noexcept override;
	virtual bool process_send(SEND_NODE&& _send_node) override;
			void process_send_async(const SEND_NODE& _send_node);

			std::list<SEND_NODE> m_send_msgs;
			std::shared_ptr<Isocket_tcp> m_hold_send;
};
