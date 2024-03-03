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

class CGDK::asio::Nsocket_tcp_async : public Nsocket_tcp
{
public:
			Nsocket_tcp_async();
	virtual ~Nsocket_tcp_async() noexcept;

protected:
	virtual void process_closesocket(boost::system::error_code _error_code) noexcept override;
	virtual bool process_send(SEND_NODE&& _send_node) override;
			void process_send_async(const SEND_NODE& _send_node);

			std::list<SEND_NODE> m_send_msgs;
			std::shared_ptr<Isocket_tcp> m_hold_send;
};
