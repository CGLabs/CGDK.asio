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
 @class		Igroupable
*///-----------------------------------------------------------------------------
template <class _TMEMBERBASE, class _TGROUPBASE= Igroup<_TMEMBERBASE>>
class Igroupable : virtual public Ireferenceable
{
// declare) Member의 Type
public:
	using member_base_t = _TMEMBERBASE;
	using group_base_t = _TGROUPBASE;
	using member_set_t = group::MEMBER_SET<_TMEMBERBASE>;

// constructor/destructor)
public:
			Igroupable() noexcept : m_pgroup(), m_lockable_group(), m_pgroup_member_set(nullptr)	{}
	virtual	~Igroupable() noexcept { CGASSERT_ERROR(m_pgroup.empty()); CGASSERT_ERROR(m_pgroup_member_set == nullptr);}

// public)
public:
	//! @brief 현재 그룹을 얻는다. @return 현재 그룹
	[[nodiscard]] object_ptr<group_base_t> get_group() const noexcept  { std::lock_guard<lockable<>> cs(const_cast<lockable<>&>(this->m_lockable_group)); return this->_get_group();	}
	[[nodiscard]] bool			is_member_of(const Igroup<member_base_t>* _pgroup) const noexcept { std::lock_guard<lockable<>> cs(const_cast<lockable<>&>(this->m_lockable_group)); return this->_is_member_of(_pgroup); }

	//! @brief 현재 그룹에서 나온다.
			void				leave_group(uintptr_t _param = 0) noexcept;

	//! @brief 멤버 데이터를 얻는다. @return 그룹의 멤버 데이터
	[[nodiscard]] member_set_t*	member_set() const noexcept { return this->m_pgroup_member_set;}
			void				member_set(member_set_t* _pGMD) noexcept { this->m_pgroup_member_set = _pGMD;}
			void				reset_member_set() noexcept { this->m_pgroup_member_set = nullptr;}
	[[nodiscard]] size_t		member_serial() const noexcept { RETURN_IF(this->m_pgroup_member_set == nullptr, static_cast<size_t>(-1)); return this->m_pgroup_member_set->member_serial();}

	[[nodiscard]] object_ptr<group_base_t> _get_group() const noexcept { return this->m_pgroup;}
	[[nodiscard]] bool			_is_member_of(const Igroup<member_base_t>* _pgroup) const noexcept { return this->m_pgroup == _pgroup;}

// implementation)
private:
	// 1) Group(Memberable가 속해있는 Group이다. Group에 속해있지 않으면 nullptr이 된다.)
			object_ptr<group_base_t> m_pgroup;
			lockable<>			m_lockable_group;

	// 2) Group Member Data(Group에 종속된 Data)
			member_set_t*		m_pgroup_member_set;

	// 3) 붙어 있는 Group을 얻고 설정하는 함수.
	//    - set_group()과 ResetGroup()은 되도록 Group밖에서는 사용하지 않는 것이 바람직하다.
			template <class TSETGROUP>
			void				_set_group(TSETGROUP* _pgroup) noexcept { this->m_pgroup = _pgroup;}
			void				_reset_group() noexcept { this->m_pgroup.reset();}

	// Friend)
	friend	Igroup<member_base_t>;
};

template <class _TMEMBERBASE, class _TGROUPBASE>
void Igroupable<_TMEMBERBASE, _TGROUPBASE>::leave_group(uintptr_t _param) noexcept
{
	// declare) 
	auto pgroup = this->get_group();

	// check) 아에 Group에 속해 있지 않으면 그냥 끝낸다.
	RETURN_IF(pgroup.empty());

	// 1) casting
	auto pmember_base = dynamic_cast<member_base_t*>(this);

	// check)
	CGASSERT_ERROR(pmember_base != nullptr);

	// 2) 일단 Group에서 Leave를 먼저 한다.
	group_base_t::request_leave(pgroup.get(), pmember_base, _param);
}


}