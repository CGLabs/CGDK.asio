//*****************************************************************************
//*                                                                           *
//*                      Cho sanghyun's Game Classes II                       *
//*                       Ver 10.0 / Release 2019.12.11                       *
//*                                                                           *
//*                              object classes                               *
//*                                                                           *
//*                                                                           *
//*                                                                           *
//*                                                                           *
//*  This Program is programmed by Cho SangHyun. sangduck@cgcii.co.kr         *
//*  Best for Game Developement and Optimized for Game Developement.          *
//*                                                                           *
//*                (c) 2003. Cho Sanghyun. All right reserved.                *
//*                          http://www.CGCII.co.kr                           *
//*                                                                           *
//*****************************************************************************
#pragma once

//-----------------------------------------------------------------------------
/**

 @class		Imessage_transmitter

*/
//-----------------------------------------------------------------------------
class CGDK::asio::message_transmitter
{
public:
			int					transmit_message(sMESSAGE& _msg);// { return this->transmit_message(_msg); }

			bool				register_messageable(const std::shared_ptr<Imessageable>& _pmessageable);
			bool				register_messageable(std::shared_ptr<Imessageable>&& _pmessageable);
			bool				unregister_messageable(Imessageable* _pmessageable) noexcept;
	virtual	int					reset_message_transmitter() noexcept;
};

// definitions)
#define	TRANSMIT_MESSAGE(msg)				\
{											\
	auto __result__ = transmit_message(msg);\
	if(__result__ != 0)						\
		return __result__;					\
}
