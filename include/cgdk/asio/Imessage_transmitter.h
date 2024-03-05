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
class CGDK::Imessage_transmitter : virtual public object::Iattachable<Imessageable>
{
public:
	virtual ~Imessage_transmitter() noexcept {}

public:
			int					transmit_message(sMESSAGE& _msg) { return process_transmit_message(_msg); }
			int					transmit_message(sMESSAGE&& _msg) { return process_transmit_message(_msg); }

			bool				register_messageable(const object_ptr<Imessageable>& _pmessageable) { return process_attach(object_ptr<Imessageable>(_pmessageable)); }
			bool				register_messageable(object_ptr<Imessageable>&& _pmessageable) { return process_attach(std::move(_pmessageable));}
			bool				unregister_messageable(Imessageable* _pmessageable) noexcept { return process_detach(_pmessageable)!=0;}
	virtual	int					reset_message_transmitter() noexcept { return process_reset_message_transmitter();}

protected:
	virtual result_code			process_transmit_message(sMESSAGE& _msg) PURE;
	virtual int					process_reset_message_transmitter() noexcept PURE;
};

// definitions)
#define	TRANSMIT_MESSAGE(msg)				\
{											\
	auto __result__ = transmit_message(msg);\
	if(__result__ != 0)						\
		return __result__;					\
}

namespace CGDK
{
	inline int transmit_message_to(Imessage_transmitter* _ptransmitter, sMESSAGE& _msg)
	{
		// check)
		RETURN_IF(_ptransmitter == nullptr, 0);

		// 1) transmite message
		return _ptransmitter->transmit_message(_msg);
	}

	inline int transmit_message_to(Imessage_transmitter* _ptransmitter, sMESSAGE&& _msg)
	{
		// check)
		RETURN_IF(_ptransmitter == nullptr, 0);

		// 1) transmite message
		return _ptransmitter->transmit_message(std::move(_msg));
	}
}