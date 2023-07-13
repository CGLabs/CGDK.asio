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

class asio::Nacceptor : public Nconnective
{
public:
			Nacceptor();
	virtual ~Nacceptor() noexcept;

			void start(boost::asio::ip::tcp::endpoint _endpoint);
			void close() noexcept;

private:
	virtual std::shared_ptr<Isocket_tcp> process_create_socket() = 0;
			void process_request_accept();
			void process_accept();

			struct instance
			{
				std::shared_ptr<Nacceptor> pacceptor;
				std::shared_ptr<tcp::acceptor> accept_context;
				std::shared_ptr<Isocket_tcp> psocket;
				void process_accept();
			};
			friend struct instance;

			void process_instance_register(std::unique_ptr<instance>&& _instance);
			void process_instance_unregister(instance* _instance) noexcept;

			std::recursive_mutex m_lockable_acceptor;
			std::shared_ptr<tcp::acceptor> m_accept_context;
			std::vector<std::unique_ptr<instance>> m_list_instance;
};