#*****************************************************************************
#*                                                                           *
#*                      Cho sanghyun's Game Classes II                       *
#*                                asio classes                               *
#*                                                                           *
#*                                   CMake                                   *
#*                                                                           *
#*                                                                           *
#*                                                                           *
#*                                                                           *
#*  This Program is programmed by Cho SangHyun. sangduck@cgcii.co.kr         *
#*  Best for Game Developement and Optimized for Game Developement.          *
#*                                                                           *
#*                (c) 2016 Cho Sanghyun All right reserved.                  *
#*                          http://www.CGCII.co.kr                           *
#*                                                                           *
#*****************************************************************************

# ----------------------------------------------------------------------------
# setting
# ----------------------------------------------------------------------------
# 1) project name
PROJECT ( "cgcii.asio" )

# 2) project version
SET ( PROJECT_VERSION_MAJOR 10 )
SET ( PROJECT_VERSION_MINOR 0 )
SET ( PROJECT_VERSION_PATCH 100 )
SET ( PROJECT_VERSION_TWEAK 1 )
SET ( DEFAULT_BUILD_TYPE Debug )

# 3) build version
CMAKE_MINIMUM_REQUIRED ( VERSION 3.20 )
CMAKE_POLICY( VERSION 3.20 )

# 4) default BUILD_TYPE
IF (NOT DEFINED CMAKE_BULD_TYPE)
	SET (CMAKE_BULD_TYPE "Debug")
ENDIF()

# 5) asio
SET(Boost_USE_STATIC_LIBS OFF)
SET(Boost_USE_MULTITHREAD ON)
FIND_PACKAGE(Boost 1.71.0 REQUIRED COMPONENTS system thread)
IF(Boost_FOUND)
  INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
  LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
ENDIF(Boost_FOUND)
SET(USED_LIBS ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_REGEX_LIBRARY} ${Boost_ASIO_LIBRARY})

# 6) directories
SET ( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )
SET ( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )
SET ( CMAKE_RUNTIME_OTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )
SET ( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR} )

SET ( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../lib/cgdk/sdk10 )
SET ( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../lib/cgdk/sdk10 )
SET ( CMAKE_RUNTIME_OTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../execute )
SET ( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../execute )


# 7) compile options
set ( CMAKE_CXX_STANDARD 20)
set ( CMAKE_CXX_STANDARD_REQUIRED ON)
SET ( CMAKE_CXX_FLAGS "-DC_FLAGS -fexceptions -std=c++20" )
SET ( CMAKE_CXX_FLAGS_DEBUG "-D_DEBUG -g3 -Wall -Wextra" )
SET ( CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3 -Wall -Wextra" )

# 8) include directories
INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}/../include
	./
)


# ----------------------------------------------------------------------------
# source files
# ----------------------------------------------------------------------------
SET (SRC_FILES

# core (asio)
asio.Nstatistics.cpp
asio.Nconnective.cpp
asio.Nconnector.cpp
asio.Nconnect_requestable.cpp
asio.Nacceptor.cpp
asio.Isocket_tcp.cpp
asio.Nsocket_tcp.cpp
asio.Nsocket_tcp_gather.cpp
asio.Nsocket_tcp_gather_buffer.cpp
asio.system.cpp
)


# ----------------------------------------------------------------------------
# compile
# ----------------------------------------------------------------------------
# 1) set variable
IF (${CMAKE_BUILD_TYPE} MATCHES "Debug")
	SET (BUILD_MESSAGE "Debug")
	SET (BUILD_TYPE "Debug")
ELSEIF (${CMAKE_BUILD_TYPE} MATCHES "Release")
	SET (BUILD_MESSAGE "Release")
	SET (BUILD_TYPE "Release")
ELSE ()
	MESSAGE ("CMAKE_BUILD_TYPE is not defined")
ENDIF ()

# 2) messages
MESSAGE (STATUS "compile ${PROJECT_NAME}")
MESSAGE (STATUS "  - configure['${BUILD_MESSAGE}']")

# 3) add library
ADD_LIBRARY(${PROJECT_NAME}_linux.${BUILD_TYPE} STATIC ${SRC_FILES} ${SRC_FILES_ZLIB} ${SRC_FILES_FMT})
