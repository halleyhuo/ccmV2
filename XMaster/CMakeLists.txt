project( XMaster )
cmake_minimum_required(VERSION 3.0)
include( ../IDCCommon.cmake )

include_directories( ./inc ../inc ../IDCLibs)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

set(IDC_LIBS_PATH ../IDCLibs)

find_library(LIB_LOG
        IDCLog
        ${IDC_LIBS_PATH}/IDCLog/build
)

find_library(LIB_BASE
        IDCBase
        ${IDC_LIBS_PATH}/IDCBase/build
)

find_library(LIB_XTPCLIENT
        XTPClientIf
        ${IDC_LIBS_PATH}/IDCXTP/build
)

add_executable( XMaster
src/Init.cpp
src/XGenerator.cpp
src/XMaster.cpp
src/XTui.cpp)

target_link_libraries( XMaster ${LIB_LOG} ${LIB_XTPCLIENT} ${LIB_BASE} rt pthread )
