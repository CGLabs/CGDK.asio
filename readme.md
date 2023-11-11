# What is CGDK.asio classes
asio를 이용해 CGDK와 유사한 인터페이스를 가지도록 설계한 network 모듈입니다.<br>
송수신은 CGDK.buffer를 사용하도록 설계를 했습니다.<br>
네트워크 송수신 through-put 성능 향상을 위해 기본적인 비동기 전송 처리를 구현했습니다.<br>
asio를 사용한 간략한 버전인 만큼 CGDK에서 제공되는 메모리풀이나 객체풀, 다양한 송수신 효율화 및 안정화 알고리즘들은 구현되지 않았지만 <br>
일반적인 상용 server에서도 사용할 수 있을 정도의 성능과 안정성은 제공할 것으로 생각합니다.<br>

<br>

# Getting Start
### Directory 설명
- /include<br>
.h 파일들이 들어 있는 디렉토리로 컴파일시 이곳을 include directory로 설정해 주어야 합니다.<br>
그리고 'include/cgdk/asio.h' 파일을 #include를 통해 포함시켜주면 사용 가능합니다.<br>

- /src <br>
.cpp 파일들이 들어 있는 폴더입니다.<br>
windows의 경우 이 디렉토리에 'CGDK.asio.vs17.sln' 파일을 열어 엔진를 컴파일 할 수 있습니다.<br>

- /lib<br>
엔진이 컴파일 되면 '/lib/cgdk/asio' 디렉토리에 .lib파일 혹은 .a파일이 생성됩니다.<br>

- /example<br>
간단한 서버와 클라이언트 예제입니다.
(엔진('/src')를 먼저 컴파일 후 컴파일 하셔야 됩니다.)
디렉토리에 자세한 설명이 있습니다.
1.simple ()

<br>

# Compile
### windows
visual studio 2022이상 필요합니다.<br>
엔진은 'src/CGDK.asio.vs17.sln'을 열어 컴파일 하시면 .lib 혹은 .a 파일이 생성됩니다.<br>

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
<br>
<br>
> sangducks@gmail.com<br>
> sangduck@cgcii.co.kr<br>
