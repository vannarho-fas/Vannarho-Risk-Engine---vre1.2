cmake_minimum_required(VERSION 3.15)
add_compile_options(
    -Wno-error=unused-variable
    -Wno-error=unused-but-set-variable
    -Wno-unused-private-field
    -Wno-unused-but-set-variable
)


cmake_policy(SET CMP0091 NEW)


project(ORE CXX)

option(ORE_BUILD_DOC "Build documentation" ON)
option(ORE_BUILD_EXAMPLES "Build examples" ON)
option(ORE_BUILD_TESTS "Build test suite" ON)
option(ORE_BUILD_APP "Build app" ON)
option(ORE_USE_ZLIB "Use compression for boost::iostreams" OFF)

include(CTest)

# Path for local cmake modules
if(NOT "${CMAKE_CURRENT_LIST_DIR}/cmake" IN_LIST CMAKE_MODULE_PATH)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
endif()
include(commonSettings)

set(USE_GLOBAL_ORE_BUILD ON)

# work around boost issue https://bugs.freebsd.org/bugzilla/show_bug.cgi?id=271488
set(CMAKE_CXX_STANDARD 14)
add_subdirectory("QuantLib")
set(CMAKE_CXX_STANDARD 17)

add_subdirectory("QuantExt")
add_subdirectory("OREData")
add_subdirectory("OREAnalytics")
if (ORE_BUILD_APP)
    add_subdirectory("App")
endif()

# add examples testsuite
if (ORE_BUILD_EXAMPLES AND ORE_BUILD_TESTS)
add_test(NAME examples WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Examples COMMAND nosetests run_examples_testsuite.py)
endif()
