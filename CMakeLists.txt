cmake_minimum_required (VERSION 3.7)

set(NAME protock)
project(${NAME})

file(GLOB SRCS
	src/*.c
)

set(INC
	include
)

set(LINK_DIR
)

set(LIBS
)

set(CFLAGS
	"-Werror"
)	

set(CMAKE_C_FLAGS "${CFLAGS}")

include_directories(${INC})
link_directories(${LINK_DIR})

add_executable(${NAME} ${SRCS} ${SRCS_ARCH})

set_target_properties(${NAME} PROPERTIES COMPILE_FLAGS "${CFLAGS}")
target_link_libraries(${NAME} ${LIBS})

add_custom_command(
	TARGET ${NAME}
	POST_BUILD
	COMMAND COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${NAME}> ${PROJECT_SOURCE_DIR}/${NAME}
)

include(${PROJECT_SOURCE_DIR}/test/unit/CMakeLists.txt)
include(${PROJECT_SOURCE_DIR}/test/integration/CMakeLists.txt)
include(${PROJECT_SOURCE_DIR}/test/framework/CMakeLists.txt)



