cmake_minimum_required(VERSION 3.22)

project(calculator LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
	SimpleMath
	GIT_REPOSITORY https://github.com/RIO-Alex/SimpleMath.git
	GIT_TAG main
)

FetchContent_MakeAvailable(SimpleMath)

add_executable(${PROJECT_NAME} 
	src/main.cpp
)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

target_link_libraries(${PROJECT_NAME} PRIVATE SimpleMath)

set_target_properties(${PROJECT_NAME}
	PROPERTIES
		CXX_STANDARD 17
		CXX_STANDARD_REQUIRED ON
)

# CLang Tidy
if(USE_CLANG_TIDY)
	find_program(CLANG_TIDY_PATH NAMES clang-tidy-21 REQUIRED)
	message(STATUS "Found clang-tidy: ${CLANG_TIDY_PATH}")

	set(CLANG_TIDY_COMMAND ${CLANG_TIDY_PATH} "-config-file=${CMAKE_SOURCE_DIR}/.clang-tidy")
	set_target_properties(${PROJECT_NAME}
		PROPERTIES
			CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}"
	)
endif()

# CLang Format
if(USE_CLANG_FORMAT)
	find_program(CLANG_FORMAT_PATH NAMES clang-format-21 REQUIRED)
	message(STATUS "Found clang-format: ${CLANG_FORMAT_PATH}")

	file(GLOB_RECURSE CLANG_FORMAT_SRC CONFIGURE_DEPENDS
		src/*.h
    	src/*.cpp
    	src/*.hpp
	)

	add_custom_target(clang-format-fix
		COMMAND 
			${CLANG_FORMAT_PATH} -style=file -i ${CLANG_FORMAT_SRC}
		WORKING_DIRECTORY
			${CMAKE_SOURCE_DIR}
		COMMENT
			"Formatting ${PROJECT_NAME} with ${CLANG_FORMAT_PATH} ..."
	)

	add_custom_target(clang-format
        COMMAND
            run-clang-format.py --clang-format-executable=${CLANG_FORMAT_PATH} --style=file ${CLANG_FORMAT_SRC}
        WORKING_DIRECTORY
            ${CMAKE_SOURCE_DIR}
        COMMENT
            "Checking format ${PROJECT_NAME} with ${CLANG_FORMAT_PATH} ..."
    )

	add_dependencies(${PROJECT_NAME} clang-format)
endif()