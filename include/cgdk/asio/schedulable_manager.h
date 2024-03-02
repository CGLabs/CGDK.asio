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

class CGDK::asio::schedulable_manager : public std::enable_shared_from_this<schedulable_manager>
{
public:
			bool				register_schedulable(std::shared_ptr<Ischedulable>&& _pschedulable) { return this->process_attach(std::move(_pschedulable)); }
			bool				unregister_schedulable(Ischedulable* _pschedulable) noexcept { return this->process_detach(_pschedulable) != 0; }

protected:
			bool				process_start();
			void				process_stop() noexcept;
			bool				process_execute(clock::duration _ticks_wait, intptr_t _option);

			bool				process_attach(std::shared_ptr<Ischedulable>&& _pschedulable);
			bool				process_detach(Ischedulable* _pschedulable) noexcept;

			void				process_execute_schedulable(std::shared_ptr<Ischedulable>&& _pschedulable);
			void				process_cancel_schedulable_all() noexcept;

			void				_push_schedulable(std::shared_ptr<Ischedulable>&& _pschedulable);
			void				_pop_schedulable() noexcept;

			std::atomic<int>	m_flag_run;
	mutable std::thread			m_thread;
			std::mutex			m_lockable_set_schedulable;
			struct compare
			{
				using is_transparent = void;
				bool operator() (const std::shared_ptr<Ischedulable>& _lhs, const std::shared_ptr<Ischedulable>& _rhs) const { return _lhs.get() < _rhs.get(); }
				bool operator() (const std::shared_ptr<Ischedulable>& _lhs, const Ischedulable* _rhs) const { return _lhs.get() < _rhs;}
				bool operator() (const Ischedulable* _lhs, const std::shared_ptr<Ischedulable>& _rhs) const { return _lhs < _rhs.get(); }
			};
			std::set<std::shared_ptr<Ischedulable>, compare> m_set_schedulable;
			std::vector<Ischedulable*> m_priorityqueue_schedulable;
			std::mutex				m_lockable_priorityqueue;
			std::condition_variable	m_cv_priorityqueue_exist;
			std::condition_variable	m_cv_priorityqueue_in_time;
			friend class system;

	static	uint32_t			fn_thread(schedulable_manager* _this);
};
