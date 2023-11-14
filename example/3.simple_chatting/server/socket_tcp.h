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
class socket_tcp : public asio::Nsocket_tcp, public asio::Nconnect_requestable
{
public:
	virtual void on_connect() override;
	virtual void on_disconnect(boost::system::error_code _error_code) noexcept override;
	virtual int on_message(shared_buffer& _msg) override;
};
