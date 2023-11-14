# What is CGDK.asio classes
asio를 이용해 CGDK와 유사한 인터페이스를 가지도록 설계한 network 모듈입니다.<br>
송수신에 CGDK.buffer를 사용해 구현했습니다.<br>
네트워크 송수신 through-put 성능 향상을 위해 기본적인 비동기 및 gathering 전송 처리를 구현했습니다.<br>
(다만 asio의 특성상 buffer를 사용한 gathering처리를 해도 성능에는 약간 한계가 있습니다.)<br>
asio를 사용한 간략한 버전인 만큼 CGDK에서 제공되는 메모리풀이나 객체풀, 다양한 송수신 효율화 및 안정화 알고리즘들은 구현되지 않습니다. <br>
하지만 일반적인 상용 server에서 사용할 수 있을 정도의 성능과 안정성은 제공할 것으로 생각합니다.<br>
<br>

# Getting Start
### Directory 설명
- /include<br>
.h 파일들이 들어 있는 디렉토리로 컴파일시 이곳을 include directory로 설정해 주어야 합니다.<br>
그리고 'include/cgdk/asio.h' 파일을 포함(#include ...)시켜주면 사용 가능합니다.<br>

- /src <br>
.cpp 파일들이 들어 있는 폴더입니다.<br>
windows의 경우 이 디렉토리에 'CGDK.asio.vs17.sln' 파일을 열어 코어를 컴파일 할 수 있습니다.<br>

- /lib<br>
코어가 컴파일 되면 '/lib/cgdk/asio' 디렉토리에 .lib파일 혹은 .a파일이 생성됩니다.<br>

- /example<br>
간단한 서버와 클라이언트 예제입니다.
(엔진('/src')를 먼저 컴파일 후 컴파일 하셔야 됩니다.)
디렉토리에 자세한 설명이 있습니다.
1.simple 간단한 서버와 클라이언트의 예제입니다.
2.tcp_echo_test tcp 송수신 능력과 접속 능력을 테스트 할 수 있는 서버와 클라이언트 예제입니다.

<br>

# Compile
### windows
visual studio 2022이상 필요합니다.<br>
'src/CGDK.asio.vs17.sln'을 열어 컴파일 하시면 .lib 파일이 생성됩니다.<br>

### linux
gcc와 cmake가 필요합니다.<br>
Debug로 컴파일을 하려면 프로젝트의 루트 디렉토리에에서<br>
```bash
$ cmake . -DCMAKE_BUILD_TYPE=Debug
$ make
```
Release로 컴파일 하려면<br>
```bash
$ cmake . -DCMAKE_BUILD_TYPE=Release
$ make
```
프로젝트 루트 디렉토리에서 make를 하면 엔진 컴파일(lib파일)과 example까지 한번에 컴파일할 수 있습니다.<br>
정적 라이브로리(.a 파일)을 /lib 디렉토리에 생성합니다.<br>
('.so' 파일이 필요하시면 cmake 파일을 조금 수정하시면 됩니다. 하지만 서버라면 웬만하면 '.a'파일을 쓰는 것을 추천합니다.)<br>
<br>
<br>
# Example<br>
1. __simple server/client__ 간단한 server와 client를 만드는 예제입니다. (https://github.com/CGLabs/CGDK.asio/tree/main/example/1.simple)<br>
2. __tcp echo server/client__ 송수신 성능을 확인하기 위한 server와 client 예제입니다.(https://github.com/CGLabs/CGDK.asio/tree/main/example/2.tcp_echo_test)<br>
<br>
### 라이센스(License)
   MIT License(IT) 를 따르고 있습니다.<br>

> sangducks@gmail.com<br>
> sangduck@cgcii.co.kr<br>

