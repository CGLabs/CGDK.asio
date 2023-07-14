# What is CGDK.asio classes
asio를 이용해 network 모듈이다.
CGDK와 유사한 인터페이스를 가지도록 설계했다.
CGDK.buffer를 사용해 송수신이 가능하도록 처리를 했다.
네트워크 송수신 through-put 성능 향상을 위해 비동기 모아 보내기(send gatering) 처리를 구현했다.
다만 CGDK에서 제공되는 메모리풀이나 객체풀, 다양한 송수신 효율화 및 안정화 알고리즘들은 구현되지 않았다.
server에서 사용할 수 있는 수준의 높은 성능과 안정성을 확보하도록 설계가 되었다.

# Getting Start
## Directory
### /include 
.h파일들이 들어 있는 디렉토리로 컴파일시 이곳을 include directory로 설정해 주어야 한다.
'include/cgdk/asio.h' 파일을 #include를 통해 포함시켜주어야 한다.

### /src 
.cpp파일들이 들어 있는 폴더이다.

### /lib
모듈이 컴파일 되면 /lib/cgdk/asio 디렉토리 밑에 .lib파일 혹은 .a파일이 생성된다.
lib directory로 /lib/cgdk/asio를 설정해 주어야 한다.

##compile
###windows
visual studio 2022이상이 필요하다.
lib파일은 'src/CGDK.asio.vs17.sln'을 열어 컴파일이 가능하다.
example파일들은 /example디렉토리의 각 디렉토리 안쪽에 server와 client 용 sln파일들을 있으며 이들을 열어서 컴파일 하면 된다.

###linux
cmake가 필요하며 Debug로 컴파일을 하려면 루트에서 
```bash
$ cmake . -DCMAKE_BUILD_TYPE=Debug
$ make
```

Release로 컴파일 하고 싶을 때에는 Debug대시 Release로만 변경하면 된다.
```bash
$ cmake . -DCMAKE_BUILD_TYPE=Release
$ make
```

루트 디렉토리에서 하면 lib파일과 example까지 모두 컴파일 되며 디렉토리별로 컴파일 할수 있다.
예를 들어 /src 디렉토리에 동일한 작업을 하면 lib파일만 컴파일 된다.

## Project 설정

## examples
### 1. simle tcp echo
가장 간단한 server와 client를 만들 수 있는 방법을 설명한 예제이다.
#### server
기본 구성 요소인 acceptor의 사용 방법을 설명한다.
1. 먼저 CGDK.asio의 헤더 파일을 include해준다.
```c++
#include "cgdk/asio.h"
```

socket_tcp를 CGDK::asio::Nsocket_tcp 상속 받아 정의한다.
on_connect, on_disconnect, on_message 함수를 재정의한다.
```c++
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
		send(_msg);
		return 1;
	}
};
```
server는 acceptor를 생성 후 start함수로 20000번 포트로 클라이언트의 접속을 대기한다.
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

접속이 들어오면 sockt_tcp 객체를 만들어 on_connect 함수를 호출해 준다.
메시지가 전달되어 오면 on_message 함수를 호출해 준다.
접속이 종료되면 on_disconnect 함수를 호출해 준다.
socket_tcp 객체에 send 함수를 호출하면 그 소켓이 연결된 클라이언트에 메시지를 전송한다.

#### client
접속을 시도하는 방법을 설명한다.
서버와 동일한 방법으로 socket_tcp을 CGDK::asio::Nsocket_tcp과 CGDK::asio::Nconnect_requestable를 상속받아 정의한다.

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
CGDK::asio::Nconnect_requestable는 접속을 시도할 수 있는 start 함수의 호출이 가능하도록 해준다.
socket_tcp 객체를 생성 후 start 함수를 호출해 원하는 주소로 접속을 시도한다.
접속할 주소는 boost::asio::ip를 사용해서 지정한다.

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
접속이 성공하면 on_connect가 호출되며 실패하면 on_fail_connect가 호출된다.
접속된 후 접속이 종료되면 on_disconnect 함수, 메시지가 전달되어 오면 o_message 함수가 호출되며 또 send함수를 사용해 전송할 수 있다는 기능은 서버의 socket 예제와 동일한다.
여기에서 on_connect에서 8byte 메시지를 작성해 상대에게 10번 전송하도록 작성이 되었다.
