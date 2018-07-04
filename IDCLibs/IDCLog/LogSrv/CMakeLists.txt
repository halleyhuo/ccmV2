include_directories( . ../inc ../../../inc )


set(SERVER_SRC_LIST
server.cpp /
cfg.cpp)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
add_executable( IDCLogSrv ${SERVER_SRC_LIST})
target_link_libraries(IDCLogSrv rt pthread)
