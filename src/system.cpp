#include "cgdk/asio.h"
#if defined(__linux__)
	#include <sys/resource.h>
#endif

std::mutex CGDK::asio::system::lock_instance;
std::shared_ptr<CGDK::asio::system> CGDK::asio::system::pinstance;

CGDK::asio::system::~system() noexcept
{
	this->process_destroy();
}

bool CGDK::asio::system::register_schedulable(std::shared_ptr<Ischedulable>&& _pschedulable)
{
	// check)
	assert(_pschedulable);

	// check) executor가 nullptr인지 확인한다.
	if(_pschedulable == nullptr)
		return false;

	// 1) scheduler를 얻는다.
	auto pschedulable_manager = get_instance()->m_pschedulable_manager;

	// check) scheduler가 nullptr이면 안됀다.
	assert(pschedulable_manager);

	// check) scheduler가 없으면 Exception을 던진다.
	if(!pschedulable_manager)
		throw std::exception();

	// return) scheduler에 붙인다.
	return pschedulable_manager->register_schedulable(std::move(_pschedulable));
}

bool CGDK::asio::system::unregister_schedulable(Ischedulable* _pschedulable) noexcept
{
	// check)
	assert(_pschedulable);

	// check) executor가 nullptr인지 확인한다.
	if(_pschedulable == nullptr)
		return false;

	// 1) scheduler를 얻는다.
	auto pschedulable_manager = get_instance()->m_pschedulable_manager;

	// check) scheduler가 없으면 여기서 끝낸다.
	if(!pschedulable_manager)
		return false;

	// 2) Schedulable Manager에서 Schedulable을 제거한다.
	return pschedulable_manager->unregister_schedulable(_pschedulable);
}

void CGDK::asio::system::run_executor()
{
	get_instance()->m_io_service.run_one();
}

std::shared_ptr<CGDK::asio::system> CGDK::asio::system::process_initialize_instance(int _thread_count)
{
	// declare) 
	std::shared_ptr<asio::system> temp_instance;

	{
		// lock) 
		std::lock_guard cs(lock_instance);

		// 1) create instance
		temp_instance = std::make_shared<asio::system>();

		// 2) initialize
		temp_instance->process_initialize(_thread_count);

		// 4) set instance
		pinstance = temp_instance;
	}

	// return) 
	return temp_instance;
}

void CGDK::asio::system::process_initialize(int _thread_count)
{
	// check)
	assert(this->m_is_thread_run == false);

	// check)
	if (this->m_is_thread_run == true)
		return;

	// 1) thread run flag 'true'
	this->m_is_thread_run = true;

	try
	{
		// 2) prepare thread
		this->process_initialize_thread(_thread_count);

		// 3) initialize singe_execuor 
		this->process_initialize_single_executor();

		// 4) initialize scheduler
		this->process_initialize_scheduler();
	}
	catch (...)
	{
		// - rollback
		this->process_destroy();
	}
}

void CGDK::asio::system::process_destroy() noexcept
{
	// check)
	if (this->m_is_thread_run == false)
		return;

	// 1) thread run flag 'false'
	this->m_is_thread_run = false;

	// 2) destroy scheduler & single executor
	this->process_destroy_scheduler();
	this->process_destroy_single_executor();

	// 3) stop io_service
	this->m_io_service.stop();

	// 4) close threads
	this->process_destroy_thread();

}

void CGDK::asio::system::process_run_executor()
{
	// declare) 
	boost::system::error_code ec;

	// 1) work quard
	auto work_guard = boost::asio::make_work_guard(this->m_io_service);

	for (;;)
	{
		// 2) run
		this->m_io_service.run(ec);

		// 3) restart
		if (this->m_is_thread_run)
			this->m_io_service.restart();
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

void CGDK::asio::system::process_initialize_thread(int _thread_count)
{
	// check)
	assert(this->m_is_thread_run == true);

	// 1) get thread count
	if (_thread_count < 0)
		_thread_count = static_cast<int>(std::thread::hardware_concurrency()) * 2;

	// check)
	assert(_thread_count >= 0);

#if defined(__linux__)
	// 2) max open file 
	_expanding_max_open_files();
#endif

	// 4) prepare thread
	{
		// check) 
		std::vector<std::shared_ptr<std::thread>> vector_threads;

		// - reservce
		vector_threads.reserve(_thread_count);

		// - create threads
		for (; _thread_count > 0; --_thread_count)
		{
			auto t = std::make_shared<std::thread>([this]() { this->process_run_executor(); });

			vector_threads.push_back(t);
		}

		// - stroe thread objects
		this->m_vector_threads = std::move(vector_threads);
	}
}

void CGDK::asio::system::process_destroy_thread() noexcept
{
	// check)
	assert(this->m_is_thread_run == false);

	// declare)
	const auto vector_threads = std::move(this->m_vector_threads);

	// 2) wait terminal of all threads
	for (const auto& it : vector_threads)
	{
		it->join();
	}
}

void CGDK::asio::system::process_initialize_scheduler()
{
	// check)
	assert(this->m_is_thread_run == true);

	// 1) create scheduler
	auto pschedulable_manager = std::make_shared<schedulable_manager>();

	// 2) start
	pschedulable_manager->process_initialize();

	// 3) set
	this->m_pschedulable_manager = std::move(pschedulable_manager);
}

void CGDK::asio::system::process_destroy_scheduler() noexcept
{
	// check)
	assert(this->m_is_thread_run == false);

	// 1) reset scheduler manager
	auto pschedulable_manager = std::move(this->m_pschedulable_manager);

	// 2) destroy
	pschedulable_manager->process_destroy();
}

void CGDK::asio::system::process_initialize_single_executor()
{
	// check)
	assert(this->m_is_thread_run == true);

	// 1) create sigle executor
	auto pexecutor_single = std::make_shared<executor::single>();

	// 2) initialize single_executor
	pexecutor_single->process_initialize();

	this->m_pexecutor_single = std::move(pexecutor_single);
}

void CGDK::asio::system::process_destroy_single_executor() noexcept
{
	// check)
	assert(this->m_is_thread_run == false);

	// 1) reset executor_single
	auto pexecutor_single = std::move(this->m_pexecutor_single);

	// 2) destory 
	pexecutor_single->process_destroy();
}
