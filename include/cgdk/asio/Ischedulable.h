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

class CGDK::asio::Ischedulable
{
public:
	virtual ~Ischedulable() noexcept {}

	virtual	bool				process_on_register() = 0;
	virtual	void				process_on_unregister() = 0;
	virtual bool				process_schedule() = 0;

			void				next_tick(clock::time_point _tick_next) { this->m_tick_next = _tick_next; }
	[[nodiscard]] auto			next_tick() const noexcept { return this->m_tick_next; }

	[[nodiscard]] auto			execute_interval() const noexcept { return this->m_tick_diff_execute; }
			void				execute_interval(clock::duration _tick_interval) { this->m_tick_diff_execute = _tick_interval; this->m_tick_next = clock::now() + m_tick_diff_execute; }

protected:
			std::shared_ptr<schedulable_manager> m_pschedulable_manager;
			clock::time_point m_tick_next = clock::time_point::max();
			clock::duration	m_tick_diff_execute = 1s;
			//statistics::schedulable m_statistics_schedulable;
			friend class schedulable_manager;
};
