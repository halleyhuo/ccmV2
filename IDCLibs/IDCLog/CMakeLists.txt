project( IDCLogS )
cmake_minimum_required(VERSION 3.0)
include( ../../IDCCommon.cmake )

include_directories( ./inc ../../inc)

set(SRC_LIST src/LogTrace.cpp)

add_library(IDCLog STATIC ${SRC_LIST})

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR})

add_subdirectory(LogSrv)

#target_link_libraries(IDCLogSrv rt pthread)
