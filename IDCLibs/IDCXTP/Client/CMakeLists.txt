
find_library(LIB_LOG
	IDCLog
	../../IDCLog/build
)
find_library(LIB_BASE
	IDCBase
	../../IDCBase/build
)

add_library(XTPClientIf STATIC src/xtpClient.cpp )
target_link_libraries(XTPClientIf m)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR})
