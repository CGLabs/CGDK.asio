#include "cgdk/asio.h"
#include "iostream"
#if defined(_WIN32)
	#include <conio.h>
#elif defined(__linux__)
	int _kbhit();
	int _getch();
#endif


class socket_tcp : public asio::Nsocket_tcp, public asio::Nconnect_requestable
{
public:
	virtual void on_connect() override
	{
		// trace)
		std::cout << "@ connected" << std::endl;

		// - make message 
		auto buf = alloc_shared_buffer(32);
		buf.append<uint32_t>(8);
		buf.append<uint32_t>(100);

		// - send 10 times
		for(int i=0;i<10;++i)
			this->send(buf);
	}
	virtual void on_disconnect(boost::system::error_code /*_error_code*/) noexcept override
	{
		// trace)
		std::cout << "@ disconnted" << std::endl;
	}
	virtual int on_message(sMESSAGE& _msg) override
	{
		// trace)
		std::cout << "@ message received " << _msg.buf_message.size() << "bytes" << std::endl;
		return 1;
	}
};

int main()
{
	// trace)
	std::cout << "starting client..." << std::endl;

	// 1) create socket
	auto psocket_tcp = std::make_shared<socket_tcp>();

	// 2) start(connect)
	psocket_tcp->start(boost::asio::ip::tcp::endpoint{ boost::asio::ip::address_v4::loopback(), 20000 });

	// 3) wait for exit pressing ESC key
	while (_getch() != 27);

	// trace)
	std::cout << "terminating client..." << std::endl;
}
