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

template <class TEXECUTE = std::function<void()>>
class CGDK::asio::schedulable::executable: virtual public schedulable::Iexecutable
{
public:
			executable() noexcept {}
			executable(TEXECUTE&& _function) noexcept : m_function_completion(std::forward<TEXECUTE>(_function)) {}
	virtual	~executable() noexcept {}

			void				set_function(TEXECUTE&& _function) noexcept { std::lock_guard cs(this->m_lockable_completion); this->m_function_completion = std::forward<TEXECUTE>(_function); }
	[[nodiscard]] TEXECUTE&		get_function() const noexcept { std::lock_guard cs(this->m_lockable_completion); return this->m_function_completion; }

private:
			std::mutex			m_lockable_completion;
			TEXECUTE			m_function_completion{ nullptr };
	virtual	void				process_execute() override { this->m_function_completion(); }
};

