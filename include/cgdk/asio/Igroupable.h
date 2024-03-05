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

template <class _TMEMBER, class _TGROUP=CGDK::asio::Igroup<_TMEMBER>>
class CGDK::asio::Igroupable
{
public:
	virtual	~Igroupable() noexcept { assert(this->m_pgroup.empty());}

	[[nodiscard]] std::shared_ptr<_TGROUP> get_group() noexcept  { std::lock_guard<std::mutex> cs(this->m_cs_group); return this->_get_group();	}
	[[nodiscard]] bool			is_member_of(const Igroup<_TMEMBER>* _pgroup) const noexcept { std::lock_guard<std::mutex> cs(this->m_cs_group); return this->_is_member_of(_pgroup); }
			void				leave_group() noexcept
			{
				// declare) 
				auto pgroup = this->get_group();

				// check) 아에 Group에 속해 있지 않으면 그냥 끝낸다.
				if (pgroup.empty())
					return;

				// 1) casting
				auto pmember_base = dynamic_cast<_TMEMBER*>(this);

				// check)
				assert(pmember_base != nullptr);

				// 2) 일단 Group에서 Leave를 먼저 한다.
				_TGROUP::request_leave(pgroup.get(), pmember_base);
			}
	[[nodiscard]] std::shared_ptr<_TGROUP> _get_group() const noexcept { return this->m_pgroup;}
	[[nodiscard]] bool			_is_member_of(const Igroup<_TMEMBER>* _pgroup) const noexcept { return this->m_pgroup.get() == _pgroup; }

private:
			std::shared_ptr<_TGROUP> m_pgroup;
			std::mutex			m_cs_group;

			template <class TSETGROUP>
			void				_set_group(std::shared_ptr<TSETGROUP>&& _pgroup) noexcept { this->m_pgroup = st::move(_pgroup); }
			void				_reset_group() noexcept { this->m_pgroup.reset();}

	// friend)
	friend	Igroup<_TMEMBER>;
};
