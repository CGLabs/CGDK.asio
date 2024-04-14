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

struct sMESSAGE
{
	int message_type = 0;
	void* source = nullptr;
	shared_buffer buf_message;

	template<class T>
	auto get_source() const  noexcept { return reinterpret_cast<T*>(source); }
	void set_source(void* _source) noexcept { source = _source; }
};

class CGDK::asio::Imessageable
{
public:
	virtual int process_message(sMESSAGE _msg) = 0;
};

class CGDK::asio::messageable : virtual public CGDK::asio::Imessageable
{
public:
	virtual int process_message(sMESSAGE _msg) override { return this->on_message(_msg); }
protected:
	virtual int on_message(sMESSAGE& /*_msg*/) { return 0; }
};
