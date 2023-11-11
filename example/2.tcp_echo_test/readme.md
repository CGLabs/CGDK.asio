# simle tcp echo
대규모 송수신과 랜덤 접속 테스트를 할 수 있는 서버와 클라이언트 입니다.<br>
asio인 만큼 극단적인 성능까지는 아니더라도 일반적인 서버에서 충분히 쓸 수 있을 만큼의 성능을 발휘해 줍니다.<br>
이 tcp echo test server와 client로 크게 2가지의 테스트를 수행 할 수 있습니다.<br>
<br>
__첫번째, 접속 테스트__<br>
1초당 정해진 수량의 socket을 신규로 접속하고 접속 해제하는 테스트입니다.<br>
초당 수백 수천번의 접속과 접속 해제를 하며 접속 성능과 함께 안정성을 확인할 수 있습니다.<br>
서버가 다운되거나 더이상 접속을 받지 못하거나 메모리가 계속 증가하게 되면 안되겠죠.<br>
<br>

__두번째, 전송 능력 테스트__<br>
접속한 소캣당 많은 수의 메시지를 전송하여 얼마만큼의 메시지를 안정적으로 송수신할 수 있는지를 테스트 할 수 있습니다.<br>
test client는 전송하는 메시지의 크기를 8byte로 부터 256Mbyte 크기까지 설정하여 송수신 테스트를 할 수 있습니다.<br>
또 초당 몇 개의 메시지를 전송할 것인가를 설정할 수도 있습니다.<br>
<br>
__세번째, 두가지 모두 테스트__<br>
접속테스트와 전송 테스트를 한꺼번에 진행 할 수 있습니다.
서버용 모듈이라면 접속과 송수신을 한꺼번에 동작시켜 하드웨어 성능의 극한까지 테스트 해서 안정적이여아만 할 것입니다.


## Directory

- /sever<br>
서버 소스입니다.

- /client<br>
클라이언트 소스입니다.

## compile
(!) 먼저 engine의 core가 컴파일 되어 있어야 합니다. ('/src'를 먼저 컴파일 해주십시요.)<br>

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

## 사용법

서버와 클라이언트는 실행 파일을 실행 하시면 됩니다.
서버와 클라이언트의 console 화면에 조작키에 대한 설명이 나올 것 입니다.