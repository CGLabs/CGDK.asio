#include "pch.h"

void socket_tcp::on_connect() 
{
	// 1) group에 입장합니다.(enter chatting group)
	g_pgroup_chatting->enter_member(std::dynamic_pointer_cast<socket_tcp>(this->shared_from_this()));

	// trace)
	std::cout << "@ connected" << std::endl;
}

void socket_tcp::on_disconnect(boost::system::error_code /*_error_code*/) noexcept
{
	// 1) group에서 퇴장합니다.(leave chatting group)
	g_pgroup_chatting->leave_member(std::dynamic_pointer_cast<socket_tcp>(this->shared_from_this()));

	// trace)
	std::cout << "@ disconnted" << std::endl;
}

int socket_tcp::on_message(sMESSAGE& _msg)
{
	// 1) 전체 group의 멤버들에게 전송합니다.(send to all member)
	g_pgroup_chatting->send(_msg.buf_message);

	// trace)
	std::cout << "@ message received" << std::endl;

	// return) 
	return 1;
}
