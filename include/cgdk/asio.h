//*****************************************************************************
//*                                                                           *
//*                      Cho sanghyun's Game Classes II                       *
//*                                                                           *
//*                           asio network classes                            *
//*                                                                           *
//*                                                                           *
//*                                                                           *
//*                                                                           *
//*  This Program is programmed by Cho SangHyun. sangduck@cgcii.co.kr         *
//*  Best for Game Developement and Optimized for Game Developement.          *
//*                                                                           *
//*                (c) 2019. Cho Sanghyun. All right reserved.                *
//*                          http://www.CGCII.co.kr                           *
//*                                                                           *
//*****************************************************************************
#pragma once

#ifdef _WIN32
	#pragma warning(disable:4250 6001 6255 6258 6387 26439 26451 26452 26495 26498)
#endif

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/buffer.hpp>
#include <set>
#include <list>
#include <memory>
#include <mutex>
#include <thread>

#ifdef _WIN32
	#pragma warning(default:6001 6255 6258 6387 26439 26451 26452 26495 26498)
#endif

#include "cgdk/shared_buffer"

using namespace boost::asio::ip;
using boost::asio::mutable_buffer;
using boost::asio::const_buffer;
using namespace CGDK;

namespace CGDK
{
	namespace asio
	{
		class Isocket_tcp;

		class Nstatistics;
		class Nconnective;
		class Nacceptor;
		class Nconnector;
		class Nconnect_requestable;
		class Nsocket_tcp;
		class Nsocket_tcp_gather;
		class Nsocket_tcp_gather_buffer;
		class Nsocket_tcp_client;

		template <class> class acceptor;
		template <class> class connector;

		class system;
	}
	const size_t RECEIVE_BUFFER_SIZE = 8192;
}

#include "asio/definitions.h"
#include "asio/Nstatistics.h"
#include "asio/Isocket_tcp.h"
#include "asio/Nsocket_tcp.h"
#include "asio/Nsocket_tcp_gather.h"
#include "asio/Nsocket_tcp_gather_buffer.h"
#include "asio/Nconnect_requestable.h"
#include "asio/Nsocket_tcp_client.h"
#include "asio/Nconnective.h"
#include "asio/Nacceptor.h"
#include "asio/Nconnector.h"
#include "asio/acceptor.h"
#include "asio/connector.h"
#include "asio/system.h"


//----------------------------------------------------------------------------
//
// libraries
//
//----------------------------------------------------------------------------
#if defined(_MSC_VER)
	#if !defined(_LIB) || defined(_CONSOLE)

	#if defined(WDK_NTDDI_VERSION)
		// check) 
		#if !defined(_M_X64)
			#error "[cgdk.asio] No WIN32-x86. Please use x64"
		#endif

		// check) 
		#ifndef _DLL 
			#error "[cgdk.asio] No Multi-Thread(/MT) or Multi-Thread Debug(/MTd). use Multi-thread DLL(/MD) or Multi-thread DLL Debug(/MDd)"
		#endif

		// WinSDK 10
		#if WDK_NTDDI_VERSION >= NTDDI_WIN10
			#ifdef NDEBUG
				#pragma message("[cgdk.asio] 'cgdk.asio_Windows10.0_x64_Release.lib' will be linked")
				#pragma comment(lib, "cgdk/asio/cgdk.asio_Windows10.0_x64_Release.lib")
			#else
				#pragma message("[cgdk.asio] 'cgdk.asio_Windows10.0_x64_Debug.lib' will be linked")
				#pragma comment(lib, "cgdk/asio/cgdk.asio_Windows10.0_x64_Debug.lib")
			#endif
		// Error - No supported
		#else
			#error "[cgdk.asio] Not supported Windonws SDK version (Need Windows SDK 10.0 or higher)"
		#endif

	#else
		#error "[cgdk.asio] Not supported platform toolset (require visual studio 2019 or higher)"
	#endif
	#endif

#elif defined(__ANDROID__)
#elif defined(__APPLE__)
#elif defined(__linux__)
#else
		#error "[cgdk.asio] Not supported platform"
#endif
