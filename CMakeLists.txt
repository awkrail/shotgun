cmake_minimum_required(VERSION 3.16)
project(shotgun CXX)

include(FetchContent)

set(CMAKE_CXX_STANDARD 17)

# use onnxruntime 1.20.1
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
    set(ORT_URL "https://github.com/microsoft/onnxruntime/releases/download/v1.20.1/onnxruntime-linux-x64-1.20.1.tgz")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin" AND CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")
    set(ORT_URL "https://github.com/microsoft/onnxruntime/releases/download/v1.20.1/onnxruntime-osx-arm64-1.20.1.tgz")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(ORT_URL "https://github.com/microsoft/onnxruntime/releases/download/v1.20.1/onnxruntime-osx-x86_64-1.20.1.tgz")
else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_PROCESSOR}")
endif()

FetchContent_Declare(onnxruntime URL ${ORT_URL})
FetchContent_MakeAvailable(onnxruntime)

add_executable(shotgun
    src/main.cpp
    src/transnetv2.cpp
)

target_include_directories(shotgun PRIVATE
    include/
    ${onnxruntime_SOURCE_DIR}/include
)

target_link_directories(shotgun PRIVATE
    ${onnxruntime_SOURCE_DIR}/lib
)

target_link_libraries(shotgun PRIVATE
    onnxruntime
)


#cmake_minimum_required(VERSION 3.14)

#project(shutoh VERSION 0.1)

#set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_STANDARD_REQUIRED True)
#set(CMAKE_POSITION_INDEPENDENT_CODE ON)
#set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

#include(FetchContent)
#FetchContent_Declare(
#    argparse
#    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
#)
#FetchContent_MakeAvailable(argparse)

#include(FetchContent)
#FetchContent_Declare(
#  pybind11
#  GIT_REPOSITORY https://github.com/pybind/pybind11.git
#  GIT_TAG        v2.11.1
#)
#FetchContent_MakeAvailable(pybind11)

#find_package(OpenCV REQUIRED)
#find_package(fmt REQUIRED)
#find_package(Python3 REQUIRED COMPONENTS Interpreter Development)

# Library
#if (EXISTS "${CMAKE_SOURCE_DIR}/src")
#  file(GLOB LIB_SOURCES 
#    "${CMAKE_SOURCE_DIR}/src/*.cpp"
#    "${CMAKE_SOURCE_DIR}/src/detector/*.cpp"
#    "${CMAKE_SOURCE_DIR}/src/bindings/*.cpp")
#else()
#  FetchContent_MakeAvailable(shutoh)
#  file(GLOB LIB_SOURCES
#    "${shutoh_SOURCE_DIR}/src/*.cpp"
#    "${shutoh_SOURCE_DIR}/src/detector/*.cpp"
#    "${shutoh_SOURCE_DIR}/src/bindings/*.cpp"
#  )
#  set(SKIP_TESTS TRUE)
#endif()
#add_library(shutoh SHARED ${LIB_SOURCES})
#target_compile_options(shutoh PRIVATE -O3 -Wall)
#target_include_directories(shutoh PUBLIC include ${OpenCV_INCLUDE_DIRS} ${Python3_INCLUDE_DIRS})
#target_link_libraries(shutoh PUBLIC ${OpenCV_LIBS} fmt::fmt argparse pybind11::module ${Python3_LIBRARIES})

# CLI
#add_executable(shutoh_cli src/main.cpp)
#set_target_properties(shutoh_cli PROPERTIES OUTPUT_NAME "shutoh")
#target_link_libraries(shutoh_cli PRIVATE shutoh)

# Tests
#if (NOT SKIP_TESTS)
#  enable_testing()
#  add_subdirectory(tests)

  # Install
  #  install(TARGETS shutoh DESTINATION lib)
  #install(TARGETS shutoh_cli DESTINATION bin)
  #install(DIRECTORY include/ DESTINATION include)
  #endif()
