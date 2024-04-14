#include "pch.h"

void socket_tcp::on_connect() 
{
	// 1) group에 입장합니다.(enter chatting group)
	g_pgroup_chatting->enter(this->shared_from_this());

	// trace)
	std::cout << "@ connected" << std::endl;
}

void socket_tcp::on_disconnect(boost::system::error_code /*_error_code*/) noexcept
{
	// 1) group에서 퇴장합니다.(leave chatting group)
	g_pgroup_chatting->leave(this);

	// trace)
	std::cout << "@ disconnted" << std::endl;
}

int socket_tcp::on_message(sMESSAGE& _msg)
{
	// 1) msg의 source를 this로 설정한다.
	_msg.set_source(this);

	// 2) message를 dispatch하다.
	TRANSMIT_MESSAGE(_msg);

	// return) 
	return 1;
}
