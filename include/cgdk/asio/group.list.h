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

template <class _TMEMBER, class _TPARAM = sMESSAGE>
class group::list : virtual public Igroup<_TMEMBER>
{
public:
	using member_t			 = typename Igroup<_TMEMBER>::member_t;
	using container_t		 = std::list<std::shared_ptr<member_t>>;
	using iterator_t		 = typename container_t::iterator;
	using const_iterator_t	 = typename container_t::const_iterator;
	using param_t			 = _TPARAM;

	virtual	~list() noexcept { this->leave_all(); }

			int64_t				enter(std::shared_ptr<member_t>&& _pmember, param_t& _param) { return this->process_enter(std::move(_pmember), _param); }
			int64_t				enter(std::shared_ptr<member_t>&& _pmember) { param_t temp; return this->process_enter(std::move(_pmember), temp); }
			template <class T>
			int64_t				enter(std::shared_ptr<T>&& _pmember, param_t& _param) { return this->process_enter(dynamic_pointer_cast<member_t>(std::move(_pmember)), _param); }
			template <class T>
			int64_t				enter(std::shared_ptr<T>&& _pmember) { param_t temp; return this->process_enter(dynamic_pointer_cast<member_t>(std::move(_pmember)), temp); }
			void				leave_all() noexcept;
	[[nodiscard]] std::mutex&	member_enter_lock() noexcept { return this->m_lockable_enter;}

	[[nodiscard]] auto			begin() noexcept { return this->m_container_member.begin();}
	[[nodiscard]] auto			begin() const noexcept { return this->m_container_member.begin();}
	[[nodiscard]] auto			end() noexcept { return this->m_container_member.end();}
	[[nodiscard]] auto			end() const noexcept { return this->m_container_member.end();}

	virtual	bool				for_each_member(const std::function<bool (const std::shared_ptr<member_t>&)>& _function) override;
	[[nodiscard]] virtual std::shared_ptr<member_t> find_member(const std::function<bool(const std::shared_ptr<member_t>&)>& _pred) override;
	template <class T>
			bool				send(T& _buf);

protected:
	virtual	void				on_enable_enter() {}
	virtual	void				on_disable_enter() {}
	virtual	int64_t				on_member_entering(member_t* /*_pmember*/, param_t& /*_param*/) { return 0;}
	virtual	void				on_member_entered(member_t* /*_pmember*/, param_t& /*_param*/) {}
	virtual	uintptr_t			on_member_leaving(member_t* /*_pmember*/, uintptr_t _param) { return _param;}
	virtual	void				on_member_leaved(member_t* /*_pmember*/, uintptr_t /*_result*/) {}

			bool				process_enable_member_enter(bool _enable);
			int64_t				process_enter(std::shared_ptr<member_t>&& _pmember, param_t& _param);
	virtual	void				process_leave(member_t* _pmember, uintptr_t _param) noexcept override;
			void				_process_attach_member(std::shared_ptr<member_t> _pmember);
			void				_process_detach_member(member_t* _pmember) noexcept;

			bool				_is_member_full() const noexcept { return this->m_container_member.size()>=this->m_count_max_member; }

			bool				m_enable_enter {true};
			std::mutex			m_lockable_enter;
			container_t			m_container_member;
			size_t				m_count_max_member{SIZE_MAX};
};

template <class _TMEMBER, class _TPARAM>
bool group::list<_TMEMBER, _TPARAM>::process_enable_member_enter(bool _enable)
{
	// lock) 
	std::lock_guard lock_enter(this->m_lockable_enter);

	// check) 
	if(this->m_enable_enter == _enable)
		return false;

	// 1) set value
	this->m_enable_enter = _enable;

	// 2) call 'on_enable_enter'
	if (_enable)
		this->on_enable_enter();
	else
		this->on_disable_enter();

	// return) 
	return true;
}

template <class _TMEMBER, class _TPARAM>
int64_t group::list<_TMEMBER, _TPARAM>::process_enter(std::shared_ptr<member_t>&& _pmember, param_t& _param)
{
	// check)
	if(_pmember == nullptr)
		return 1;

	// lock) 
	std::lock_guard lock_enter(this->m_lockable_enter);

	// check) enable enter?
	if (this->m_enable_enter == false)
		return 0; // eRESULT::FAIL_DISABLED;

	// check) aleady entered?
	if(_pmember->_is_member_of(this) == true)
		return 0; // eRESULT::ALEADY_EXIST;

	// check) is full?
	if(this->_is_member_full())
		return 0; // eRESULT::MEMBER_FULL;

	// 1) call 'on_member_entering'
	auto result = this->on_member_entering(_pmember.get(), _param);

	// check) failed?
	if(result != 0) // eRESULT::SUCCESS
		return result;

	// 2) leave from entered group
	_pmember->leave_group();

	// lock) 
	std::lock_guard lock_group(this->m_cs_group);

	// 3) attach to list
	this->_process_attach_member(_pmember);

	// 4) call 'on_member_entered'
	try
	{
		this->on_member_entered(_pmember.get(), _param);
	}
	catch (...)
	{
		// - rollback(detach member)
		this->_process_detach_member(_pmember.get());

		// reraise) 
		throw;
	}

	// return)
	return 1;
}

template <class _TMEMBER, class _TPARAM>
void group::list<_TMEMBER, _TPARAM>::process_leave(member_t* _pmember, uintptr_t _param) noexcept
{
	// check)
	assert(_pmember != nullptr);

	// declare) 
	uintptr_t result = 0;

	{
		std::lock_guard lock_group(this->m_cs_group);

		// check)
		if (_pmember->_is_member_of(this) == false)
			return;

		// 1) call processMemberLeaving()
		try
		{
			result = this->on_member_leaving(_pmember, _param);
		}
		catch (...)
		{
			// result = static_cast<uintptr_t>(eRESULT::FAIL);
		}

		// 2) detach
		this->_process_detach_member(_pmember);
	}

	// 3) call 'on_member_leaved'
	try
	{
		this->on_member_leaved(_pmember, result);
	}
	catch (...)
	{
		//// log) 
		//LOG_FATAL << "(excp) occure exception in 'on_meber_leaved' function on group"sv;
	}
}

template <class _TMEMBER, class _TPARAM>
void group::list<_TMEMBER, _TPARAM>::_process_attach_member(std::shared_ptr<member_t> _pmember)
{
	// 1) attach
	this->m_container_member.push_back(_pmember);

	// 2) set iterator to member
	Igroup<_TMEMBER>::member_put_iter(_pmember.get(), --this->m_container_member.end());

	// 3) member group
	Igroup<_TMEMBER>::member_group_as(std::dynamic_pointer_cast<Igroup<member_t>>(this->shared_from_this()), _pmember.get());
}

template <class _TMEMBER, class _TPARAM>
void group::list<_TMEMBER, _TPARAM>::_process_detach_member(member_t* _pmember) noexcept
{
	// 1) get iterator
	auto iter = Igroup<_TMEMBER>::member_get_iter<iterator_t>(_pmember);

	// 2) erase from list
	this->m_container_member.erase(iter);

	// 3) reset member group info
	this->Igroup<_TMEMBER>::member_reset_group(_pmember);
}

template <class _TMEMBER, class _TPARAM>
void group::list<_TMEMBER, _TPARAM>::leave_all() noexcept
{
	// lock)
	std::lock_guard cs(this->m_cs_group);

	for (auto& iter : this->m_container_member)
		iter->leave_group();
}


template <class _TMEMBER, class _TPARAM>
bool group::list<_TMEMBER, _TPARAM>::for_each_member(const std::function<bool(const std::shared_ptr<member_t>&)>& _function)
{
	std::lock_guard lock(this->m_cs_group);

	for (auto& iter : this->m_container_member)
		if (!_function(iter))
			return false;

	return true;
}

template <class _TMEMBER, class _TPARAM>
std::shared_ptr<typename Igroup<_TMEMBER>::member_t> group::list<_TMEMBER, _TPARAM>::find_member(const std::function<bool(const std::shared_ptr<member_t>&)>& _pred)
{
	std::lock_guard lock(this->m_cs_group);

	for (auto iter : this->m_container_member)
		if (_pred(iter))
			return iter;

	return {};
}

template <class _TMEMBER, class _TPARAM>
template <class T>
bool group::list<_TMEMBER, _TPARAM>::send(T& _buf)
{
	std::lock_guard lock(this->m_cs_group);

	for (auto iter : this->m_container_member)
			iter->send(_buf);

	return true;
}


}