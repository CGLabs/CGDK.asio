#include "pch.h"

void group_chatting::on_member_entered(socket_tcp* _pmember, param_t& /*_param*/)
{
	// 1) message transmitter에 현재 group을 등록한다.
	_pmember->register_messageable(this->shared_from_this());

	// 2) 전체에게 입장 메시지를 전송 (메시지크기(8) + MESSAGE_TYPE(LEAVE))
	this->send(alloc_shared_buffer(8) << uint32_t(8) << eMESSAGE::ENTER);
}

uintptr_t group_chatting::on_member_leaving(socket_tcp* _pmember, uintptr_t _param)
{
	// 1) message transmitter에 현재 group을 등록한다.
	_pmember->unregister_messageable(this);

	// 2) 전체 멤버에게 퇴장 메시지를 전송 (메시지크기(8) + MESSAGE_TYPE(LEAVE))
	this->send(alloc_shared_buffer(8) << uint32_t(8) << eMESSAGE::LEAVE); 

	// return) 
	return _param;
}

int group_chatting::on_message(sMESSAGE& _msg)
{
	// 1) get source
	[[maybe_unused]] auto psource = _msg.get_source<socket_tcp>(); // (!) this is source of msg (set at 'socket_tcp.cpp' line '24')

	// 2) 전체 group의 멤버들에게 전송합니다.(send to all member)
	g_pgroup_chatting->send(_msg.buf_message);

	// return) 
	return 1;
}
