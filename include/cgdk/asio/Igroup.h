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

template <class _TMEMBER>
class Igroup : public std::enable_shared_from_this<Igroup<_TMEMBER>>
{
public:
	using member_t			 = _TMEMBER;

public:
			bool				leave(_TMEMBER* _pmember, uintptr_t _param = 0) noexcept;
	[[nodiscard]] auto&			get_group_lockable() noexcept { return this->m_cs_group;}

	[[nodiscard]] virtual std::shared_ptr<_TMEMBER> find_member(const std::function<bool(const std::shared_ptr<_TMEMBER>&)>& _pred) = 0;
	virtual	bool				for_each_member(const std::function<bool(const std::shared_ptr<_TMEMBER>&)>& _function) = 0;

protected:
	virtual	void				process_leave(_TMEMBER* _pmember, uintptr_t _param) noexcept = 0;
			std::mutex			m_cs_group;
 
	template <class group_t>
	static	void				member_group_as(std::shared_ptr<group_t>&& _pgroup, _TMEMBER* _pmember) noexcept { _pmember->_set_group(std::move(_pgroup));}
	static	void				member_reset_group(_TMEMBER* _pmember) noexcept { _pmember->_reset_group();}
	template <class TITER>
	static	auto				member_put_iter(_TMEMBER* _pmember, TITER _iter) noexcept { _pmember->m_iter=_iter; }
	template <class TITER>
	static	auto				member_get_iter(_TMEMBER* _pmember) noexcept { return std::any_cast<TITER>(_pmember->m_iter); _pmember->m_iter.reset(); }
	[[nodiscard]] static std::mutex& member_group_lock(_TMEMBER* _pmember) noexcept { return _pmember->m_cs_group;}
};

template <class _TMEMBER>
bool Igroup<_TMEMBER>::leave(_TMEMBER* _pmember, uintptr_t _param) noexcept
{
	// check)
	if(_pmember == nullptr)
		return false;

	// lock)
	std::lock_guard<std::mutex> cs(_pmember->m_cs_group);

	// check) 아에 Group에 속해 있지 않으면 그냥 끝낸다.
	if(_pmember->_get_group().get() != this)
		return false;

	// 1) 일단 Group에서 Leave를 먼저 한다.
	this->process_leave(_pmember, _param);

	// return) 
	return true;
}



}