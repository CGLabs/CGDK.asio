#include "asio.h"


asio::Nconnective::Nconnective()
{
}

asio::Nconnective::~Nconnective() noexcept
{
}

void asio::Nconnective::send(shared_buffer&& _buffer)
{
	// lock) 
	std::unique_lock lock(this->m_lockable_list_sockets);

	// 1) erase socket
	for (auto iter = this->m_list_sockets.begin(); iter != this->m_list_sockets.end(); ++iter)
	{
		(*iter)->send(_buffer);
	}
}

void asio::Nconnective::close_connectable_all() noexcept
{
	// 1) copy sockets
	std::vector<std::shared_ptr<Isocket_tcp>> vector_socket;
	{
		std::unique_lock lock(this->m_lockable_list_sockets);
		vector_socket.reserve(this->m_list_sockets.size());
		vector_socket.assign(this->m_list_sockets.begin(), this->m_list_sockets.end());
	}

	// 2) closesocket
	for (auto& iter : vector_socket)
	{
		iter->closesocket();
	}
}

void asio::Nconnective::disconnect_connectable_all() noexcept
{
	// 1) copy sockets
	std::vector<std::shared_ptr<Isocket_tcp>> vector_socket;
	{
		std::unique_lock lock(this->m_lockable_list_sockets);
		vector_socket.reserve(this->m_list_sockets.size());
		vector_socket.assign(this->m_list_sockets.begin(), this->m_list_sockets.end());
	}

	// 2) disconnect
	for (auto& iter : vector_socket)
	{
		iter->disconnect();
	}
}

void asio::Nconnective::process_register_socket(const std::shared_ptr<Isocket_tcp>& _psocket)
{
	std::unique_lock lock(this->m_lockable_list_sockets);

	// 1) insert socket
	this->m_list_sockets.insert(_psocket);

	// 2) get connective
	try
	{
		// lock) 
		std::lock_guard cs(_psocket->m_lock_socket);

		// check)
		assert(!_psocket->m_pconnective);

		// check) 
		if (_psocket->m_pconnective)
			throw std::runtime_error("socket aleady have connective");

		_psocket->m_pconnective = this->shared_from_this();;
	}
	catch (...)
	{
		// erase) 
		this->m_list_sockets.erase(_psocket);

		// reraise) 
		throw;
	}
}

void asio::Nconnective::process_unregister_socket(const std::shared_ptr<Isocket_tcp>& _psocket) noexcept
{
	std::unique_lock lock(this->m_lockable_list_sockets);

	// check)
	{
		// lock) 
		std::lock_guard cs(_psocket->m_lock_socket);

		if (_psocket->m_pconnective.get() != this)
			return;

		// 2) reset socket
		_psocket->m_pconnective.reset();
	}

	// 1) erase socket
	this->m_list_sockets.erase(_psocket);
}

void asio::Nconnective::destroy_connectable_all() noexcept
{
	// declare) 
	std::set<std::shared_ptr<Isocket_tcp>> list_sockets;

	// 1) move sockets
	{
		// - lock
		std::unique_lock lock(this->m_lockable_list_sockets);

		// - copy
		list_sockets = this->m_list_sockets;
	}

	// 2) destroy sockets
	for (auto& iter : list_sockets)
		iter->closesocket();
}
