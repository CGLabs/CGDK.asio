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

class CGDK::asio::message_transmitter
{
public:
	static constexpr size_t	MAX_MEDIATOR = 8;

			int					transmit_message(sMESSAGE& _msg);

			bool				register_messageable(const std::shared_ptr<Imessageable>& _pmessageable);
			bool				register_messageable(std::shared_ptr<Imessageable>&& _pmessageable);
			template <class T>
			bool				register_messageable(const std::shared_ptr<T>& _pmessageable) { return this->register_messageable(dynamic_pointer_cast<Imessageable>(_pmessageable)); }
			template <class T>
			bool				register_messageable(std::shared_ptr<T>&& _pmessageable) { return this->register_messageable(dynamic_pointer_cast<Imessageable>(std::move(_pmessageable))); }
			bool				unregister_messageable(Imessageable* _pmessageable) noexcept;
	virtual	int					reset_message_transmitter() noexcept;

private:
			std::mutex			m_lock_container;
			std::vector<std::shared_ptr<Imessageable>> m_container;
};

// definitions)
#define	TRANSMIT_MESSAGE(msg)				\
{											\
	auto __result__ = transmit_message(msg);\
	if(__result__ != 0)						\
		return __result__;					\
}

