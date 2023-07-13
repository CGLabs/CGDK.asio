#include "cgdk/asio.h"
#if defined(_WIN32)
	#include <conio.h>
#elif defined(__linux__)
	int _getch();
#endif


class socket_tcp : public asio::Nsocket_tcp_gather_buffer
{
public:
	virtual void on_connect() override
	{
	}
	virtual void on_disconnect(boost::system::error_code /*_error_code*/) noexcept override
	{
	}
	virtual int on_message(shared_buffer& _msg) override
	{
		send(_msg);
		return 1;
	}
};

int main()
{
	// 1) create acceptor
	auto pacceptor = std::make_shared<asio::acceptor<socket_tcp>>();

	// 2) start accept
	pacceptor->start(tcp::endpoint{ boost::asio::ip::tcp::v6(), 20000 });

	// 3) wait for exit pressing ESC key
	while (_getch() != 27);
}