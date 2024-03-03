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

class CGDK::asio::executor::single
{
public:
	struct EXECUTION_AT
	{
		clock::time_point	tick_at;
		std::shared_ptr<Iexecutable> pexecutable;
	};

public:
			single();
	virtual ~single();

			template <class T>
			bool post_at(T&& _completor, clock::time_point _tick_execute)
			{
				// declaration)
				struct executable_function : virtual public Iexecutable
				{
					virtual void process_execute() override { this->f_execute(); }
					T f_execute;
				};

				// 1) alloc executable
				auto pexecutable = std::make_shared<executable_function>();

				// 2) set '_function' to pexecutable
				pexecutable->f_execute = std::forward<T>(_completor);

				// 3) process push
				this->process_push_executable(EXECUTION_AT{ _tick_execute, std::move(pexecutable) });

				// return)
				return true;
			}
			template <class TEXECUTABLE>
			std::enable_if_t<std::is_base_of_v<Iexecutable, TEXECUTABLE> , bool>
			post_at(std::shared_ptr<TEXECUTABLE>&& _pexecutable, clock::time_point _tick_execute)
			{
				// check) 
				assert(_pexecutable);

				// 1) process push
				this->process_push_executable(EXECUTION_AT{ _tick_execute, std::move(_pexecutable) });

				// return)
				return true;
			}
			bool cancel(Iexecutable* _pexecutable) noexcept;
			void cancel() noexcept;
private:
			void process_create();
			void process_destroy() noexcept;
			void process_push_executable(EXECUTION_AT&& _execute_at);
			void process_pop_executable() noexcept;
			void process_schedulable();

			std::vector<EXECUTION_AT> m_priorityqueue_executable;
			std::mutex m_lockable_priorityqueue;
			std::condition_variable	m_cv_priorityqueue_exist;
			std::condition_variable	m_cv_priorityqueue_in_time;

			std::atomic<int> m_flag_run;
	mutable std::thread m_thread;
	static	void fn_thread(single* _this);
};
