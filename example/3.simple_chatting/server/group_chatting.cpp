#include "pch.h"

void group_chatting::enter_member(std::shared_ptr<socket_tcp>&& _pmember)
{

	// lock)
	std::lock_guard lock(this->m_lock_memer);

	// 1) 멤버 리스트에 추가합니다. (add member to member_list)
	this->m_set_member.insert(std::move(_pmember));

	// 2) 전체에게 입장 메시지를 전송 (메시지크기(8) + MESSAGE_TYPE(LEAVE))
	this->send(alloc_shared_buffer(8) << uint32_t(8) << eMESSAGE::ENTER);
}

void group_chatting::leave_member(const std::shared_ptr<socket_tcp>& _pmember) noexcept
{
	// lock)
	std::lock_guard lock(this->m_lock_memer);

	// 1) 멤버 리스트에서 제거합니다.(remove member from member_list)
	this->m_set_member.erase(_pmember);

	// 2) 전체 멤버에게 퇴장 메시지를 전송 (메시지크기(8) + MESSAGE_TYPE(LEAVE))
	this->send(alloc_shared_buffer(8) << uint32_t(8) << eMESSAGE::LEAVE); 
}

void group_chatting::send(shared_buffer _buf_message)
{
	// lock)
	std::lock_guard lock(this->m_lock_memer);

	// 1) 전체에게 전송
	for (auto& iter : this->m_set_member)
		iter->send(_buf_message);
}
