cmake_minimum_required(VERSION 3.10)
project(CachingPolicies VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -DPROFILE -static")

if(NOT DISABLE_JEMALLOC)
    if(JEMALLOC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DROCKSDB_JEMALLOC -DJEMALLOC_NO_DEMANGLE ${JEMALLOC_INCLUDE}")
        list(APPEND EXEC_LDFLAGS "${JEMALLOC_LIB}" "-lpthread")
    else()
        list(APPEND EXEC_LDFLAGS "-lpthread")
    endif()
else()
    list(APPEND EXEC_LDFLAGS "-lpthread")
endif()

option(WITH_TESTS "Build with tests" OFF)
option(WITH_TOOLS "Build with tools" OFF)
option(WITH_ALL_TESTS "Build with all tests" OFF)
option(WITH_BENCHMARK_TOOLS "Build with benchmark tools" OFF)
option(WITH_CORE_TOOLS "Build with core tools" OFF)
option(WITH_TRACE_TOOLS "Build with trace tools" OFF)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/rocksdb/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/workload-generator/include)

add_subdirectory(lib/rocksdb)
add_subdirectory(lib/workload-generator)

set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/working_version.cc
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

add_executable(working_version ${SOURCES})

target_link_libraries(working_version
        ${CMAKE_BINARY_DIR}/lib/rocksdb/librocksdb.a
        ${EXEC_LDFLAGS}
        shlwapi
        rpcrt4
)

add_dependencies(working_version rocksdb)

target_compile_definitions(working_version PRIVATE)