project( IDCBase )
cmake_minimum_required(VERSION 3.0)
include( ../../IDCCommon.cmake )

include_directories(./inc ../../inc ../IDCLog/inc)


set(src_list 
src/IDCMsgQueue.cpp
src/IDCThread.cpp
src/IDCSema.cpp
src/IDCAlarm.cpp )

add_library(IDCBase STATIC ${src_list})


set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/)

target_link_libraries(IDCBase m)

