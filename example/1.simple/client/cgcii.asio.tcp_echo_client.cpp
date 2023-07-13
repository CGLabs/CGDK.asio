#include "cgcii.asio/asio.h"
#include <thread>
#if defined(_WIN32)
	#include <conio.h>
#elif defined(__linux__)
	int _kbhit();
	int _getch();
#endif


class socket_tcp : public asio::Nsocket_tcp_client
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
	// 1) create socket
	auto psocket_tcp = std::make_shared<socket_tcp>();

	// 2) start(connect)
	psocket_tcp->start(boost::asio::ip::tcp::endpoint{ boost::asio::ip::address_v4::loopback(), 20000 });

	// 3) wait for exit pressing ESC key
	while (_getch() != 27);
}
