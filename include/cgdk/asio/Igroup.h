//*****************************************************************************
//*                                                                           *
//*                      Cho sanghyun's Game Classes II                       *
//*                       Ver 10.0 / Release 2019.12.11                       *
//*                                                                           *
//*                          Group Template Classes                           *
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

namespace CGDK
{
//-----------------------------------------------------------------------------
/**
 @class		Igroup
*///-----------------------------------------------------------------------------
template <class _TMEMBER>
class Igroup :
{
public:
			bool				leave(_TMEMBER* _pmember) noexcept;
	[[nodiscard]] auto&			get_group_lockable() noexcept { return this->m_cs_group;}

protected:
	virtual	void				process_leave(_TMEMBER* _pmember) noexcept = 0;
			std::mutex			m_cs_group;
 
protected:
	template <class group_t>
	static	void				member_group_as(group_t* _pgroup, _TMEMBER* _pmember) noexcept	{ _pmember->_set_group(_pgroup);}
	static	void				reset_member_group(_TMEMBER* _pmember) noexcept { _pmember->_reset_group();}
	[[nodiscard]] static lockable<>& member_group_lock(_TMEMBER* _pmember) noexcept { return _pmember->m_lockable_group;}
};

template <class _TMEMBER>
bool Igroup<_TMEMBER>::leave(_TMEMBER* _pmember) noexcept
{
	// check)
	if(_pmember == nullptr)
		return false;

	// lock)
	std::lock_guard<std::mutex> cs(_pmember->m_cs_group);

	// check) 아에 Group에 속해 있지 않으면 그냥 끝낸다.
	if(_pmember->_get_group() != this)
		return false;

	// 1) 일단 Group에서 Leave를 먼저 한다.
	this->process_leave(_pmember);

	// return) 
	return true;
}


}