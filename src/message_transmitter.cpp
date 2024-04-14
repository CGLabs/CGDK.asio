#include "cgdk/asio.h"


int CGDK::asio::message_transmitter::transmit_message(sMESSAGE& _msg)
{
	// declare) 
	std::vector<std::shared_ptr<Imessageable>> container;

	// 1) copy messages
	{
		std::lock_guard lock(this->m_lock_container);
		container = this->m_container;
	}

	// 2) dispatch message
	for (auto& iter : container)
	{
		iter->process_message(_msg);
	}

	return 1;
}

bool CGDK::asio::message_transmitter::register_messageable(const std::shared_ptr<CGDK::asio::Imessageable>& _pmessageable)
{
	return this->register_messageable(std::shared_ptr<CGDK::asio::Imessageable>(_pmessageable));
}

bool CGDK::asio::message_transmitter::register_messageable(std::shared_ptr<CGDK::asio::Imessageable>&& _pmessageable)
{
	// lock)
	std::lock_guard lock(this->m_lock_container);

	// 1) already exist
	auto iter_find = std::find(this->m_container.begin(), this->m_container.end(), _pmessageable);

	// check)
	if (iter_find != this->m_container.end())
		return false;

	// 2) add messageable
	this->m_container.push_back(std::move(_pmessageable));

	// return) 
	return true;
}

bool CGDK::asio::message_transmitter::unregister_messageable(CGDK::asio::Imessageable* _pmessageable) noexcept
{
	// lock)
	std::lock_guard lock(this->m_lock_container);

	// 1) find messageable and remove it!
	for (auto iter = this->m_container.begin(), iter_end = this->m_container.end(); iter != iter_end; ++iter)
	{
		if (iter->get() == _pmessageable)
		{
			this->m_container.erase(iter);
			return true;
		}
	}

	// return) return false
	return false;
}

int CGDK::asio::message_transmitter::reset_message_transmitter() noexcept
{
	return 0;
}
