//*****************************************************************************
//*                                                                           *
//*                      Cho sanghyun's Game Classes II                       *
//*                       Ver 10.0 / Release 2019.12.11                       *
//*                                                                           *
//*                     asio.example.simle_chatting.server                    *
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

class group_chatting
{
public:
		void enter_member(std::shared_ptr<socket_tcp>&& _pmember);
		void leave_member(const std::shared_ptr<socket_tcp>& _pmember) noexcept;
		void send(shared_buffer _buf_message);
private:
		std::recursive_mutex m_lock_memer;
		std::set<std::shared_ptr<socket_tcp>> m_set_member;
};
