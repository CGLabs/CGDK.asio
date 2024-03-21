#include "pch.h"

void group_chatting::on_member_entered(socket_tcp* _pmember, param_t& /*_param*/)
{
	// 1) 전체에게 입장 메시지를 전송 (메시지크기(8) + MESSAGE_TYPE(LEAVE))
	this->send(alloc_shared_buffer(8) << uint32_t(8) << eMESSAGE::ENTER);
}

uintptr_t group_chatting::on_member_leaving(socket_tcp* _pmember, uintptr_t _param)
{
	// 1) 전체 멤버에게 퇴장 메시지를 전송 (메시지크기(8) + MESSAGE_TYPE(LEAVE))
	this->send(alloc_shared_buffer(8) << uint32_t(8) << eMESSAGE::LEAVE); 

	// return) 
	return _param;
}
