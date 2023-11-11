//*****************************************************************************
//*                                                                           *
//*                      Cho sanghyun's Game Classes II                       *
//*                                                                           *
//*                           asio network classes                            *
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

class CGDK::asio::system
{
public:
	~system() noexcept;

	[[nodiscard]] static boost::asio::io_service& get_io_service() { return get_instance()->io_service; }
	[[nodiscard]] static std::shared_ptr<asio::system> get_instance() { if (!pinstance) { return init_instance(); }; return pinstance; }
	static std::shared_ptr<asio::system> init_instance(int _thread_count = -1);
	static void destroy_instance() noexcept;
	static void run_executor();

protected:
	void process_prepare_thread(int _thread_count);
	void process_run_executor();
	void process_destroy();
	boost::asio::io_service io_service;
	std::vector<std::shared_ptr<std::thread>> m_vector_threads;
	bool m_is_thread_run = false;

	static std::mutex lock_instance;
	static std::shared_ptr<asio::system> pinstance;
};


