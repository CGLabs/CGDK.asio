#include "cgdk/asio.h"
#include "../definitions_message.h"
#include "iostream"
#if defined(_WIN32)
	#include <conio.h>
#elif defined(__linux__)
	int _kbhit();
	int _getch();
#endif
using namespace std::chrono_literals;


class socket_tcp : public asio::Nsocket_tcp, public asio::Nconnect_requestable, std::enable_shared_from_this<socket_tcp>
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
		// 1) 메시지를 읽기 위한 임시 buffer_view (message_size 부분을 뛰기 위해 4Byte offset)
		buffer_view msg_recv = _msg + offset(4);

		// 2) get type
		switch (msg_recv.extract<int>())
		{
		case	eMESSAGE::ENTER:
				std::cout << "> member entered" << std::endl;
				break;

		case	eMESSAGE::LEAVE:
				std::cout << "< member leaved" << std::endl;
				break;

		case	eMESSAGE::CHATTING:
				std::cout << '\r' << msg_recv.extract<std::string>() << std::endl;
				break;
		}

		return 1;
	}
};

shared_buffer make_chatting_message(const std::string& _string)
{
	auto buf_temp = alloc_shared_buffer(8 + get_size_of(_string));
	buf_temp.append<uint32_t>();
	buf_temp.append<int>(eMESSAGE::CHATTING);
	buf_temp.append<std::string>(_string);
	buf_temp.front<uint32_t>() = buf_temp.size<uint32_t>();
	return buf_temp;
}

int main()
{
	// trace)
	std::cout << "starting client..." << std::endl;

	// 1) create socket
	auto psocket_tcp = std::make_shared<socket_tcp>();

	// 2) start(connect)
	psocket_tcp->start(boost::asio::ip::tcp::endpoint{ boost::asio::ip::address_v4::loopback(), 20000 });

	// declare)
	std::string chatting_input;

	// 3) loop(exit pressing ESC key)
	for (;;)
	{
		// - key 눌렀나?
		if (_kbhit())
		{
			// - Key를 읽는다.
			int	ch = _getch();

			// - ESC키를 누르면 접속을 종료한다.
			if (ch == 27)
				break;

		#if defined(_WIN32)
			if(ch == 0x0d)
		#elif defined(__linux__)
			if(ch == 0x0a)
		#endif
			{
				psocket_tcp->send(make_chatting_message(chatting_input));
				chatting_input.clear();
			}
			else
			{
				chatting_input.append(1, static_cast<char>(ch));
			}
		}

		std::this_thread::sleep_for(10ms);
	}


	// trace)
	std::cout << "terminating client..." << std::endl;
}
