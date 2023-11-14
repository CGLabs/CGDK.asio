# simle chatting
콘솔창을 사용한 간단하 chatting server와 client의 예제입니다..
서버는 하나의 채팅방만 존재합니다.
따라서 서버에 접속한 클라이언트는 이 방에 입장합니다.
서버는 멤버의 입장과 퇴장을 전체 멤버에게 알립니다.(다만 멤버의 id가 존재하지 않습니다.)
클라이언트에서 문자열을 전송하면 전체 멤버들에게 채팅 매시지가 전송됩니다.<br>

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


```c++
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
```
<br>

__5.__ acceptor에 클라이언트의 접속이 들어오면 sockt_tcp 객체를 생성해 on_connect 함수를 호출해 줍니다.<br>
__6.__ 메시지가 전달되어 오면 on_message 함수를 호출해 줍니다.<br>
__7.__ 접속이 종료되면 on_disconnect 함수를 호출해 줍니다.<br>
__8.__ socket_tcp 객체의 멤버함수인 send를 호출하면 그 소켓이 연결된 클라이언트로 메시지를 전송합니다.<br>
<br>
<br>

## client<br>

클라이언트가 서버로 접속해 메시지를 전송하는 것을 보여 줍니다.<br>

'CGDK.asio.tcp_echo_client.vs17.sln'<br>

__1.__ 먼저 socket_tcp을 CGDK::asio::Nsocket_tcp과 CGDK::asio::Nconnect_requestable를 상속받아 정의합니다.<br>
   'CGDK::asio::Nconnect_requestable'는 접속 기능을 제공합니다.<br>

```c++
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
			send(buf);
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
		return 1;
	}
};
```
<br>

__2.__ 정의한 socket_tcp 클래스의 객체를 생성합합니다.<br>
__3.__ socket_tcp 객체의 start 함수를 호출해 원하는 주소로 접속을 시도합니다..<br>
   접속 주소는 boost::asio::ip를 사용해 지정합니다.<br>

```c++
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
```

<br>

__4.__ 접속이 성공하면 on_connect가 호출되며 실패하면 on_fail_connect가 호출됩니다.<br>
__5.__ 접속이 종료되면 on_disconnect 함수, 메시지가 전달되어 오면 on_message 함수가 호출됩니다.<br>
__6.__ socket의 멤버함수인 send를 사용해 메시지를 전송할 수 있습니다.(서버에서의 socket과 동일합니다.)<br>
__7.__ socket의 on_connect에서 8byte 메시지를 상대에게 10번 전송하도록 작성 되었습니다.<br>
