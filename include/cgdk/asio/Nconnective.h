﻿//*****************************************************************************
//*                                                                           *
//*                      Cho sanghyun's Game Classes II                       *
//*                                                                           *
//*                          asio.ex network classes                          *
//*                                                                           *
//*                                                                           *
//*                                                                           *
//*                                                                           *
//*  This Program is programmed by Cho SangHyun. sangduck@cgcii.co.kr         *
//*  Best for Game Developement and Optimized for Game Developement.          *
//*                                                                           *
//*                (c) 2019. Cho Sanghyun. All right reserved.                *
//*                          http://www.CGCII.co.kr                           *
//*                                                                           *
//*****************************************************************************
#pragma once

class CGDK::asio::Nconnective : public std::enable_shared_from_this<asio::Nconnective>
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
			void process_unregister_socket(Isocket_tcp* _psocket) noexcept;

private:
			struct compare 
			{
				using is_transparent = void;
				bool operator() (const std::shared_ptr<Isocket_tcp>& _lhs, const std::shared_ptr<Isocket_tcp>& _rhs) const { return _lhs.get() < _rhs.get(); }
				bool operator() (const std::shared_ptr<Isocket_tcp>& _lhs, const Isocket_tcp* _rhs) const { return _lhs.get() < _rhs; }
				bool operator() (const Isocket_tcp* _lhs, const std::shared_ptr<Isocket_tcp>& _rhs) const { return _lhs < _rhs.get(); }
			};			std::recursive_mutex m_lockable_list_sockets;
			std::set<std::shared_ptr<Isocket_tcp>, compare> m_list_sockets;
	friend class Isocket_tcp;
};