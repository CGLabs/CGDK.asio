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
namespace CGDK
{

template <class _TMEMBER, class _TPARAM, class _TMEMBER_DATA, class _TMEMBERBASE>
group::list<_TMEMBER, _TPARAM, _TMEMBER_DATA, _TMEMBERBASE>::list() noexcept :
	m_enable_enter(false)
{
	this->process_set_member_capacity(SIZE_MAX);
}

template <class _TMEMBER, class _TPARAM, class _TMEMBER_DATA, class _TMEMBERBASE>
group::list<_TMEMBER, _TPARAM, _TMEMBER_DATA, _TMEMBERBASE>::list(size_t _max_member) noexcept :
	m_enable_enter(false)
{
	this->process_set_member_capacity(_max_member);
}

template <class _TMEMBER, class _TPARAM, class _TMEMBER_DATA, class _TMEMBERBASE>
group::list<_TMEMBER, _TPARAM, _TMEMBER_DATA, _TMEMBERBASE>::~list() noexcept
{
	this->leave_all();
}

template <class _TMEMBER, class _TPARAM, class _TMEMBER_DATA, class _TMEMBERBASE>
void group::list<_TMEMBER, _TPARAM, _TMEMBER_DATA, _TMEMBERBASE>::process_set_member_capacity(size_t _max_count)
{
	// locked) 
	{
		// scoped_lock)
		std::lock_guard<lockable<>> cs(const_cast<lockable<>&>(this->m_lockable_group));

		// 1) 최대 Member수를 설정한다.
		this->m_count_max_member = _max_count;
	
		// 2) Reserve한다.(Reserver함수가 있을 경우에만...)
	#if defined(_MSC_VER)
		__if_exists(container_t::reserve)
		{
			this->m_container_member.reserve((_max_count>256) ? 256:_max_count);
		}
	#endif
	}
}

template <class _TMEMBER, class _TPARAM, class _TMEMBER_DATA, class _TMEMBERBASE>
typename group::list<_TMEMBER, _TPARAM, _TMEMBER_DATA, _TMEMBERBASE>::member_data_t& group::list<_TMEMBER, _TPARAM, _TMEMBER_DATA, _TMEMBERBASE>::member_data(const member_t* _pmember)
{
	// return) 현재 Member Data를 즉시 Return한다.		
	return ((member_set_t*)_pmember->member_set())->m_member_data;
}

template <class _TMEMBER, class _TPARAM, class _TMEMBER_DATA, class _TMEMBERBASE>
object_ptr<_TMEMBERBASE> group::list<_TMEMBER, _TPARAM, _TMEMBER_DATA, _TMEMBERBASE>::find_member(const std::function<bool (_TMEMBERBASE*)>& _pred)
{
	scoped_lock(this->m_lockable_group)
	{
		for(member_set_t& iter:m_container_member)
		{
			// 1) execute함수를 실행한다.
			bool result = _pred(*iter);

			// check) 
			RETURN_IF(result == true, *iter);
		}
	}

	// return) 찾지 못했다..
	return	object_ptr<member_t>();
}

template <class _TMEMBER, class _TPARAM, class _TMEMBER_DATA, class _TMEMBERBASE>
bool group::list<_TMEMBER, _TPARAM, _TMEMBER_DATA, _TMEMBERBASE>::for_each_member(const std::function<bool (_TMEMBERBASE*)>& _function)
{
	scoped_lock(this->m_lockable_group)
	{
		for(member_set_t& iter:this->m_container_member)
		{
			// 1) execute함수를 실행한다.
			bool result = _function(iter.member());

			// check) 
			RETURN_IF(result == false, false);
		}
	}

	// return) 성공...
	return true;
}

template <class _TMEMBER, class _TPARAM, class _TMEMBER_DATA, class _TMEMBERBASE>
void group::list<_TMEMBER, _TPARAM, _TMEMBER_DATA, _TMEMBERBASE>::_attach_member_to_list_head(member_t* _pmember)
{
	// 1) attach를 한다.
	this->push_front(member_set_t(this));

	// 2) 값을 읽어온다.
	auto& rSeat = this->m_container_member.front();

	// 3) detach를 위해 저장해 놓는다.
	rSeat.member(_pmember);
	rSeat.iter = this->m_container_member.begin();

	// 4) Seat Info를 Push하낟.
	_pmember->member_set(&rSeat);

	// 5) Object을 설정!
	this->member_group_as((typename _TMEMBER::group_base_t*)this, _pmember);
}

template <class _TMEMBER, class _TPARAM, class _TMEMBER_DATA, class _TMEMBERBASE>
void group::list<_TMEMBER, _TPARAM, _TMEMBER_DATA, _TMEMBERBASE>::_attach_member_to_list_tail(member_t* _pmember)
{
#if defined(_MSC_VER)
	__if_exists(container_t::attach)
#endif
	{
		// 1) attach를 한다.
		this->m_container_member.push_back(member_set_t(this));

		// 2) 값을 읽어온다.
		auto& rSeat = this->m_container_member.back();

		// 3) 값을 설정한다.
		rSeat.member(_pmember);
		rSeat.iter = --this->m_container_member.end();

		// 4) Seat Info를 Push하낟.
		_pmember->member_set(&rSeat);
	}

	// 5) Object을 설정!
	this->member_group_as((typename _TMEMBER::group_base_t*)this, _pmember);
}

template <class _TMEMBER, class _TPARAM, class _TMEMBER_DATA, class _TMEMBERBASE>
void group::list<_TMEMBER, _TPARAM, _TMEMBER_DATA, _TMEMBERBASE>::_detach_member(member_base_t* _pmember) noexcept
{
#if defined(_MSC_VER)
	__if_exists(container_t::detach)
#endif
	{
		// 1) pMemberData를 얻어낸다.
		auto* pmember_set = (member_set_t*)_pmember->member_set();

		// 2) Object을 때낸다.
		this->m_container_member.erase(pmember_set->iter);

		// 3) seat Info를 pop한다.
		_pmember->reset_member_set();
	}

	// 4) obejct group를 nullptr로 설정한다.
	Igroup<_TMEMBERBASE>::reset_member_group(_pmember);
}

template <class _TMEMBER, class _TPARAM, class _TMEMBER_DATA, class _TMEMBERBASE>
bool group::list<_TMEMBER, _TPARAM, _TMEMBER_DATA, _TMEMBERBASE>::process_is_enable_member_enter() const noexcept
{
	// locked) 
	{
		// scoped_lock) 
		std::lock_guard<lockable<>> cs(const_cast<lockable<>&>(this->m_lockable_enter));

		// return) 
		return this->m_enable_enter;
	}
}

template <class _TMEMBER, class _TPARAM, class _TMEMBER_DATA, class _TMEMBERBASE>
bool group::list<_TMEMBER, _TPARAM, _TMEMBER_DATA, _TMEMBERBASE>::process_enable_member_enter(bool _enable)
{
	scoped_lock(this->m_lockable_enter)
	{
		// check) 
		RETURN_IF(this->m_enable_enter == _enable, false);

		// 1) 값을 설정
		this->m_enable_enter = _enable;

		// 2) OnOpen을 호출한다.
		if(_enable)
		{
			this->on_enable_enter();
		}
		else
		{
			this->on_disable_enter();
		}
	}

	// return) 
	return true;
}

template <class _TMEMBER, class _TPARAM, class _TMEMBER_DATA, class _TMEMBERBASE>
result_code group::list<_TMEMBER, _TPARAM, _TMEMBER_DATA, _TMEMBERBASE>::process_enter(member_base_t* _pmember, param_t& _param)
{
	// check) _pmember가 nullptr인가?
	RETURN_IF(_pmember == nullptr, eRESULT::INVALID_ARGUMENT);

	scoped_lock(this->member_group_lock(_pmember))
	scoped_lock(this->m_lockable_enter)
	{
		// check) 이미 Member인가?
		RETURN_IF(_pmember->_is_member_of(this) == true, eRESULT::ALEADY_EXIST);

		// check) Enter가능한 상태인가?
		RETURN_IF(this->m_enable_enter == false, eRESULT::FAIL_DISABLED);

		// check) 꽉 찼는가?
		RETURN_IF(this->is_member_full(), eRESULT::MEMBER_FULL);

		// 1) _process_member_entering()함수를 호출한다.
		auto result = this->_process_member_entering(_pmember, _param);

		// check) 입장 조건에 맞지 않는다!
		RETURN_IF(result != eRESULT::SUCCESS, result);
		
		// 2) 이전 Group에서 떼낸다.
		_pmember->leave_group();

		scoped_lock(this->m_lockable_group)
		{
			// 3) attach한다.
			this->_attach_member(_pmember);

			// 4) on_member_entered()함수를 호출한다. 
			try
			{
				this->_process_member_entered(_pmember, _param);
			}
			// exception) rollback한다.
			catch(std::exception& _e)
			{
				// trace)
				LOG_FATAL << "(excp) exception complete[on_member_entered]"sv;

				// - exceeption complete
				auto is_executed = throwable::execute_exception_handler(this, _e, __FUNCTION__);

				// - Member를 떼낸다.
				this->_detach_member(_pmember);

				// check) throwable::execute_exception_handler함수에서 처리된 것으로 설정되었다면 reraise하지 않는다.
				RETURN_IF(is_executed, eRESULT::EXCEPTION);

				// reraise) 
				throw;
			}
			// exception) rollback한다.
			catch (...)
			{
				// trace)
				LOG_FATAL << "(excp) exception complete[on_member_entered]"sv;

				// - exceeption complete
				auto is_executed = throwable::execute_exception_handler(this, __FUNCTION__);

				// - Member를 떼낸다.
				this->_detach_member(_pmember);

				// check) throwable::execute_exception_handler함수에서 처리된 것으로 설정되었다면 reraise하지 않는다.
				RETURN_IF(is_executed, eRESULT::EXCEPTION);

				// reraise) 
				throw;
			}
		}
	}
	
	// return) 성공이닷!!!
	return	eRESULT::SUCCESS;
}

template <class _TMEMBER, class _TPARAM, class _TMEMBER_DATA, class _TMEMBERBASE>
void group::list<_TMEMBER, _TPARAM, _TMEMBER_DATA, _TMEMBERBASE>::leave_all() noexcept
{
	// locked) 
	{
		// scoped_lock)
		std::lock_guard<lockable<>> cs(const_cast<lockable<>&>(this->m_lockable_group));

		for(auto& iter:this->m_container_member)
		{
			iter->leave_group();
		}
	}
}

template <class _TMEMBER, class _TPARAM, class _TMEMBER_DATA, class _TMEMBERBASE>
void group::list<_TMEMBER, _TPARAM, _TMEMBER_DATA, _TMEMBERBASE>::process_leave(member_base_t* _pmember, uintptr_t _param) noexcept
{
	// check) _pmember가 nullptr이어서는 안됀다.
	CGASSERT_ERROR(_pmember != nullptr);

	scoped_lock(this->member_group_lock(_pmember))
	{
		// declare) 
		uintptr_t result = 0;

		scoped_lock(this->m_lockable_group)
		{
			// check) _pmember의 Group이 똑같지 않은가?
			RETURN_IF(_pmember->_is_member_of(this) == false);

			// 1) ProcessMemberLeaving()함수를 호출한다.
			result = this->_process_member_leaving(_pmember, _param);

			// 2) detach한다.
			this->_detach_member(_pmember);
		}

		// 3) _process_member_leaved()함수를 호출한다.
		try
		{
			this->_process_member_leaved(_pmember, result);
		}
		// exception) 
		catch(std::exception& _e)
		{
			// log) 
			LOG_FATAL << "(excp) occure exception in 'on_meber_leaved' function on group"sv;

			// - exceeption complete
			throwable::execute_exception_handler(this, _e, __FUNCTION__);
		}
		// exception) 
		catch (...)
		{
			// log) 
			LOG_FATAL << "(excp) occure exception in 'on_meber_leaved' function on group"sv;

			// - exceeption complete
			throwable::execute_exception_handler(this, __FUNCTION__);
		}
	}
}

template <class _TMEMBER, class _TPARAM, class _TMEMBER_DATA, class _TMEMBERBASE>
result_code group::list<_TMEMBER, _TPARAM, _TMEMBER_DATA, _TMEMBERBASE>::_process_member_entering(member_t* _pmember, param_t& _param)
{
	// 1) on_member_entering함수를 호출한다.
	return	this->on_member_entering(_pmember, _param);
}

template <class _TMEMBER, class _TPARAM, class _TMEMBER_DATA, class _TMEMBERBASE>
void group::list<_TMEMBER, _TPARAM, _TMEMBER_DATA, _TMEMBERBASE>::_process_member_entered(member_t* _pmember, param_t& _param)
{
	// 1) on_member_entering함수를 호출한다.
	this->on_member_entered(_pmember, _param);
}

template <class _TMEMBER, class _TPARAM, class _TMEMBER_DATA, class _TMEMBERBASE>
uintptr_t group::list<_TMEMBER, _TPARAM, _TMEMBER_DATA, _TMEMBERBASE>::_process_member_leaving(member_t* _pmember, uintptr_t _param) noexcept
{
	// declare)
	uintptr_t result = 0;

	try
	{
		// 1) on_member_leaving함수를 호출한다.
		result = this->on_member_leaving(_pmember, _param);
	}
	catch(...)
	{
		result = static_cast<uintptr_t>(eRESULT::FAIL);
	}

	// return)
	return result;
}

template <class _TMEMBER, class _TPARAM, class _TMEMBER_DATA, class _TMEMBERBASE>
void group::list<_TMEMBER, _TPARAM, _TMEMBER_DATA, _TMEMBERBASE>::_process_member_leaved(member_t* _pmember, uintptr_t _result) noexcept
{
	try
	{
		// 1) on_member_leaved함수를 호출한다.
		this->on_member_leaved(_pmember, _result);
	}
	catch (...)
	{
	}
}


}