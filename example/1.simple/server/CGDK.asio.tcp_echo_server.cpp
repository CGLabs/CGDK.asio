#include "cgdk/asio.h"
#include "iostream"
#if defined(_WIN32)
	#include <conio.h>
#elif defined(__linux__)
	int _getch();
#endif

class socket_tcp : public asio::Nsocket_tcp
{
public:
	virtual void on_connect() override
	{
		// trace)
		std::cout << "@ connected" << std::endl;
	}
	virtual void on_disconnect(boost::system::error_code /*_error_code*/) noexcept override
	{
		// trace)
		std::cout << "@ disconnted" << std::endl;
	}
	virtual int on_message(shared_buffer& _msg) override
	{
		// trace)
		std::cout << "@ message received " << _msg.size() << "bytes" << std::endl;

		// - echo send
		this->send(_msg);
		return 1;
	}
};

int main()
{
	// trace)
	std::cout << "starting server..." << std::endl;

	// 1) create acceptor
	auto pacceptor = std::make_shared<asio::acceptor<socket_tcp>>();

	// 2) start accept
	pacceptor->start(tcp::endpoint{ boost::asio::ip::tcp::v6(), 20000 });

	// 3) wait for exit pressing ESC key
	while (_getch() != 27);

	// trace)
	std::cout << "terminating server..." << std::endl;
}