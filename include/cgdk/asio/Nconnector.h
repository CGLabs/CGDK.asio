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

class CGDK::asio::Nconnector : public Nconnective
{
public:
			Nconnector();
	virtual ~Nconnector() noexcept;

			void start(const boost::asio::any_io_executor& _executor);
			void start();
			void close() noexcept;
			void request_connect(boost::asio::ip::tcp::endpoint _endpoint_connect);
			void request_connect(std::shared_ptr<Isocket_tcp> _socket_new, boost::asio::ip::tcp::endpoint _endpoint_connect);
	virtual std::shared_ptr<Isocket_tcp> process_create_socket() = 0;
			void process_connect_completion(std::shared_ptr<Isocket_tcp> _socket, const boost::system::error_code& _error);

private:
			boost::asio::any_io_executor m_executor;
};
