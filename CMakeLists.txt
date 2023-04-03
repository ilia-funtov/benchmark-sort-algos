cmake_minimum_required(VERSION 3.14)
project(benchmark-sort-algos)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND SRCFILES src/main.cpp)

add_executable(benchmark-sort-algos ${SRCFILES})

set_target_properties(benchmark-sort-algos PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_include_directories(benchmark-sort-algos PRIVATE include)

if (MSVC)
    target_compile_options(benchmark-sort-algos PRIVATE /W4)
else()
    target_compile_options(benchmark-sort-algos PRIVATE -Wall)
endif()

find_package(Boost REQUIRED)
if(Boost_FOUND)
    target_include_directories(benchmark-sort-algos SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})
    target_link_libraries(benchmark-sort-algos PRIVATE ${Boost_LIBRARIES})
else()
    message(FATAL_ERROR "Required boost libraries are not found")
endif()

Include(FetchContent)

set(CATCH2_GIT_REPOSITORY "https://github.com/catchorg/Catch2.git")
set(CATCH2_VERSION_TAG "v2.13.10")

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY ${CATCH2_GIT_REPOSITORY}
  GIT_TAG        ${CATCH2_VERSION_TAG}
)

message(STATUS "Fetching Catch2 from ${CATCH2_GIT_REPOSITORY} with tag ${CATCH2_VERSION_TAG}")
FetchContent_MakeAvailable(Catch2)

if((DEFINED Catch2_SOURCE_DIR) AND (EXISTS ${Catch2_SOURCE_DIR}))
    set(CATCH2_INCLUDE_DIR ${Catch2_SOURCE_DIR}/single_include)
    message(STATUS "Using Catch2 from ${CATCH2_INCLUDE_DIR}")
    target_include_directories(benchmark-sort-algos PRIVATE ${CATCH2_INCLUDE_DIR})
else()
    message(FATAL_ERROR "Could not fetch Catch2")
endif()

find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
if ((DEFINED CLANG_TIDY_EXE) AND (EXISTS ${CLANG_TIDY_EXE}))
    message(STATUS "Using clang-tidy for linting")
    
    add_executable(benchmark-sort-algos-lint ${SRCFILES})
    set_target_properties(benchmark-sort-algos-lint PROPERTIES EXCLUDE_FROM_ALL 1)

    set_target_properties(benchmark-sort-algos-lint PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
    )

    if (MSVC)
        target_compile_options(benchmark-sort-algos-lint PRIVATE /W4)
    else()
        target_compile_options(benchmark-sort-algos-lint PRIVATE -Wall -Wextra -pedantic)
    endif()

    target_include_directories(benchmark-sort-algos-lint PRIVATE include)
    
    target_include_directories(benchmark-sort-algos-lint SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})
    target_link_libraries(benchmark-sort-algos-lint PRIVATE ${Boost_LIBRARIES})

    if (DEFINED CATCH2_INCLUDE_DIR)
        target_include_directories(benchmark-sort-algos-lint PRIVATE ${CATCH2_INCLUDE_DIR})
    endif()

    add_custom_target(lint DEPENDS benchmark-sort-algos-lint)

    set(CLANG_TIDY_COMMAND "${CLANG_TIDY_EXE}" "--checks=clang-analyzer-cplusplus*,concurrency-*,bugprone-*,boost-*,cppcoreguidelines-*")
    set_target_properties(benchmark-sort-algos-lint PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
endif()