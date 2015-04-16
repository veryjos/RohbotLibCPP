# Install script for directory: C:/dev/rohbutt/repo/RohbotLibCPP/Source/ThirdParty/jsoncpp/src/JsonCPPBuild/include

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/jsoncpp")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/json" TYPE FILE FILES
    "C:/dev/rohbutt/repo/RohbotLibCPP/Source/ThirdParty/jsoncpp/src/JsonCPPBuild/include/json/assertions.h"
    "C:/dev/rohbutt/repo/RohbotLibCPP/Source/ThirdParty/jsoncpp/src/JsonCPPBuild/include/json/autolink.h"
    "C:/dev/rohbutt/repo/RohbotLibCPP/Source/ThirdParty/jsoncpp/src/JsonCPPBuild/include/json/config.h"
    "C:/dev/rohbutt/repo/RohbotLibCPP/Source/ThirdParty/jsoncpp/src/JsonCPPBuild/include/json/features.h"
    "C:/dev/rohbutt/repo/RohbotLibCPP/Source/ThirdParty/jsoncpp/src/JsonCPPBuild/include/json/forwards.h"
    "C:/dev/rohbutt/repo/RohbotLibCPP/Source/ThirdParty/jsoncpp/src/JsonCPPBuild/include/json/json.h"
    "C:/dev/rohbutt/repo/RohbotLibCPP/Source/ThirdParty/jsoncpp/src/JsonCPPBuild/include/json/reader.h"
    "C:/dev/rohbutt/repo/RohbotLibCPP/Source/ThirdParty/jsoncpp/src/JsonCPPBuild/include/json/value.h"
    "C:/dev/rohbutt/repo/RohbotLibCPP/Source/ThirdParty/jsoncpp/src/JsonCPPBuild/include/json/version.h"
    "C:/dev/rohbutt/repo/RohbotLibCPP/Source/ThirdParty/jsoncpp/src/JsonCPPBuild/include/json/writer.h"
    )
endif()

