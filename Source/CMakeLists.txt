cmake_minimum_required (VERSION 2.8.7)
project (RohbotLibCPP)

include(ExternalProject)

if (NOT WIN32)
	include(/usr/local/share/cmake-2.8/Modules/ExternalProject.cmake)
	set(CMAKE_CXX_FLAGS "-sx=c++11")
else()
	set(CMAKE_CXX_FLAGS "/EHsc")
endif()

externalproject_add (JsonCPPBuild
	PREFIX ${CMAKE_BINARY_DIR}/ThirdParty/jsoncpp
	GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp
	INSTALL_DIR ${CMAKE_BINARY_DIR}/ThirdParty/install/jsoncpp
)

include_directories (
	${CMAKE_BINARY_DIR}/ThirdParty/jsoncpp/src/JsonCPPBuild/include
	${CMAKE_BINARY_DIR}/ThirdParty/libwebsockets/include
)

add_library (JsonCPP STATIC IMPORTED)

if (CMAKE_BUILD_TYPE MATCHES DEBUG)
	set_target_properties (JsonCPP
		PROPERTIES IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/ThirdParty/jsoncpp/src/JsonCPP-build/src/lib_json/Debug/jsoncpp.lib
	)
else()
	set_target_properties (JsonCPP
		PROPERTIES IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/ThirdParty/jsoncpp/src/JsonCPP-build/src/lib_json/Release/jsoncpp.lib
	)
endif()

add_library (RohbotLibCPP
	Client/RohbotClient.cpp
	Client/RohbotClient.hpp

	Packet/PacketHandler.cpp
	Packet/PacketHandler.hpp
	Packet/AuthPacket.hpp
	Packet/BasePacket.hpp
	Packet/ChatPacket.hpp

	Room/Chatroom.cpp
	Room/Chatroom.hpp

	User/RohbotUser.cpp
	User/RohbotUser.hpp

	Websocket/Websocket.cpp
	Websocket/Websocket.hpp
)

add_dependencies (RohbotLibCPP
	JsonCPP
)

SOURCE_GROUP (Client
FILES Client/RohbotClient.cpp
	  Client/RohbotClient.hpp
)

SOURCE_GROUP (Packet
FILES Packet/PacketHandler.cpp
	  Packet/PacketHandler.hpp
	  Packet/AuthPacket.hpp
	  Packet/BasePacket.hpp
	  Packet/ChatPacket.hpp
)

SOURCE_GROUP (Room
FILES Room/Chatroom.cpp
	  Room/Chatroom.hpp
)

SOURCE_GROUP (User
FILES User/RohbotUser.cpp
	  User/RohbotUser.hpp
)

SOURCE_GROUP (Websocket
FILES Websocket/Websocket.cpp
	  Websocket/Websocket.hpp
)

target_link_libraries (RohbotLibCPP
	JsonCPP
)