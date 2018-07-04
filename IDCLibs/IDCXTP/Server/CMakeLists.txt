

find_library(LIB_LOG
	IDCLog
	../../IDCLog/build
)

find_library(LIB_BASE
	IDCBase
	../../IDCBase/build
)

add_executable( XTPServer 
src/xtpServer.cpp
src/ExtComm.cpp
src/msgqueue.cpp
)


target_link_libraries( XTPServer ${LIB_LOG} ${LIB_BASE} rt pthread )

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
