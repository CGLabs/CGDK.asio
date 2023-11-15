# simle chatting
콘솔창을 사용한 간단하 chatting server와 client의 예제입니다..
서버는 하나의 채팅방만 존재합니다.
따라서 서버에 접속한 클라이언트는 하나의 방으로 입장합니다.
또 서버는 멤버의 입장과 퇴장을 전체 멤버에게 알립니다.(다만 멤버의 id가 존재하지 않습니다.)
클라이언트에서 문자열을 전송하면 서버는 전체 멤버들에게 채팅 매시지를 전송됩니다.<br>

## Directory

- /sever<br>
서버 소스입니다.

- /client<br>
클라이언트 소스입니다.

## compile
@Notice 먼저 engine의 core가 컴파일 되어 있어야 합니다. ('/src'를 먼저 컴파일 해주십시요.)<br>

### windows
visual studio 2022이상이 필요합니다.<br>
서버는 'CGDK.asio.tcp_echo_server.vs17.sln'<br>
클라이언트는 'CGDK.asio.tcp_echo_client.vs17.sln'<br>
을 열어 컴파일해 주시면 됩니다.<br>

### linux
cmake가 필요합니다.<br>
Debug로 컴파일을 원하시면<br>
```bash
$ cmake . -DCMAKE_BUILD_TYPE=Debug
$ make
```
Release로 컴파일 하려면  Debug대신 Release로만 변경해서 실행해 주시면 됩니다.<br>
```bash
$ cmake . -DCMAKE_BUILD_TYPE=Release
$ make
```
<br>

## server
기본 구성 요소인 acceptor의 사용법으로 보여 줍니다.<br>

'CGDK.asio.tcp_echo_server.vs17.sln'<br>

__1.__ 먼저 CGDK.asio의 헤더 파일을 include해줍니다.<br>

```c++
#include "cgdk/asio.h"
```
<br>

__2.__ CGDK::asio::Nsocket_tcp 상속 받아 socket_tcp를 정의합니다.<br>
서버에서 접속을 받을 socket 클래스 입니다. <br>
__3.__ 상속 받은 후 on_connect, on_disconnect, on_message 함수를 재정의해 줍니다.<br>
이 함수들은 각각 접속되었을 때, 접속이 종료되었을 때, 메시지가 전달되어 왔을 때 호출 되는 함수입니다.

```c++
class socket_tcp : public asio::Nsocket_tcp, public asio::Nconnect_requestable
{
public:
	virtual void on_connect() override;
	virtual void on_disconnect(boost::system::error_code _error_code) noexcept override;
	virtual int on_message(shared_buffer& _msg) override;
};
```
<br>

```c++
void socket_tcp::on_connect() 
{
	// 1) group에 입장합니다.(enter chatting group)
	g_pgroup_chatting->enter_member(std::dynamic_pointer_cast<socket_tcp>(this->shared_from_this()));

	// trace)
	std::cout << "@ connected" << std::endl;
}

void socket_tcp::on_disconnect(boost::system::error_code /*_error_code*/) noexcept
{
	// 1) group에서 퇴장합니다.(leave chatting group)
	g_pgroup_chatting->leave_member(std::dynamic_pointer_cast<socket_tcp>(this->shared_from_this()));

	// trace)
	std::cout << "@ disconnted" << std::endl;
}

int socket_tcp::on_message(shared_buffer& _msg)
{
	// 1) 전체 group의 멤버들에게 전송합니다.(send to all member)
	g_pgroup_chatting->send(_msg);

	// trace)
	std::cout << "@ message received" << std::endl;

	// return) 
	return 1;
}
```
__4.__ 클라이언트가 접속을 하면(socket_tcp::on_connect) 바로 g_group_chatting에 입장시킵니다.
 이때 shared_from_this 함수로 this(자신)의 shared_ptr를 얻어냅니다.
 하지만 이것은 socket_tcp의 포인터가 아니므로 dynamic_pointer_cast<socket_tcp>를 사용해 std::shared_ptr<socket_tcp>로 캐스팅을 해줍니다.
 

__5.__ 클라이언트가 접속 해제를 하면 g_pgroul_chatting에서 퇴장시킵니다.<br>
이때도 역시 on_connect 때와 같이 shared_from_hist를 dynamic_pointer_cast한 shared_ptr<socket_tcp>를 사용해 퇴장시켜줍니다.

__6.__ 메시지가 전송되어 오면 g_pgroup_chatting의 모든 멤버들에게 그대로 echo 전송을 헤줍니다.
<br>

__7.__ gruop_chatting 클래스를 정의합니다. 채팅방에 해당하는 클래스로 멤버의 입장과 퇴장 그리고 멤버들의 관리를 수행합니다.
```c++
class group_chatting
{
public:
		void enter_member(std::shared_ptr<socket_tcp>&& _pmember);
		void leave_member(const std::shared_ptr<socket_tcp>& _pmember) noexcept;
		void send(shared_buffer _buf_message);
private:
		std::recursive_mutex m_lock_memer;
		std::set<std::shared_ptr<socket_tcp>> m_set_member;
};
```
__8.__ 멤버 입장 처리 함수.
멤버가 입장하면 m_set_member에 추가한 후 모든 멤버들이게 입장했음을 전송합니다.
멤버의 id가 없으므로 중복 접속 등의 검사를 하지 않습니다.
(만약 중복 접속 검사 등이 들어간다면 훨씬 복잡해 지겠죠)

```c++
void group_chatting::enter_member(std::shared_ptr<socket_tcp>&& _pmember)
{
	// lock)
	std::lock_guard lock(this->m_lock_memer);

	// 1) 멤버 리스트에 추가합니다. (add member to member_list)
	this->m_set_member.insert(std::move(_pmember));

	// 2) 전체에게 입장 메시지를 전송 (메시지크기(8) + MESSAGE_TYPE(LEAVE))
	this->send(alloc_shared_buffer(8) << uint32_t(8) << eMESSAGE::ENTER);
}
```
<br>

__9.__ 멤버 퇴장 처리 함수
해당 멤버를 제거하고 전체 멤버에게 퇴장을 알리는 메시지를 전송합니다.

```c++
void group_chatting::leave_member(const std::shared_ptr<socket_tcp>& _pmember) noexcept
{
	// lock)
	std::lock_guard lock(this->m_lock_memer);

	// 1) 멤버 리스트에서 제거합니다.(remove member from member_list)
	this->m_set_member.erase(_pmember);

	// 2) 전체 멤버에게 퇴장 메시지를 전송 (메시지크기(8) + MESSAGE_TYPE(LEAVE))
	this->send(alloc_shared_buffer(8) << uint32_t(8) << eMESSAGE::LEAVE); 
}
```

__10.__ 전체 멤버에게 전송하는 함수
group에 존재하는 모든 멤버에게 전송하는 함수입니다.
m_set_member의 모든 멤버의 send를 호출해주면 됩니다.
```c++
void group_chatting::send(shared_buffer _buf_message)
{
	// lock)
	std::lock_guard lock(this->m_lock_memer);

	// 1) 전체에게 전송
	for (auto& iter : this->m_set_member)
		iter->send(_buf_message);
}
```

__11.__ 이로써 서버 동작에 필요한 group과 socket_tcp의 작성이 끝났으니 main함수에서 서버를 시작하는 코드만 작성하면 되겠죠.
```c++
int main()
{
	// trace)
	std::cout << "starting server... [CGDK.asio example.3.simple_chatting server]" << std::endl;

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
```
<br>
__12.__ 먼저 g_pgroup_chatting부터 먼저 생성합니다.
__13.__ 그 다음 acceptor를 생성해 listen을 시작해 드디어 서버를 클라이언트의 접속을 받습니다.<br>
<br>
<br>

## client<br>

클라이언트가 서버로 접속해 채팅 메시지를 받으면 화면에 표시해주며 채팅 메시지를 서버로 전송합니다.<br>

'CGDK.asio.tcp_echo_client.vs17.sln'<br>

__1.__ 먼저 socket_tcp을 정의합니다.
  GDK::asio::Nsocket_tcp과 CGDK::asio::Nconnect_requestable를 상속 받아 정의합니다.<br>
   'CGDK::asio::Nconnect_requestable'는 접속 기능을 제공합니다.<br>

```c++
class socket_tcp : public asio::Nsocket_tcp, public asio::Nconnect_requestable
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
				std::cout << '\r' << "> member entered" << std::endl << ">> " << g_chatting_input;
				break;

		case	eMESSAGE::LEAVE:
				std::cout << '\r' << "< member leaved" << std::endl << ">> " << g_chatting_input;
				break;

		case	eMESSAGE::CHATTING:
				std::cout << "\r\033[J" << msg_recv.extract<std::string>() << std::endl << ">> " << g_chatting_input;
			
				break;
		}

		return 1;
	}
};
```
__2.__ 여기서 on_message에 메시지에 따라 다른 동작을 수행하도록 정의를 했습니다.<br>
메시지의 포맷은 처음 4Byte는 메시지의 길이이며, 다음 4Byte는 메시지의 종류입니다.<br>
다라서 Offset 4Byte의 값을 읽으면 그것이 메시지의 종류가 됩니다.<br>
	 * 메시지의 종류는 'eMESSAGE::ENTER'와 eMESSAGE::LEAVE' 그리고 'eMESSAGE::CHATTING'이 존재합니다.<br>
	 * 'eMESSAGE::ENTER'와 eMESSAGE::LEAVE'는 화면상에 해당 메시지만 출력해 주면 됩니다.<br>
	 * 'eMESSAGE::CHATTING'일 경우 채팅 메시지를 추가적으로 읽어내어 화면에 출력해 줍니다.<br>
<br>
__3.__ 채팅 메시지 보내기<br>
키입력을 받으면 ESC와 enter를 제외한 입력을 'g_chatting_input' 변수에 하나씩 저장해 높습니다.<br>
그리고 enter키를 입력하면 지금까지 저장한 문자열을 전송하고 'g_chatting_input'를 클리어합니다.<br>
<br>
__4.__ 이때 서버로 전송할 매시를 make_chatting_message를 사용해 작성한 후 서버로 바로 전송한다.<br>
```c++
shared_buffer make_chatting_message(const std::string& _string)
{
	auto buf_temp = alloc_shared_buffer(8 + get_size_of(_string));
	buf_temp.append<uint32_t>();
	buf_temp.append<int>(eMESSAGE::CHATTING);
	buf_temp.append<std::string>(_string);
	buf_temp.front<uint32_t>() = buf_temp.size<uint32_t>();
	return buf_temp;
}
```

```c++
int main()
{
	// trace)
	std::cout << "starting client... [CGDK.asio example.3.simple_chatting client]" << std::endl;

	// 1) create socket
	auto psocket_tcp = std::make_shared<socket_tcp>();

	// 2) start(connect)
	psocket_tcp->start(boost::asio::ip::tcp::endpoint{ boost::asio::ip::address_v4::loopback(), 20000 });

	// 3) loop(exit pressing ESC key)
	for (;;)
	{
		// - key 눌렀나?
		if (::_kbhit())
		{
			// - Key를 읽는다.
			int	ch = ::_getch();

			// - ESC키를 누르면 접속을 종료한다.
			if (ch == 27)
				break;

		#if defined(_WIN32)
			if(ch == 0x0d)
		#elif defined(__linux__)
			if(ch == 0x0a)
		#endif
			{
				auto str_end = std::move(g_chatting_input);
				psocket_tcp->send(make_chatting_message(str_end));
			}
			else
			{
				std::cout << static_cast<char>(ch);
				g_chatting_input.append(1, static_cast<char>(ch));
			}
		}

		std::this_thread::sleep_for(10ms);
	}

	// trace)
	std::cout << "terminating client..." << std::endl;
}
```
<br>
