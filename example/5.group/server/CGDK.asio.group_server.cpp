#include "pch.h"

#if defined(_WIN32)
	#include <conio.h>
#elif defined(__linux__)
	int _getch();
#endif

int main()
{
	// trace)
	std::cout << "starting server... [CGDK.asio example.5.group server]" << std::endl;

	// 1) create entities
	g_pgroup_chatting = std::make_shared<group_chatting>();
	auto pacceptor = std::make_shared<asio::acceptor<socket_tcp>>();

	// 2) start accept
	pacceptor->start(tcp::endpoint{ boost::asio::ip::tcp::v6(), 20000 });

	// 3) wait for exit pressing ESC key
	while (_getch() != 27);

	// trace)
	std::cout << "terminating server..." << std::endl;
}