#include "cgdk/asio.h"


CGDK::asio::executor::single::single() : m_flag_run(0)
{
	this->process_create();
}

CGDK::asio::executor::single::~single() noexcept
{
	this->process_destroy();
}

bool CGDK::asio::executor::single::cancel(Iexecutable* _pexecutable) noexcept
{
	// lock)
	std::lock_guard cs(this->m_lockable_priorityqueue);

	// 1) get first executable object
	auto iter_end = this->m_priorityqueue_executable.end();
	auto iter = std::find_if(this->m_priorityqueue_executable.begin(), iter_end, [=](const EXECUTION_AT& iter) { return iter.pexecutable.get() == _pexecutable; });

	// check) return if not exists
	if(iter == iter_end)
		return false;

	// 2) get last item
	auto ptarget = std::move(this->m_priorityqueue_executable.back());
	auto tick_compare = ptarget.tick_at;

	// 3) get pos_now of item
	auto pos_now = static_cast<size_t>(iter - this->m_priorityqueue_executable.begin());

	// check) notify one 
	if (iter == this->m_priorityqueue_executable.begin())
		this->m_cv_priorityqueue_in_time.notify_one();

	// 4) remove tail of heap
	this->m_priorityqueue_executable.pop_back();

	// check) 
	if (pos_now == this->m_priorityqueue_executable.size())
		return true;

	// check) 
	assert(pos_now < this->m_priorityqueue_executable.size());

	// case A) up heap
	if (pos_now != 0 && this->m_priorityqueue_executable.at((pos_now - 1) >> 1).tick_at > tick_compare)
	{
		// - ...
		auto iter_child = iter;

		do
		{
			// - minus 1 then divied 2
			pos_now = (pos_now - 1) >> 1;

			// - get left node
			auto iter_now = this->m_priorityqueue_executable.begin() + pos_now;

			// check) 
			if (iter_now->tick_at <= tick_compare)
				break;

			// - move to child node
			*iter_child = std::move(*iter_now);
			iter_child = iter_now;
		} while (pos_now != 0);

		// - 
		*iter_child = std::move(ptarget);
	}
	// case B) down heap
	else
	{
		// - multiply 2 + 1
		pos_now = (pos_now << 1) + 1;

		// - size & parent
		auto pos_max = this->m_priorityqueue_executable.size() - 1;
		auto iter_parent = iter;

		while (pos_now <= pos_max)
		{
			// - get left node
			auto iter_now = this->m_priorityqueue_executable.begin() + pos_now;

			// - select right node if right node is larger then left node
			if (pos_now < pos_max && iter_now->tick_at >(iter_now + 1)->tick_at)
				++iter_now;

			// check) 
			if (tick_compare <= iter_now->tick_at)
				break;

			// - move to parent node
			*iter_parent = std::move(*iter_now);
			iter_parent = iter_now;

			// - set new pos_now ( pos_now x 2 + 1 )
			pos_now = (pos_now << 1) + 1;
		}

		*iter_parent = std::move(ptarget);
	}

	// return) 
	return true;
}

void CGDK::asio::executor::single::cancel() noexcept
{
	// lock)
	std::lock_guard<std::mutex> lock(this->m_lockable_priorityqueue);

	if (this->m_priorityqueue_executable.empty())
	{
		this->m_cv_priorityqueue_exist.notify_one();
	}
	else
	{
		this->m_priorityqueue_executable.clear();
		this->m_cv_priorityqueue_in_time.notify_one();
	}
}

void CGDK::asio::executor::single::process_create()
{
	// 1) set m_flag_run 'true' and get old state
	const int state_old = this->m_flag_run.exchange(1);

	// check) return if aleady running
	if(state_old != 0)
		return;

	try
	{
		// 2) reserve
		{
			std::lock_guard<std::mutex> lock(this->m_lockable_priorityqueue);

			this->m_priorityqueue_executable.reserve(16384);
		}

		// 3) start thread
		this->m_thread = std::thread(fn_thread, this);
	}
	catch (...)
	{
		// - destroy
		this->process_destroy();

		// throw) 
		throw;
	}
}

void CGDK::asio::executor::single::process_destroy() noexcept
{
	// 1) set m_flag_run 'false' and get old state
	const int state_old = this->m_flag_run.exchange(0);

	// check) return if not running
	if(state_old == 0)
		return;

	// 2) calcel
	this->cancel();

	// 3) wait stop thread
	if (this->m_thread.joinable())
		this->m_thread.join();
}

void CGDK::asio::executor::single::process_schedulable()
{
	while (this->m_flag_run != 0)
	{
		try
		{
			while(this->m_flag_run != 0)
			{
				// lock)
				std::unique_lock<std::mutex> lock_wait{ this->m_lockable_priorityqueue };

				// 1) wait until 
				if (this->m_priorityqueue_executable.empty())
					this->m_cv_priorityqueue_exist.wait(lock_wait);

				// 2) get now tick
				auto tick_now = clock::now();

				// 3) execute 
				while (this->m_priorityqueue_executable.empty() == false)
				{
					// - get first eecutable object
					auto& pexecutable = this->m_priorityqueue_executable.at(0);

					// check) wait if not yet 
					if (pexecutable.tick_at > tick_now)
					{
						// - wait time
						auto time_wait = pexecutable.tick_at - tick_now;

						// - sleep
						this->m_cv_priorityqueue_in_time.wait_for(lock_wait, time_wait);

						// - get now tick
						tick_now = clock::now();

						// continue)
						continue;
					}

					struct TT
					{
						std::shared_ptr<Iexecutable> t;
						void operator () () { t->process_execute(); }
					};

					// - execute 'executable'
					system::post(TT{ std::move(pexecutable.pexecutable) });

					// statistics)
					//++this->m_statistics.now.count_execute;

					// - remove 'executable'
					this->process_pop_executable();
				}
			}
		}
		// On Exception) 
		catch (std::exception& /*_exception*/)
		{
		}
	}
}

void CGDK::asio::executor::single::process_push_executable(EXECUTION_AT&& _execute_at)
{
	// lock
	std::lock_guard cs(this->m_lockable_priorityqueue);

	// check) 
	if(this->m_flag_run == 0)
		return;

	// 1) store pos_now
	auto pos_now = this->m_priorityqueue_executable.size();

	// 2) push back item
	this->m_priorityqueue_executable.emplace_back(EXECUTION_AT());

	// 3) heap up
	auto iter_child = --this->m_priorityqueue_executable.end();

	while (pos_now != 0)
	{
		// - 빼기 1 후 나누기 2한다.
		pos_now = ((pos_now - 1) >> 1);

		// - ...
		auto iter_now = this->m_priorityqueue_executable.begin() + pos_now;

		// check) 
		if (iter_now->tick_at <= _execute_at.tick_at)
			break;

		// - move to down
		*iter_child = std::move(*iter_now);
		iter_child = iter_now;
	}

	// 4) ...
	*iter_child = std::move(_execute_at);

	// check)
	assert(this->m_priorityqueue_executable.empty() == false);

	// 5) notify_one
	if (this->m_priorityqueue_executable.size() == 1)
		this->m_cv_priorityqueue_exist.notify_one();
	else if (iter_child->pexecutable == this->m_priorityqueue_executable[0].pexecutable)
		this->m_cv_priorityqueue_in_time.notify_one();
}

void CGDK::asio::executor::single::process_pop_executable() noexcept
{
	// 1) 
	auto ptarget = std::move(this->m_priorityqueue_executable.back());
	auto tick_compare = ptarget.tick_at;

	// 2) target객체를 제거하고 제일 마지막 객체를 그 위치로 가져 온다.
	this->m_priorityqueue_executable.pop_back();

	// check) 
	if (this->m_priorityqueue_executable.empty())
		return;

	// 3) Size & Parent
	auto pos_max = this->m_priorityqueue_executable.size() - 1;
	auto pos_now = size_t(1);
	auto iter_parent = this->m_priorityqueue_executable.begin();

	// 4) down heap
	while (pos_now <= pos_max)
	{
		// - get now node
		auto iter_now = this->m_priorityqueue_executable.begin() + pos_now;

		// - Left가 더 크면 Right 선택한다.
		if (pos_now < pos_max && iter_now->tick_at >(iter_now + 1)->tick_at)
		{
			++iter_now;
			++pos_now;
		}

		// check) Child가 더 크면 끝냄.
		if (tick_compare <= iter_now->tick_at)
			break;

		// - move to up
		*iter_parent = std::move(*iter_now);
		iter_parent = iter_now;

		// - set child node as new pos_now (pos_now = pos_now x 2 + 1)
		pos_now = (pos_now << 1) + 1;
	}

	*iter_parent = std::move(ptarget);
}

void CGDK::asio::executor::single::fn_thread(single* _this)
{
	_this->process_schedulable();
}
