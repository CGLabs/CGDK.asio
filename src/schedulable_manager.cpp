#include "cgdk/asio.h"


bool CGDK::asio::schedulable_manager::process_initialize()
{
	// 1) set m_flag_run 'true' and get old state
	const int state_old = this->m_flag_run.exchange(1);

	// check) return if aleady running
	if(state_old != 0)
		return false;

	// 2) reserve
	{
		// lock)
		std::lock_guard<std::mutex> cs(this->m_lockable_priorityqueue);

		// - clear & reserve
		{
			// lock)
			std::lock_guard cs_nested(this->m_lockable_set_schedulable);

			this->m_set_schedulable.clear();
		}
		this->m_priorityqueue_schedulable.reserve(16384);
	}

	try
	{
		// 4) begin thread
		this->m_thread = std::thread(fn_thread, this);

	}
	catch (...)
	{
		// - stop
		this->process_destroy();

		// reraise)
		throw;
	}

	return true;
}

void CGDK::asio::schedulable_manager::process_destroy() noexcept
{
	// 1) set m_flag_run 'false' and get old state
	const int state_old = this->m_flag_run.exchange(0);

	// check) return if not running
	if(state_old == 0)
		return;

	// 2) cancel all executable
	this->process_cancel_schedulable_all();

	// 3) wait stop thread
	if (this->m_thread.joinable())
		this->m_thread.join();
}

void CGDK::asio::schedulable_manager::process_cancel_schedulable_all() noexcept
{
	// declare) 
	std::set<std::shared_ptr<Ischedulable>, compare> set_schedulable;

	// 1) terminate execting loop
	{
		// lock)
		std::lock_guard<std::mutex> cs(this->m_lockable_priorityqueue);

		// - swap
		{
			// lock)
			std::lock_guard<std::mutex> cs_nested(this->m_lockable_set_schedulable);

			set_schedulable.swap(this->m_set_schedulable);
		}

		// - clear priority queue
		if (this->m_priorityqueue_schedulable.empty())
		{
			this->m_cv_priorityqueue_exist.notify_one();
		}
		else
		{
			this->m_priorityqueue_schedulable.clear();
			this->m_cv_priorityqueue_in_time.notify_one();
		}
	}

	// 2) call 'process_on_unregister' all
	for (auto& iter : set_schedulable)
	{
		iter->process_on_unregister();
	}
}

bool CGDK::asio::schedulable_manager::process_execute(clock::duration /*_ticks_wait*/, intptr_t /*_option*/)
{
	while (this->m_flag_run)
	{
		try
		{
			while (this->m_flag_run)
			{
				std::unique_lock<std::mutex> lock_wait{ this->m_lockable_priorityqueue };

				// 1) wait until
				if (this->m_priorityqueue_schedulable.empty())
				{
					this->m_cv_priorityqueue_exist.wait(lock_wait);
				}

				// 2) get now tick
				auto tick_now = clock::now();

				// 3) execute 
				while (this->m_priorityqueue_schedulable.empty() == false)
				{
					// - get first eecutable object
					auto* pschedulable = this->m_priorityqueue_schedulable.at(0);

					// - get next tick
					auto next_tick = pschedulable->next_tick();

					// check) wait if not yet 
					if (next_tick > tick_now)
					{
						// - wait time
						auto time_wait = next_tick - tick_now;

						// - sleep
						this->m_cv_priorityqueue_in_time.wait_for(lock_wait, time_wait);

						// - get now tick
						tick_now = clock::now();

						// continue)
						continue;
					}

					// - remove 'executable'
					this->_pop_schedulable();

					// - execute 'executable'
					class executable_schedulable : public Iexecutable
					{
					public:
						std::shared_ptr<schedulable_manager> pscheduler;
						std::shared_ptr<Ischedulable> pschedulable;
						virtual void process_execute() override
						{
							// check)
							assert(this->pscheduler);

							// declare)
							std::shared_ptr<Ischedulable> pschedulable_temp = std::move(this->pschedulable);

							// check)
							assert(pschedulable_temp);

							//// statistics)
							//pschedulable_temp->statistics_on_extra();
							//++pschedulable_temp->m_statistics_schedulable.count_execute;

							// 1) execute
							this->pscheduler->process_execute_schedulable(std::move(pschedulable_temp));
						}
					};

					auto pexecutable_scheduler = std::make_shared<executable_schedulable>();
					pexecutable_scheduler->pscheduler = this->shared_from_this();
					pexecutable_scheduler->pschedulable = *this->m_set_schedulable.find(pschedulable);

					// - post execut
					system::post(std::move(pexecutable_scheduler));

					//// statistics)
					//++this->m_statistics.now.count_execute;
				}
			}
		}
		// On Exception) 
		catch (std::exception& /*_exception*/)
		{
		}
	}

	// return)
	return true;
}

bool CGDK::asio::schedulable_manager::process_attach(std::shared_ptr<Ischedulable>&& _pschedulable)
{
	// check) 
	assert(_pschedulable);

	// check) 
	if(!_pschedulable)
		return false;

	{
		// lock)
		std::lock_guard<std::mutex> cs(this->m_lockable_priorityqueue);

		// check) aleady registered
		if(_pschedulable->m_pschedulable_manager)
			return false;

		// 1) attach
		_pschedulable->m_pschedulable_manager = this->shared_from_this();

		try
		{
			{
				// lock)
				std::lock_guard<std::mutex> cs_nested(this->m_lockable_set_schedulable);

				// check) 
				assert(this->m_set_schedulable.find(_pschedulable) == this->m_set_schedulable.end());

				// 2) call 'process_on_register'
				auto result = _pschedulable->process_on_register();

				// check) 
				if(result == false)
					return false;

				// 3) push_back
				this->m_set_schedulable.emplace(_pschedulable);
			}

			// 4) push schedulable
			_push_schedulable(std::move(_pschedulable));
		}
		catch (...)
		{
			// - detach from set'schedulable'
			{
				// lock)
				std::lock_guard<std::mutex> cs_nested(this->m_lockable_set_schedulable);

				this->m_set_schedulable.erase(_pschedulable);
			}

			// - ...
			_pschedulable->m_pschedulable_manager.reset();

			// reraise)
			throw;
		}
	}

	// return) 
	return true;
}

bool CGDK::asio::schedulable_manager::process_detach(Ischedulable* _pschedulable) noexcept
{
	// check) 
	if(_pschedulable == nullptr)
		return false;

	// declare)
	std::shared_ptr<CGDK::asio::schedulable_manager> phold_this;

	{
		// lock)
		std::unique_lock<std::mutex> cs(this->m_lockable_priorityqueue);

		// check) 
		if(_pschedulable->m_pschedulable_manager.get() != this)
			return false;

		{
			// lock)
			std::lock_guard<std::mutex> cs_nested(this->m_lockable_set_schedulable);

			// 1) detach from set
			auto iter_find = this->m_set_schedulable.find(_pschedulable);

			// check)
			if(iter_find == this->m_set_schedulable.end())
				return false;

			// 2) erase from set
			this->m_set_schedulable.erase(iter_find);
		}

		// 3) get first executable object
		auto iter_end = this->m_priorityqueue_schedulable.end();
		auto iter = std::find_if(this->m_priorityqueue_schedulable.begin(), iter_end, [=](const Ischedulable* iter) { return iter == _pschedulable; });

		// 4) remove schedulable if exist
		if (iter != iter_end)
		{
			// - get last item
			auto ptarget = std::move(this->m_priorityqueue_schedulable.back());
			auto tick_compare = ptarget->next_tick();

			// - get pos_now of item
			auto pos_now = static_cast<size_t>(iter - this->m_priorityqueue_schedulable.begin());

			// check) 
			if (iter == this->m_priorityqueue_schedulable.begin())
			{
				// - notify one
				this->m_cv_priorityqueue_in_time.notify_one();
			}

			// - remove tail of heap
			this->m_priorityqueue_schedulable.pop_back();

			// check) 
			if (pos_now != this->m_priorityqueue_schedulable.size())
			{
				// check) 
				assert(pos_now < this->m_priorityqueue_schedulable.size());

				// case A) up heap
				if (pos_now != 0 && this->m_priorityqueue_schedulable.at((pos_now - 1) >> 1)->next_tick() > tick_compare)
				{
					// - ...
					auto iter_child = iter;

					do
					{
						// - minus 1 then divied 2
						pos_now = (pos_now - 1) >> 1;

						// - get left node
						auto iter_now = this->m_priorityqueue_schedulable.begin() + pos_now;

						// check) 
						if ((*iter_now)->next_tick() <= tick_compare)
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
					pos_now = (pos_now << 1) + 1;

					// - size & parent
					auto pos_max = this->m_priorityqueue_schedulable.size() - 1;
					auto iter_parent = iter;

					while (pos_now <= pos_max)
					{
						// - get left node
						auto iter_now = this->m_priorityqueue_schedulable.begin() + pos_now;

						// - select right node if right node is larger then left node
						if (pos_now < pos_max && (*iter_now)->next_tick() >(*(iter_now + 1))->next_tick())
						{
							++iter_now;
						}

						// check) 
						if (tick_compare <= (*iter_now)->next_tick())
							break;

						// - move to parent node
						*iter_parent = std::move(*iter_now);
						iter_parent = iter_now;

						// - set new pos_now ( pos_now x 2 + 1 )
						pos_now = (pos_now << 1) + 1;
					}

					*iter_parent = std::move(ptarget);
				}
			}
		}

		// 5) reset schedulable
		phold_this = std::move(_pschedulable->m_pschedulable_manager);
	}

	// 6) Hook
	_pschedulable->process_on_unregister();

	// 7) reset hold
	phold_this.reset();

	// return) 
	return true;
}

void CGDK::asio::schedulable_manager::_push_schedulable(std::shared_ptr<Ischedulable>&& _pschedulable)
{
	// check)
	assert(_pschedulable->m_pschedulable_manager.get() == this);

	// check)
	if (_pschedulable->next_tick() == (clock::time_point::max)())
		return;

	// 1) store pos_now
	auto pos_now = this->m_priorityqueue_schedulable.size();

	// 2) push back item
	this->m_priorityqueue_schedulable.emplace_back(_pschedulable.get());

	// 3) heap up
	auto iter_child = --this->m_priorityqueue_schedulable.end();

	while (pos_now != 0)
	{
		// - 빼기 1 후 나누기 2한다.
		pos_now = ((pos_now - 1) >> 1);

		// - ...
		auto iter_now = this->m_priorityqueue_schedulable.begin() + pos_now;

		// check) 
		if ((*iter_now)->next_tick() <= _pschedulable->next_tick())
			break;

		// - move to down
		*iter_child = std::move(*iter_now);
		iter_child = iter_now;
	}

	// 4) ...
	*iter_child = _pschedulable.get();

	// check)
	assert(this->m_priorityqueue_schedulable.empty() == false);

	// 5) notify_one
	if (this->m_priorityqueue_schedulable.size() == 1)
	{
		this->m_cv_priorityqueue_exist.notify_one();
	}
	else if (_pschedulable.get() == this->m_priorityqueue_schedulable[0])
	{
		this->m_cv_priorityqueue_in_time.notify_one();
	}
}

void CGDK::asio::schedulable_manager::_pop_schedulable() noexcept
{
	// 1) 
	auto ptarget = std::move(this->m_priorityqueue_schedulable.back());
	auto tick_compare = ptarget->next_tick();

	// 2) target객체를 제거하고 제일 마지막 객체를 그 위치로 가져 온다.
	this->m_priorityqueue_schedulable.pop_back();

	// check) 
	if (this->m_priorityqueue_schedulable.empty())
		return;

	// 3) Size & Parent
	auto pos_max = this->m_priorityqueue_schedulable.size() - 1;
	auto pos_now = size_t(1);
	auto iter_parent = this->m_priorityqueue_schedulable.begin();

	// 4) down heap
	while (pos_now <= pos_max)
	{
		// - get now node
		auto iter_now = this->m_priorityqueue_schedulable.begin() + pos_now;

		// - Left가 더 크면 Right 선택한다.
		if (pos_now < pos_max && (*iter_now)->next_tick() >(*(iter_now + 1))->next_tick())
		{
			++iter_now;
			++pos_now;
		}

		// check) Child가 더 크면 끝냄.
		if (tick_compare <= (*iter_now)->next_tick())
			break;

		// - move to up
		*iter_parent = std::move(*iter_now);
		iter_parent = iter_now;

		// - set child node as new pos_now (pos_now = pos_now x 2 + 1)
		pos_now = (pos_now << 1) + 1;
	}

	*iter_parent = std::move(ptarget);
}

void CGDK::asio::schedulable_manager::process_execute_schedulable(std::shared_ptr<Ischedulable>&& _pschedulable)
{
	{
		// lock)
		std::lock_guard<std::mutex> cs(this->m_lockable_priorityqueue);

		// check)
		if (_pschedulable->m_pschedulable_manager.get() != this)
			return;

		// check)
		if (_pschedulable->next_tick() == (clock::time_point::max)())
			return;
	}

	// 1) execute
	auto result = _pschedulable->process_schedule();

	// check)
	if (result == false)
		return;

	// 2) re-push
	{
		// lock)
		std::lock_guard<std::mutex> cs(this->m_lockable_priorityqueue);

		// check)
		if (_pschedulable->m_pschedulable_manager.get() != this)
			return;

		// - push again
		this->_push_schedulable(std::move(_pschedulable));
	}
}

uint32_t CGDK::asio::schedulable_manager::fn_thread(CGDK::asio::schedulable_manager* _this)
{
	//// statistics)
	//++_this->m_statistics.now.thread_count;

	// 1) call 'execute'
	auto result = _this->process_execute(clock::duration(), 0);

	//// statistics)
	//--_this->m_statistics.now.thread_count;

	// return) 
	return result;
}
