if("master" STREQUAL "")
  message(FATAL_ERROR "Tag for git checkout should not be empty.")
endif()

set(run 0)

if("C:/dev/rohbutt/repo/RohbotLibCPP/Source/ThirdParty/jsoncpp/src/JsonCPPBuild-stamp/JsonCPPBuild-gitinfo.txt" IS_NEWER_THAN "C:/dev/rohbutt/repo/RohbotLibCPP/Source/ThirdParty/jsoncpp/src/JsonCPPBuild-stamp/JsonCPPBuild-gitclone-lastrun.txt")
  set(run 1)
endif()

if(NOT run)
  message(STATUS "Avoiding repeated git clone, stamp file is up to date: 'C:/dev/rohbutt/repo/RohbotLibCPP/Source/ThirdParty/jsoncpp/src/JsonCPPBuild-stamp/JsonCPPBuild-gitclone-lastrun.txt'")
  return()
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND} -E remove_directory "C:/dev/rohbutt/repo/RohbotLibCPP/Source/ThirdParty/jsoncpp/src/JsonCPPBuild"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to remove directory: 'C:/dev/rohbutt/repo/RohbotLibCPP/Source/ThirdParty/jsoncpp/src/JsonCPPBuild'")
endif()

# try the clone 3 times incase there is an odd git clone issue
set(error_code 1)
set(number_of_tries 0)
while(error_code AND number_of_tries LESS 3)
  execute_process(
    COMMAND "C:/Program Files (x86)/Git/cmd/git.exe" clone "https://github.com/open-source-parsers/jsoncpp" "JsonCPPBuild"
    WORKING_DIRECTORY "C:/dev/rohbutt/repo/RohbotLibCPP/Source/ThirdParty/jsoncpp/src"
    RESULT_VARIABLE error_code
    )
  math(EXPR number_of_tries "${number_of_tries} + 1")
endwhile()
if(number_of_tries GREATER 1)
  message(STATUS "Had to git clone more than once:
          ${number_of_tries} times.")
endif()
if(error_code)
  message(FATAL_ERROR "Failed to clone repository: 'https://github.com/open-source-parsers/jsoncpp'")
endif()

execute_process(
  COMMAND "C:/Program Files (x86)/Git/cmd/git.exe" checkout master
  WORKING_DIRECTORY "C:/dev/rohbutt/repo/RohbotLibCPP/Source/ThirdParty/jsoncpp/src/JsonCPPBuild"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to checkout tag: 'master'")
endif()

execute_process(
  COMMAND "C:/Program Files (x86)/Git/cmd/git.exe" submodule init
  WORKING_DIRECTORY "C:/dev/rohbutt/repo/RohbotLibCPP/Source/ThirdParty/jsoncpp/src/JsonCPPBuild"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to init submodules in: 'C:/dev/rohbutt/repo/RohbotLibCPP/Source/ThirdParty/jsoncpp/src/JsonCPPBuild'")
endif()

execute_process(
  COMMAND "C:/Program Files (x86)/Git/cmd/git.exe" submodule update --recursive 
  WORKING_DIRECTORY "C:/dev/rohbutt/repo/RohbotLibCPP/Source/ThirdParty/jsoncpp/src/JsonCPPBuild"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to update submodules in: 'C:/dev/rohbutt/repo/RohbotLibCPP/Source/ThirdParty/jsoncpp/src/JsonCPPBuild'")
endif()

# Complete success, update the script-last-run stamp file:
#
execute_process(
  COMMAND ${CMAKE_COMMAND} -E copy
    "C:/dev/rohbutt/repo/RohbotLibCPP/Source/ThirdParty/jsoncpp/src/JsonCPPBuild-stamp/JsonCPPBuild-gitinfo.txt"
    "C:/dev/rohbutt/repo/RohbotLibCPP/Source/ThirdParty/jsoncpp/src/JsonCPPBuild-stamp/JsonCPPBuild-gitclone-lastrun.txt"
  WORKING_DIRECTORY "C:/dev/rohbutt/repo/RohbotLibCPP/Source/ThirdParty/jsoncpp/src/JsonCPPBuild"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to copy script-last-run stamp file: 'C:/dev/rohbutt/repo/RohbotLibCPP/Source/ThirdParty/jsoncpp/src/JsonCPPBuild-stamp/JsonCPPBuild-gitclone-lastrun.txt'")
endif()

