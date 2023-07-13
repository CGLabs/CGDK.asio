//*****************************************************************************
//*                                                                           *
//*                      Cho sanghyun's Game Classes II                       *
//*                       Ver 10.0 / Release 2019.12.11                       *
//*                                                                           *
//*                           asio network classes                            *
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

class asio::Nconnective : public std::enable_shared_from_this<asio::Nconnective>
{
public:
			Nconnective();
	virtual ~Nconnective() noexcept;

	void send(const shared_buffer& _buffer) { return this->send(shared_buffer(_buffer)); }
	void send(shared_buffer&& _buffer);
	void close_connectable_all() noexcept;
	void disconnect_connectable_all() noexcept;
	void destroy_connectable_all() noexcept;

	auto& get_lockable() noexcept { return m_lockable_list_sockets; }
	auto count() const noexcept { return m_list_sockets.size(); }
	auto begin() noexcept { return m_list_sockets.begin(); }
	auto end() noexcept { return m_list_sockets.end(); }

protected:
	void process_register_socket(const std::shared_ptr<Isocket_tcp>& _psocket);
	void process_unregister_socket(const std::shared_ptr<Isocket_tcp>& _psocket) noexcept;

private:
			std::recursive_mutex m_lockable_list_sockets;
			std::set<std::shared_ptr<Isocket_tcp>> m_list_sockets;
	friend class Isocket_tcp;
};