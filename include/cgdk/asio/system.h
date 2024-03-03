//*****************************************************************************
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

class CGDK::asio::system
{
public:
	~system() noexcept;

	template <typename T> struct is_shared_ptr : std::true_type {};
	template <typename P> struct is_shared_ptr<std::shared_ptr<P>> : std::false_type {};

			template <class T>
			static std::enable_if_t<is_shared_ptr<std::remove_pointer_t<std::decay_t<T>>>::value, void>
			post(T&& _completor) 
			{
				get_instance()->process_post(std::forward<T>(_completor)); 
			}
			template <class T>
			static std::enable_if_t<std::is_base_of_v<Iexecutable, T>, void>
			post(std::shared_ptr<T>&& _pexecutable)
			{
				struct TX
				{
					std::shared_ptr<Iexecutable> pexecutable;
					void operator ()() { pexecutable->process_execute(); }
				};
				get_instance()->process_post(TX{std::move(_pexecutable)});
			}
			template <class T>
			static std::enable_if_t<std::is_base_of_v<CGDK::asio::Iexecutable, T>, void>
			post(const std::shared_ptr<T>& _pexecutable) 
			{
				post(std::shared_ptr<T>(_pexecutable));
			}
			template <class T>
			static std::enable_if_t<is_shared_ptr<std::remove_pointer_t<std::decay_t<T>>>::value, void>
			post(T&& _completor, clock::time_point _tick_execute) { get_instance()->process_post(std::forward<T>(_completor), _tick_execute); }
			template <class T>
			static std::enable_if_t<std::is_base_of_v<Iexecutable, T>, void>
			post(std::shared_ptr<T>&& _pexecutable, clock::time_point _tick_execute) { get_instance()->m_pexecutor_single->post_at(std::move(_pexecutable), _tick_execute); }
			template <class T>
			static std::enable_if_t<std::is_base_of_v<Iexecutable, T>, void>
			post(const std::shared_ptr<T>& _pexecutable, clock::time_point _tick_execute) { post(std::shared_ptr<T>(_pexecutable), _tick_execute); }

			template <class T>
			static std::enable_if_t<is_shared_ptr<std::remove_pointer_t<std::decay_t<T>>>::value, void>
			dispatch(T&& _completor)
			{
				get_instance()->process_dispatch(std::forward<T>(_completor));
			}
			template <class T>
			static std::enable_if_t<std::is_base_of_v<Iexecutable, T>, void>
			dispatch(std::shared_ptr<T>&& _pexecutable)
			{
				struct TX
				{
					std::shared_ptr<Iexecutable> pexecutable;
					void operator ()() { pexecutable->process_execute(); }
				};
				get_instance()->process_dispatch(TX{ std::move(_pexecutable) });
			}
			template <class T>
			static std::enable_if_t<std::is_base_of_v<CGDK::asio::Iexecutable, T>, void>
			dispatch(const std::shared_ptr<T>& _pexecutable)
	{
		dispatch(std::shared_ptr<T>(_pexecutable));
	}

	static	bool register_schedulable(std::shared_ptr<Ischedulable>&& _pschedulable);
	static	bool unregister_schedulable(Ischedulable* _pschedulable) noexcept;

	[[nodiscard]] static boost::asio::io_service& get_io_service() { return get_instance()->io_service; }
	[[nodiscard]] static std::shared_ptr<asio::system> get_instance() { if (!pinstance) { return init_instance(); }; return pinstance; }
	static std::shared_ptr<asio::system> init_instance(int _thread_count = -1);
	static void destroy_instance() noexcept;
	static void run_executor();

protected:
			void process_prepare_thread(int _thread_count);
			void process_prepare_scheduler();
			void process_run_executor();
			void process_destroy() noexcept;
			template <class T> 
			void process_post(T _completor) { io_service.post(std::forward<T>(_completor)); }
			template <class T>
			void process_post(T _completor, clock::time_point _tick_execute) { this->m_pexecutor_single->post_at(std::forward<T>(_completor), _tick_execute); }
			template <class T>
			void process_dispatch(T _completor) { io_service.dispatch(std::forward<T>(_completor)); }

			boost::asio::io_service io_service;
			std::vector<std::shared_ptr<std::thread>> m_vector_threads;
			std::shared_ptr<executor::single> m_pexecutor_single;
			std::shared_ptr<schedulable_manager> m_pschedulable_manager;
			bool m_is_thread_run = false;

	static	std::mutex lock_instance;
	static	std::shared_ptr<asio::system> pinstance;
};
