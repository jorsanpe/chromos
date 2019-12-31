cmake_minimum_required (VERSION 3.11)

set(FRAMEWORK_PATH ${PROJECT_SOURCE_DIR}/test/framework)
set(UNITY_PATH ${FRAMEWORK_PATH}/unity)
set(CMOCK_PATH ${FRAMEWORK_PATH}/cmock)
set(MOCKS_PATH ${PROJECT_SOURCE_DIR}/test/unit/mocks)
set(TEST_INCLUDES  ${UNITY_PATH}/src ${UNITY_PATH}/extras/fixture/src ${CMOCK_PATH}/src ${MOCKS_PATH} ${CEXCEPTION_PATH}/lib)
add_library(unity ${UNITY_PATH}/src/unity.c ${UNITY_PATH}/extras/fixture/src/unity_fixture.c ${CMOCK_PATH}/src/cmock.c)
target_include_directories(unity PUBLIC ${TEST_INCLUDES})

if(DEFINED MOCK_HEADERS)
	foreach(mock_header ${MOCK_HEADERS})
		get_filename_component(mock_name ${mock_header} NAME_WE)
		
		add_custom_command(
			OUTPUT  ${MOCKS_PATH}/Mock${mock_name}.c
			OUTPUT  ${MOCKS_PATH}/Mock${mock_name}.h
			COMMAND ruby ${CMOCK_PATH}/lib/cmock.rb -o${PROJECT_SOURCE_DIR}/test/framework/cmock/cmock_conf.yml --mock_path="${MOCKS_PATH}" ${mock_header}
			DEPENDS ${mock_header}
		)
		set(MOCK_SOURCES ${MOCK_SOURCES} ${MOCKS_PATH}/Mock${mock_name}.c)
	endforeach()

	set(MOCK_TYPES_FILE "${MOCKS_PATH}/mock_types.h")
	file(WRITE ${MOCK_TYPES_FILE} "")
	foreach(mock_type ${MOCK_TYPES})
		file(APPEND ${MOCK_TYPES_FILE} "${mock_type} { unsigned long placeholder; };\n")
	endforeach()

	add_library(mocks ${MOCK_SOURCES})
	target_include_directories(mocks PUBLIC ${TEST_INCLUDES})

	string(REPLACE ";" " " UNIT_TEST_CFLAGS "${MOCK_CFLAGS} ${UNIT_TEST_CFLAGS}")
	set_target_properties(mocks PROPERTIES COMPILE_FLAGS "--include ${MOCK_TYPES_FILE} ${UNIT_TEST_CFLAGS}")
else()
	string(REPLACE ";" " " UNIT_TEST_CFLAGS "${UNIT_TEST_CFLAGS}")
endif()

set(UNIT_TESTS_LAUNCHER "run_unit_tests.sh")
file(WRITE ${UNIT_TESTS_LAUNCHER} "set -ex\n")

# UNIT TESTS
foreach(test_suite_package ${UNIT_TEST_SUITES})
	file(GLOB test_suite_package_files RELATIVE ${PROJECT_SOURCE_DIR}
		${test_suite_package}/test_*.c
	)
	foreach(test_file ${test_suite_package_files})
		get_filename_component(test_suite ${test_file} NAME_WE)
		set(unit_integration_test_suites 
			${unit_integration_test_suites}
			${test_suite_package}/${test_suite}
		)
	endforeach()
endforeach()

foreach(test_suite ${unit_integration_test_suites})
	get_filename_component(test_case ${test_suite} NAME)
	get_filename_component(test_suite_package ${test_suite} DIRECTORY)

	set(UNIT_TEST_RUNNER ${PROJECT_SOURCE_DIR}/${test_suite_package}/${test_case}.c ${test_suite_package}/${test_case}_runner.c)
	set(UNIT_TEST_FILES ${${test_case}_files} ${unit_tests_files})
	if(NOT "${${test_case}_exclude}" STREQUAL "")
		list(REMOVE_ITEM UNIT_TEST_FILES ${${test_case}_exclude})
	endif()
	set(UNIT_TEST_TARGET ${test_case}_runner)
	set(FRAMEWORK_FLAGS "${${test_case}_ldflags} --include unity_fixture_malloc_overrides.h")
	string(REPLACE ";" " " ${test_case}_cflags "${${test_case}_cflags}")
	
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${test_suite_package})
	  
	link_directories(${LINK_DIR})
	add_executable(${UNIT_TEST_TARGET} ${UNIT_TEST_RUNNER} ${UNIT_TEST_FILES} ${${test_case}_objects})
	target_include_directories(${UNIT_TEST_TARGET} PUBLIC ${TEST_INCLUDES} ${INC})

	set_target_properties(${UNIT_TEST_TARGET} PROPERTIES COMPILE_FLAGS "${${test_case}_cflags} ${UNIT_TEST_CFLAGS} ${CFLAGS} ${ARCH_C_FLAGS} ${FRAMEWORK_FLAGS} -Wno-format-extra-args -Wno-unused-result -Wformat=0 -fno-strict-aliasing")
	set_target_properties(${UNIT_TEST_TARGET} PROPERTIES LINK_FLAGS ${FRAMEWORK_FLAGS})
	if(DEFINED MOCK_HEADERS)
		target_link_libraries(${UNIT_TEST_TARGET} mocks)
	endif()
	target_link_libraries(${UNIT_TEST_TARGET} unity)
	target_link_libraries(${UNIT_TEST_TARGET} ${LIBS})
	
	add_custom_command(
	    OUTPUT  ${test_suite_package}/${test_case}_runner.c
	    COMMAND ruby ${UNITY_PATH}/auto/generate_test_runner.rb ${PROJECT_SOURCE_DIR}/${test_suite_package}/${test_case}.c ${test_suite_package}/${test_case}_runner.c > /dev/null 
	    DEPENDS ${test_suite_package}/${test_case}.c
	)
	add_custom_target(${test_case}
		COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${test_suite_package}/${UNIT_TEST_TARGET} 
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
		DEPENDS ${UNIT_TEST_TARGET}
	)
	
	set(UNIT_TEST_RUNNERS ${UNIT_TEST_RUNNERS} ${UNIT_TEST_TARGET})
	file(APPEND ${UNIT_TESTS_LAUNCHER} "${CMAKE_CURRENT_BINARY_DIR}/${test_suite_package}/${UNIT_TEST_TARGET}\n")
endforeach()

add_custom_target(unit
	COMMAND echo "> Done"
	DEPENDS ${UNIT_TEST_RUNNERS}
)

# INTEGRATION TESTS
set(INTEGRATION_TESTS_LAUNCHER "run_integration_tests.sh")
file(WRITE ${INTEGRATION_TESTS_LAUNCHER} "set -ex\n")

string(REPLACE ";" " " INTEGRATION_TESTS_CFLAGS "${INTEGRATION_TESTS_CFLAGS}")

if(DEFINED INTEGRATION_TESTS_FILES)
	add_library(integration_lib OBJECT ${INTEGRATION_TESTS_FILES})
	set_target_properties(integration_lib PROPERTIES COMPILE_FLAGS "${INTEGRATION_TESTS_CFLAGS} ${CFLAGS}")
	target_include_directories(integration_lib PUBLIC ${TEST_INCLUDES} ${INTEGRATION_TESTS_INCLUDES})
endif()

set(INTEGRATION_APP_FILES ${SRCS} ${SRCS_ARCH})
if(NOT "${INTEGRATION_TESTS_EXCLUDE}" STREQUAL "")
	list(REMOVE_ITEM INTEGRATION_APP_FILES ${INTEGRATION_TESTS_EXCLUDE})
endif()
add_library(app_lib ${INTEGRATION_APP_FILES})

foreach(test_suite_package ${INTEGRATION_TEST_SUITES})
	message("test_suite_package ${test_suite_package}")
	file(GLOB test_suite_package_files RELATIVE ${PROJECT_SOURCE_DIR}
		${test_suite_package}/test_*.c
	)
	foreach(test_file ${test_suite_package_files})
		message("  test_file ${test_file}")
		get_filename_component(test_suite ${test_file} NAME_WE)
		set(integration_test_suites 
			${integration_test_suites}
			${test_suite_package}/${test_suite}
		)
	endforeach()
endforeach()

foreach(test_suite ${integration_test_suites})
	get_filename_component(test_case ${test_suite} NAME)
	get_filename_component(test_suite_package ${test_suite} DIRECTORY)

	add_custom_command(
	    OUTPUT  ${test_suite_package}/${test_case}_runner.c
	    COMMAND ruby ${UNITY_PATH}/auto/generate_test_runner.rb ${PROJECT_SOURCE_DIR}/${test_suite_package}/${test_case}.c ${test_suite_package}/${test_case}_runner.c > /dev/null 
	    DEPENDS ${test_suite_package}/${test_case}.c
	)

	set(INTEGRATION_TEST_RUNNER ${PROJECT_SOURCE_DIR}/${test_suite_package}/${test_case}.c ${test_suite_package}/${test_case}_runner.c)
	set(INTEGRATION_TEST_FILES ${INTEGRATION_TEST_RUNNER} ${INTEGRATION_TESTS_FILES} ${${test_case}_integration_files})
	if(NOT "${${test_case}_exclude}" STREQUAL "")
		list(FILTER INTEGRATION_TEST_FILES EXCLUDE REGEX ${${test_case}_exclude})
	endif()
	set(INTEGRATION_TEST_TARGET ${test_case}_integration_runner)
	string(REPLACE ";" " " ${test_case}_cflags "${${test_case}_cflags}")

	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${test_suite_package})
	 
	link_directories(${LINK_DIR})
	add_executable(${INTEGRATION_TEST_TARGET} ${INTEGRATION_TEST_FILES})
	target_include_directories(${INTEGRATION_TEST_TARGET} PUBLIC ${INTEGRATION_TESTS_INCLUDES} ${TEST_INCLUDES} ${INC} ${${test_case}_includes})

	set_target_properties(${INTEGRATION_TEST_TARGET} PROPERTIES COMPILE_FLAGS "${${test_case}_cflags} ${INTEGRATION_TESTS_CFLAGS} ${CFLAGS} ${ARCH_C_FLAGS} ${FRAMEWORK_FLAGS} -Wno-format-extra-args -Wno-unused-result -Wformat=0 -fno-strict-aliasing")
	set_target_properties(${INTEGRATION_TEST_TARGET} PROPERTIES LINK_FLAGS ${FRAMEWORK_FLAGS})
	target_link_libraries(${ACCEPTANCE_TEST_TARGET} app_lib)
	target_link_libraries(${INTEGRATION_TEST_TARGET} unity)
	if(DEFINED INTEGRATION_TESTS_FILES)
		target_link_libraries(${INTEGRATION_TEST_TARGET} integration_lib)
	endif()
	target_link_libraries(${INTEGRATION_TEST_TARGET} ${LIBS})

	add_custom_target(${test_case}_integration
		COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${test_suite_package}/${INTEGRATION_TEST_TARGET} 
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
		DEPENDS ${INTEGRATION_TEST_TARGET}
	)

	set(INTEGRATION_TEST_RUNNERS ${INTEGRATION_TEST_RUNNERS} ${INTEGRATION_TEST_TARGET})
	file(APPEND ${INTEGRATION_TESTS_LAUNCHER} "${CMAKE_CURRENT_BINARY_DIR}/${test_suite_package}/${INTEGRATION_TEST_TARGET}\n")
endforeach()

add_custom_target(integration
	COMMAND echo "> Done"
	DEPENDS ${INTEGRATION_TEST_RUNNERS}
)
