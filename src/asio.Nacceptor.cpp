#include "asio.h"


asio::Nacceptor::Nacceptor()
{
}

asio::Nacceptor::~Nacceptor() noexcept
{
	this->close();
}

void asio::Nacceptor::start(boost::asio::ip::tcp::endpoint _endpoint)
{
	//-----------------------------------------------------------------------------
	//  설명) acceptor를 시작한다.
	// 
	//  
	//  
	//-----------------------------------------------------------------------------
	// 1) create context
	auto accept_context = std::make_shared<tcp::acceptor>(asio::system::get_io_service().get_executor(), _endpoint);

	// 2) copy create handle
	{
		std::lock_guard cs{this->m_lockable_acceptor};
		this->m_accept_context = std::move(accept_context);
	}

	try
	{
		// 3) accept 대기할 수는 thread의 수만큼
		auto accept_count = std::thread::hardware_concurrency();

		// 4) request accept
		for(; accept_count > 0; --accept_count)
			this->process_request_accept();
	}
	catch (...)
	{
		// - rollback
		{
			std::lock_guard cs{this->m_lockable_acceptor};
			this->m_accept_context.reset();
		}

		// reraise) 
		throw;
	}
}

void asio::Nacceptor::close() noexcept
{
	//-----------------------------------------------------------------------------
	//  설명) acceptor를 받는다.
	// 
	//  더 이상 accept를 받지 않음은 물론 이 acceptor로 접속한 모든 socket을 접속
	//  종료처리한다.
	//-----------------------------------------------------------------------------

	// 1) accept 받은 모든 connectalbe(socket)을 먼저 접속 종료 한다.
	this->destroy_connectable_all();

	// declare) 
	std::shared_ptr<tcp::acceptor> accept_context;

	// 2) copy
	{
		// lock)
		std::lock_guard cs{this->m_lockable_acceptor};

		// check) 
		if (!this->m_accept_context)
			return;

		// - accept_context를 옮겨 놓는다.
		accept_context = std::move(this->m_accept_context);
	}

	// 3) 핸들들을 모두 닫는다.
	accept_context->close();

	// 설명) handle만 닫게 되면 accept instance들은 실패를 리턴하며 스스로 제거될 것이다.
	//       따라서 m_iist_instance는 따로 제거하지 않는다.
	//       close() 후 즉시 객체가 소멸되는 것이 아니라 accpet instance들이 모두 종료되어야 제거된다.
}

void asio::Nacceptor::process_request_accept()
{
	//-----------------------------------------------------------------------------
	//  설명) 새로운 accept instance를 생성해서 accept를 걸어 놓는다.
	// 
	//  새로운 accept instance를 생성한 후 등록하고 accept를 걸어둔다.
	//  여러 개의 accept instance를 실행할 수가 있다.
	//  일반적으로는 1개의 accept instance만드로 충분하지만 빈번한 접속을 처리하기
	//  위해 여러 개의 accpet instance를 걸어 놓을 수도 있다.
	//  thread 수만큼의 accept instance 이상은 대부분의 경우 의미 없다.
	//-----------------------------------------------------------------------------

	// 1) instance를 만든다.
	auto pinstance_create = std::make_unique<instance>();

	// declare) 
	instance* pinstance = pinstance_create.get();

	// 2) acceptor를 등록한다.
	this->process_instance_register(std::move(pinstance_create));

	// 3) accept 시작
	try
	{
		pinstance->process_accept();
	}
	catch (...)
	{
		this->process_instance_unregister(pinstance);
	}
}

void asio::Nacceptor::process_instance_register(std::unique_ptr<instance>&& _instance)
{
	//-----------------------------------------------------------------------------
	//  설명) 새로운 accept instance를 등록한다.
	// 
	//  새로운 accept instance를 등록을 하고 pacceptor와 accept_context 값을 설정
	//  해준다.
	//  pacceotor를 설정해 주는 것은 다중 쓰레드로 동작을 대비한 것이다.
	//  Nacceotor 객체가 소멸되어 오류가 나는 것을 방지하기 위해 hold를 해주는 의미
	//  가 있다.
	//-----------------------------------------------------------------------------

	// lock) 
	std::lock_guard cs{this->m_lockable_acceptor};

	// 1) accept설정
	_instance->pacceptor = dynamic_pointer_cast<Nacceptor>(this->shared_from_this());
	_instance->accept_context = this->m_accept_context;

	// 2) instance list에 등록한다.
	this->m_list_instance.push_back(std::move(_instance));
}

void asio::Nacceptor::process_instance_unregister(instance* _instance) noexcept
{
	//-----------------------------------------------------------------------------
	//  설명) 새로운 accept instance를 등록을 해지한다.
	// 
	//  해당 accept instance를 찾아 등록 해지한다.
	//  acceotor의 종료를 위해 close()를 할 경우 accpetor instance들은 처리에 error
	//  가 발생하며 스스로 등록 해제 처리를 하게 된다.
	// 
	//  주의) instance가 this의 참조를 가지고 있으므로 m_iist_instance에서 erase로
	//         제거 시 this가 destroy될수 있으므루 주의!
	//-----------------------------------------------------------------------------

	// lock) 
	std::lock_guard cs{this->m_lockable_acceptor};

	// 1) find
	auto iter_find = std::find_if(this->m_list_instance.begin(), this->m_list_instance.end(), [=](const auto& iter) { return iter.get() == _instance; });

	// check) return if not exist
	if (iter_find == this->m_list_instance.end())
		return;

	// 2) erase 
	this->m_list_instance.erase(iter_find);
}

void asio::Nacceptor::instance::process_accept()
{
	// 1) alloc socket
	auto psocket_new = this->pacceptor->process_create_socket();

	// 2) set socket state ESOCKET_STATUE::SYN
	{
		// - desiged state
		ESOCKET_STATUE socket_state_old = ESOCKET_STATUE::NONE;

		// - change state
		auto changed = psocket_new->m_socket_state.compare_exchange_weak(socket_state_old, ESOCKET_STATUE::SYN);

		// check)
		assert(changed == true);

		// return) 
		if (changed == false)
			throw std::runtime_error("socket aleady connected or tring connectiong");
	}

	// 3) set socket
	this->psocket = std::move(psocket_new);

	// statistics)
	++Nstatistics::statistics_connect_try;

	// 4) request accept
	this->accept_context->async_accept(this->psocket->m_socket,
		[=, this](boost::system::error_code ec)
		{
			// 1) get socket
			std::shared_ptr<Isocket_tcp> psocket = std::move(this->psocket);

			// check)
			assert(psocket);

			// check) 
			if (ec)
			{
				// - upregister self
				this->pacceptor->process_instance_unregister(this);

				// - rollback (set socket state ESOCKET_STATUE::NONE)
				psocket->m_socket_state.exchange(ESOCKET_STATUE::NONE);

				// return)
				return;
			}

			// 2) register socket
			this->pacceptor->process_register_socket(psocket);

			try
			{
				// 2) connect
				psocket->process_connect_complete();

				// statistics) 
				++Nstatistics::statistics_connect_success;
				++Nstatistics::statistics_connect_keep;
			}
			catch (...)
			{
				this->pacceptor->process_unregister_socket(psocket);

				// - rollback (set socket state ESOCKET_STATUE::NONE)
				psocket->m_socket_state.exchange(ESOCKET_STATUE::NONE);
			}

			// 2) request asyc-accept
			this->process_accept();
		});

}
