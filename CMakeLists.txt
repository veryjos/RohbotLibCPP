cmake_minimum_required (VERSION 2.8.7)
project (RohbotLibCPP)

include(ExternalProject)

if (NOT WIN32)
	set(CMAKE_CXX_FLAGS "-std=c++11")
else()
	set(CMAKE_CXX_FLAGS "/EHsc")
endif()

externalproject_add (libwebsocketsBuild
	PREFIX ${CMAKE_BINARY_DIR}/ThirdParty/libwebsockets
	GIT_REPOSITORY https://github.com/warmcat/libwebsockets
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/ThirdParty/libwebsockets/install
	INSTALL_DIR ${CMAKE_BINARY_DIR}/ThirdParty/libwebsockets/install
	)

externalproject_add (JsonCPPBuild
	PREFIX ${CMAKE_BINARY_DIR}/ThirdParty/jsoncpp
	GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp
	CMAKE_ARGS -DJSONCPP_WITH_TESTS=OFF -DJSONCPP_POST_BUILD_UNITTEST=OFF -DJSONCPP_WITH_PKGCONFIG_SUPPORT=OFF -DJSONCPP_BUILD_SHARED_LIBS=OFF -DJSONCPP_BUILD_STATIC_LIBS=ON -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/ThirdParty/jsoncpp/install
	INSTALL_DIR ${CMAKE_BINARY_DIR}/ThirdParty/jsoncpp/install
	)

add_library (RohbotLib
	Source/Client/RohbotClient.cpp
	Source/Client/RohbotClient.hpp

	Source/Packet/PacketHandler.cpp
	Source/Packet/PacketHandler.hpp
	Source/Packet/AuthPacket.hpp
	Source/Packet/BasePacket.hpp
	Source/Packet/ChatPacket.hpp

	Source/Room/Chatroom.cpp
	Source/Room/Chatroom.hpp

	Source/User/RohbotUser.cpp
	Source/User/RohbotUser.hpp

	Source/Websocket/Websocket.cpp
	Source/Websocket/Websocket.hpp
)

SOURCE_GROUP (Client
FILES Source/Client/RohbotClient.cpp
	  Source/Client/RohbotClient.hpp
)

SOURCE_GROUP (Packet
FILES Source/Packet/PacketHandler.cpp
	  Source/Packet/PacketHandler.hpp
	  Source/Packet/AuthPacket.hpp
	  Source/Packet/BasePacket.hpp
	  Source/Packet/ChatPacket.hpp
)

SOURCE_GROUP (Room
FILES Source/Room/Chatroom.cpp
	  Source/Room/Chatroom.hpp
)

SOURCE_GROUP (User
FILES Source/User/RohbotUser.cpp
	  Source/User/RohbotUser.hpp
)

SOURCE_GROUP (Websocket
FILES Source/Websocket/Websocket.cpp
	  Source/Websocket/Websocket.hpp
)

add_dependencies (RohbotLib
	JsonCPPBuild
	libwebsocketsBuild
)

target_link_libraries (RohbotLib
	JsonCPP
	websockets
)

target_include_directories (RohbotLib PUBLIC
	${CMAKE_SOURCE_DIR}/Source
	${CMAKE_BINARY_DIR}/ThirdParty/jsoncpp/install/include
	${CMAKE_BINARY_DIR}/ThirdParty/libwebsockets/install/include
)


install (TARGETS RohbotLib DESTINATION lib)
