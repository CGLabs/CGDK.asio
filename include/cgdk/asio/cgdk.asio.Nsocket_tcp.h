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

class asio::Nsocket_tcp : virtual public Isocket_tcp
{
public:
			Nsocket_tcp();
	virtual ~Nsocket_tcp() noexcept;

protected:
	virtual void on_connect() {}
	virtual void on_disconnect(boost::system::error_code /*_error_code*/) noexcept {}
	virtual int on_message(shared_buffer& /*_msg*/) { return 0; }

	virtual void process_connect_complete() override;
	virtual void process_closesocket(boost::system::error_code _error_code) noexcept override;
	virtual bool process_send(SEND_NODE&& _send_node) override;
			bool process_send_sync(SEND_NODE&& _send_node);
			void process_receive_async();

			shared_buffer m_received_msg;
			boost::asio::mutable_buffer m_receiving_msg;
			std::shared_ptr<Isocket_tcp> m_hold_async;
			shared_buffer::object_ptr_t m_hold_receiving;
};
