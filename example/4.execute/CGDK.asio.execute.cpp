//*****************************************************************************
//*                                                                           *
//*                      Cho sanghyun's Game Classes II                       *
//*                       Ver 10.0 / Release 2019.12.11                       *
//*                                                                           *
//*                     asio.example.simle_chatting.server                    *
//*                                                                           *
//*                                                                           *
//*                                                                           *
//*                                                                           *
//*  This Program is programmed by Cho SangHyun. sangduck@cgcii.co.kr         *
//*  Best for Game Developement and Optimized for Game Developement.          *
//*                                                                           *
//*                (c) 2003. Cho Sanghyun. All right reserved.                *
//*                          http://www.CGCII.co.kr                           *
//*                                                                           *
//*****************************************************************************
#include "cgdk/asio.h"
#include <iostream>
#if defined(_WIN32)
	#include <conio.h>
#elif defined(__linux__)
	int _kbhit();
	int _getch();
#endif

int main()
{
	//-----------------------------------------------------------------------------
	// post & dispatch
	//-----------------------------------------------------------------------------
	{
		// 1) post함수
		asio::system::post([]() { std::cout << "this is posted\n"; });

		// 2) dispatch 함수
		asio::system::dispatch([]() { std::cout << "this is dispatched\n"; });
	}

	//-----------------------------------------------------------------------------
	// post at!
	//-----------------------------------------------------------------------------
	{
		// 1) Iexecutable를 상속받아 클래스를 정의합니다. 
		//    process_execute()에 실행할 내용을 정의합니다.
		class xx : public CGDK::asio::Iexecutable
		{
			virtual void process_execute() override
			{
				std::cout << "xx is dispatched\n";
			}
		};

		// 2) 1초, 2초, 3초 이후에 실행합니다.("xx is dispatched\n"를 출력합니다.)
		asio::system::post(std::make_shared<xx>(), std::chrono::system_clock::now() + 1s);
		asio::system::post(std::make_shared<xx>(), std::chrono::system_clock::now() + 2s);
		asio::system::post(std::make_shared<xx>(), std::chrono::system_clock::now() + 3s);

		// 3) lambda 함수로 정의할 수 있습니다.
		asio::system::post([]() {std::cout << "lamba executed\n"; }, std::chrono::system_clock::now() + 4s);
	}

	//-----------------------------------------------------------------------------
	// schedulable (1)
	//-----------------------------------------------------------------------------
	{
		// 1) asio::schedulable::Nexecutable를 상속받아 클래스를 정의합니다.
		//   process_execute()함수에 실행할 내용을 정의합니다.
		class test_schedulable : public asio::schedulable::Iexecutable
		{
			virtual void process_execute() override
			{
				std::cout << "schedulable executed\n";
			}
		};

		// 2) schedulable 객체를 생성합니다.
		auto pschedulable = std::make_shared<test_schedulable>();

		// 3) 2초 마다 한번씩 실행하도록 합니다.
		pschedulable->execute_interval(2s);

		// 4) schedulable을 시작합니다.
		asio::system::register_schedulable(pschedulable);

		// 5) 키입력을 기다립니다.
		_getch();

		// 6) schedulable을 중단합니다.
		asio::system::unregister_schedulable(pschedulable.get());
	}

	//-----------------------------------------------------------------------------
	// schedulable (2)
	//-----------------------------------------------------------------------------
	{
		// 1) asio::schedulable::Nexecutable_function<> 객체를 할당받습니다.
		auto pschedulable = std::make_shared<asio::schedulable::executable<>>();

		// 2) 실행할 함수를 설정합니다.(lambda, static function, std::bind 모두 가능합니다.)
		pschedulable->set_function([]() {
				std::cout << "schedulable(2) executed\n";
			});

		// 3) 2초 마다 한번씩 실행하도록 합니다.
		pschedulable->execute_interval(2s);

		// 4) schedulable을 시작합니다.
		asio::system::register_schedulable(pschedulable);

		// 5) 키입력을 기다립니다.
		_getch();

		// 6) schedulable을 중단합니다.
		asio::system::unregister_schedulable(pschedulable.get());
	}

	// 5) wait
	_getch();
}
 