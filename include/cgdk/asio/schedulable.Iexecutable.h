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

class CGDK::asio::schedulable::Iexecutable : virtual public Ischedulable, virtual public asio::Iexecutable
{
protected:
	virtual ~Iexecutable() noexcept {}

	virtual	bool				process_on_register() override
	{
		// 1) set next tick time
		this->m_tick_next = clock::now() + this->m_tick_diff_execute;

		// return)
		return true;
	}
	virtual	void				process_on_unregister() override
	{
		this->m_tick_next = (clock::time_point::max)();
	}
	virtual	bool				process_schedule() override
	{
		// check) 
		if (clock::now() < this->m_tick_next)
			return false;

		// 1) execute
		try
		{
			this->process_execute();
		}
		catch (...)
		{
		}

		// 2) set next tick
		this->m_tick_next = this->m_tick_next + this->m_tick_diff_execute;

		// return)
		return true;
	}
};
