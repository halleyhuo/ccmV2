project( XTP )
cmake_minimum_required(VERSION 3.0)
include( ../../IDCCommon.cmake )

include_directories( ./inc ../../inc ../ Server/inc Client/inc)

add_subdirectory(Server)
add_subdirectory(Client)

