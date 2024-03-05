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
template <class _TMEMBERBASE>
class Igroup :
	public						group_taits,
	virtual public				Ireferenceable
{
public:
	using member_base_t	 = _TMEMBERBASE;
	using group_t		 = typename _TMEMBERBASE::group_base_t;

public:
			result_code			leave(member_base_t* _pmember) noexcept;
			result_code			leave(member_base_t* _pmember, uintptr_t _param) noexcept;

	[[nodiscard]] object_ptr<group_t> parent_group() const noexcept { std::lock_guard<lockable<>> cs(const_cast<lockable<>&>(this->m_lockable_group)); return this->m_pgroup_parent; }
			void				parent_group(group_t* _pgroup_parent) noexcept { std::lock_guard<lockable<>> cs(const_cast<lockable<>&>(this->m_lockable_group)); this->m_pgroup_parent = _pgroup_parent; }

			void				lock_group() { add_ref(); this->m_lockable_group.lock();}
			void				unlock_group() noexcept { this->m_lockable_group.unlock(); release();}
	[[nodiscard]] lockable<>&	get_group_lockable() noexcept { return this->m_lockable_group;}
	[[nodiscard]] const lockable<>&	get_group_lockable() const noexcept { return this->m_lockable_group;}
	[[nodiscard]] 				operator lockable<>&() noexcept { return this->m_lockable_group;}
	[[nodiscard]] 				operator const lockable<>&() const noexcept { return this->m_lockable_group;}

protected:
	virtual	void				process_leave(member_base_t* _pmember, uintptr_t _param) noexcept PURE;

protected:
			lockable<>			m_lockable_group;
			object_ptr<group_t>	m_pgroup_parent;
 
protected:
	template <class group_t>
	static	void				member_group_as(group_t* _pgroup, member_base_t* _pmember) noexcept	{ _pmember->_set_group(_pgroup);}
	static	void				reset_member_group(member_base_t* _pmember) noexcept { _pmember->_reset_group();}
	[[nodiscard]] static lockable<>& member_group_lock(member_base_t* _pmember) noexcept { return _pmember->m_lockable_group;}

public:
	static	void				request_leave(group_t* _pgroup, member_base_t* _pmember, uintptr_t _param) noexcept;
};

template <class _TMEMBERBASE>
result_code Igroup<_TMEMBERBASE>::leave(member_base_t* _pmember) noexcept
{
	// declare) 
	uintptr_t param = 0;

	// return) 
	return this->leave(_pmember, param);
}

template <class _TMEMBERBASE>
result_code Igroup<_TMEMBERBASE>::leave(member_base_t* _pmember, uintptr_t _param) noexcept
{
	// check)
	RETURN_IF(_pmember == nullptr, eRESULT::INVALID_ARGUMENT);

	scoped_lock(_pmember->m_lockable_group)
	{
		// check) 아에 Group에 속해 있지 않으면 그냥 끝낸다.
		RETURN_IF(_pmember->_get_group() != this, eRESULT::INVALID_ARGUMENT);

		// 1) 일단 Group에서 Leave를 먼저 한다.
		this->process_leave(_pmember, _param);
	}

	// return) 
	return eRESULT::SUCCESS;
}

template <class _TMEMBERBASE>
void Igroup<_TMEMBERBASE>::request_leave(group_t* _pgroup, member_base_t* _pmember, uintptr_t _param) noexcept
{
	// check)
	CGASSERT_ERROR(_pgroup != nullptr); 

	// check)
	RETURN_IF(_pgroup == nullptr);

	// 1) Process...
	((Igroup<_TMEMBERBASE>*)_pgroup)->process_leave(_pmember, _param);
}


}