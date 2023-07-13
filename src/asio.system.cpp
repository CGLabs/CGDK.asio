#include "asio.h"
#if defined(__linux__)
#include <sys/resource.h>
#endif

std::mutex asio::system::lock_instance;
std::shared_ptr<asio::system> asio::system::pinstance;


std::shared_ptr<asio::system> asio::system::init_instance(int _thread_count)
{
	// declare) 
	std::shared_ptr<asio::system> temp_instance;

	{
		// lock) 
		std::lock_guard cs(lock_instance);

		// - create instance
		temp_instance = std::make_shared<asio::system>();

		// - prepare thread
		temp_instance->process_prepare_thread(_thread_count);

		// - set instance
		pinstance = temp_instance;
	}

	// return) 
	return temp_instance;
}

void asio::system::destroy_instance() noexcept
{
	get_instance()->process_destroy();
}

asio::system::~system() noexcept
{
	// 1) destroy io_service
	this->process_destroy();

	// declare)
	std::vector<std::shared_ptr<std::thread>> vector_threads = std::move(this->m_vector_threads);

	// 2) wait terminal of all threads
	for (auto& iter : vector_threads)
	{
		iter->join();
	}
}

void asio::system::run_executor()
{
	get_instance()->io_service.run_one();
}

void asio::system::process_run_executor()
{
	// declare) 
	boost::system::error_code ec;

	for (;;)
	{
		// - run
		this->io_service.run(ec);

		if (this->m_is_thread_run)
			this->io_service.restart();
		else
			break;
	}
}

#if defined(__linux__)
void _expanding_max_open_files() noexcept
{
	// declare) 
	rlimit64 lim_set;

	// 1) get hard limit of open files count
	getrlimit64(RLIMIT_NOFILE, &lim_set);

	// 2) set max
	lim_set.rlim_cur = lim_set.rlim_max;

	// 3) set!
	setrlimit64(RLIMIT_NOFILE, &lim_set);
}
#endif

void asio::system::process_prepare_thread(int _thread_count)
{
	// check)
	assert(this->m_is_thread_run == false);

	// 1) ..
		// - get thread count
	if (_thread_count < 0)
	{
		_thread_count = std::thread::hardware_concurrency() * 2;
	}

	// check)
	assert(_thread_count >= 0);

#if defined(__linux__)
	// 2) max open file 
	_expanding_max_open_files();
#endif

	// 3) thread run flag 'true'
	this->m_is_thread_run = true;

	// 4) prepare thread
	{
		// check) 
		std::vector<std::shared_ptr<std::thread>> vector_threads;

		// - reservce
		vector_threads.reserve(_thread_count);

		// - create threads
		for (; _thread_count > 0; --_thread_count)
		{
			auto t = std::make_shared<std::thread>([=, this]() { this->process_run_executor(); });

			vector_threads.push_back(t);
		}

		// - stroe thread objects
		this->m_vector_threads = std::move(vector_threads);
	}
}

void asio::system::process_destroy()
{
	// 1) thread run flag 'false'
	this->m_is_thread_run = false;

	// 2) stop
	this->io_service.stop();
}
