cmake_minimum_required(VERSION 2.8.4)

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )
IF(UNIX)
 link_libraries(m)
ENDIF(UNIX)

project (falcon) 
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
SET(RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
SET(CMAKE_BUILD_TYPE Release)

add_executable (FALCON falcon.c mem.c time.c msg.c parser.c common.c buffer.c levels.c models.c top.c defs.h param.h) 
add_executable (FALCON-FILTER falcon-filter.c mem.c time.c msg.c parser.c common.c buffer.c models.c filters.c defs.h param.h) 
add_executable (FALCON-EYE falcon-eye.c mem.c time.c msg.c parser.c common.c buffer.c paint.c defs.h param.h) 
TARGET_LINK_LIBRARIES(FALCON pthread)
TARGET_LINK_LIBRARIES(FALCON-FILTER)
TARGET_LINK_LIBRARIES(FALCON-EYE)

