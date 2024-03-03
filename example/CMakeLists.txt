#*****************************************************************************
#*                                                                           *
#*                      Cho sanghyun's Game Classes II                       *
#*                                 CGDK.asio                                 *
#*                                                                           *
#*                              CMake - example                              *
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

#-----------------------------------------------------------------------------
# setting
#-----------------------------------------------------------------------------
# 1) build version
CMAKE_MINIMUM_REQUIRED ( VERSION 3.20 )
CMAKE_POLICY( VERSION 3.20 )

# 2) project name
PROJECT ( "CGDK.asio.example" )

# 3) project version
SET ( PROJECT_VERSION_MAJOR 10 )
SET ( PROJECT_VERSION_MINOR 0 )
SET ( PROJECT_VERSION_PATCH 100 )
SET ( PROJECT_VERSION_TWEAK 1 )
SET ( DEFAULT_BUILD_TYPE Debug )


#-----------------------------------------------------------------------------
# compile
#-----------------------------------------------------------------------------
# 1) simple tcp server/client
ADD_SUBDIRECTORY (1.simple)

# 2) tcp echo server/client
ADD_SUBDIRECTORY (2.tcp_echo_test)

# 3) simple chatting server/client
ADD_SUBDIRECTORY (3.simple_chatting)

# 4) execute - system post & dispatch
ADD_SUBDIRECTORY (4.execute)

