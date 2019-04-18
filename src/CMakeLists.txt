cmake_minimum_required(VERSION 2.8.4)

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra ${GCC_COVERAGE_COMPILE_FLAGS}" )
IF(UNIX)
 link_libraries(m)
ENDIF(UNIX)

project (falcon) 
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
SET(RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
SET(CMAKE_BUILD_TYPE Release)

add_executable (FALCON falcon.c mem.c time.c msg.c parser.c common.c buffer.c stream.c levels.c models.c pmodels.c kmodels.c top.c defs.h param.h) 
add_executable (FALCON-filter falcon-filter.c mem.c time.c msg.c parser.c common.c buffer.c pmodels.c models.c kmodels.c filters.c defs.h param.h) 
add_executable (FALCON-filter-visual falcon-filter-visual.c mem.c time.c msg.c parser.c common.c buffer.c labels.c paint.c defs.h param.h) 
add_executable (FALCON-inter falcon-inter.c mem.c time.c msg.c parser.c common.c buffer.c levels.c pmodels.c kmodels.c models.c defs.h param.h)
add_executable (FALCON-inter-visual falcon-inter-visual.c mem.c msg.c parser.c common.c buffer.c paint.c defs.h)

TARGET_LINK_LIBRARIES(FALCON pthread)
TARGET_LINK_LIBRARIES(FALCON-filter)
TARGET_LINK_LIBRARIES(FALCON-filter-visual)
TARGET_LINK_LIBRARIES(FALCON-inter pthread)
TARGET_LINK_LIBRARIES(FALCON-inter-visual)
