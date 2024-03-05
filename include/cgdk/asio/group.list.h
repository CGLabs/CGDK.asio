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

#include "cgdk/sdk10/server.system/group/server.group.base.h"

namespace CGDK
{
//-----------------------------------------------------------------------------
/**
 @class		list
*///-----------------------------------------------------------------------------
template <class _TMEMBER, class _TPARAM = sMESSAGE, class _TMEMBER_DATA = int, class _TMEMBERBASE = _TMEMBER>
class group::list : 
	virtual public				Igroup_countable<_TMEMBERBASE, _TPARAM>,
	virtual public				Imember_containerable<_TMEMBERBASE>
{
public:
	using group_base_t		 = Igroup_countable<_TMEMBERBASE, _TPARAM>;
	using member_t			 = _TMEMBER;
	using member_base_t		 = _TMEMBERBASE;
	using member_data_t		 = _TMEMBER_DATA;
	using member_set_t		 = MEMBER_SET_LIST<member_t, member_data_t>;
	using param_t			 = _TPARAM;
	using container_t		 = circular_list<member_set_t>;
	using iterator_t		 = typename container_t::iterator;
	using const_iterator_t	 = typename container_t::const_iterator;

public:
			list() noexcept;
			list(size_t _max_member) noexcept;
	virtual	~list() noexcept;

public:
			void				leave_all() noexcept;

	[[nodiscard]] static member_data_t& member_data(const member_t* _pmember);
	[[nodiscard]] static group_member_id_t member_serial(const member_t* _pmember) { return ((member_set_t*)_pmember->member_set())->member_serial();}
	static	void				member_serial(member_t* _pmember, group_member_id_t _seat) { ((member_set_t*)_pmember->member_set())->member_serial(_seat);}
	static	void				reset_member_serial(member_t* _pmember)	{ ((member_set_t*)_pmember->member_set())->reset_member_serial();}

			void				lock_member_enter() noexcept { Ireferenceable::add_ref(); m_lockable_enter.lock();}
			void				unlock_member_enter() noexcept { m_lockable_enter.unlock(); Ireferenceable::release();}
	[[nodiscard]] lockable<>&	member_enter_lock() noexcept { return m_lockable_enter;}

	[[nodiscard]] iterator_t	begin() noexcept { return this->m_container_member.begin();}
	[[nodiscard]] const_iterator_t begin() const noexcept { return this->m_container_member.begin();}
	[[nodiscard]] iterator_t	end() noexcept { return this->m_container_member.end();}
	[[nodiscard]] const_iterator_t end() const noexcept { return this->m_container_member.end();}

	[[nodiscard]] virtual object_ptr<_TMEMBERBASE> find_member(const std::function<bool (_TMEMBERBASE*)>& _pred) override;

	virtual	bool				for_each_member(const std::function<bool (_TMEMBERBASE*)>& _function) override;

protected:
	virtual	void				on_enable_enter() {}
	virtual	void				on_disable_enter() {}

	virtual	result_code			on_member_entering(member_t* /*_pmember*/, param_t& /*_param*/) { return eRESULT::SUCCESS;}
	virtual	void				on_member_entered(member_t* /*_pmember*/, param_t& /*_param*/) {}
	virtual	uintptr_t			on_member_leaving(member_t* /*_pmember*/, uintptr_t _param) { return _param;}
	virtual	void				on_member_leaved(member_t* /*_pmember*/, uintptr_t /*_result*/) {}

protected:
	virtual bool				process_is_enable_member_enter() const noexcept override;
	virtual	bool				process_enable_member_enter(bool _enable) override;
	virtual result_code			process_enter(member_base_t* _pmember, param_t& _param) override;
	virtual	void				process_leave(member_base_t* _pmember, uintptr_t _param) noexcept override;

	[[nodiscard]] virtual size_t process_get_member_count() const noexcept override { return this->m_container_member.size(); }
	virtual	void				process_set_member_capacity(size_t _max_count) override;
	[[nodiscard]] virtual size_t process_get_member_capacity() const noexcept override { return this->m_count_max_member;}

private:
			void				_attach_member (member_t* _pmember) { _attach_member_to_list_tail(_pmember);}
			void				_attach_member_to_list_head (member_t* _pmember);
			void				_attach_member_to_list_tail (member_t* _pmember);
			void				_detach_member (member_base_t* _pmember) noexcept;

	virtual	result_code			_process_member_entering (member_t* _pmember, param_t& _param);
	virtual	void				_process_member_entered (member_t* _pmember, param_t& _param);
	virtual	uintptr_t			_process_member_leaving (member_t* _pmember, uintptr_t _param) noexcept;
	virtual	void				_process_member_leaved (member_t* _pmember, uintptr_t _result) noexcept;

protected:
			bool				m_enable_enter;
			lockable<>			m_lockable_enter;

			container_t			m_container_member;
			size_t				m_count_max_member;
};


}

#include "cgdk/sdk10/server.system/group/server.group.list.inl"
