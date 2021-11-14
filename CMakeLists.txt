cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Isolinear VERSION 1.0)

set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  "${PROJECT_SOURCE_DIR}/cmake/"
)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

find_package(LibMPDClient REQUIRED)

add_library(LibMiso INTERFACE)
target_include_directories(LibMiso INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/include/miso
)

add_executable(Isolinear src/main.cpp)
target_link_libraries(Isolinear
  ${CONAN_LIBS}
  ${LIBMPDCLIENT_LIBRARY}
  LibMiso
  -lSDL2_gfx
  )

target_include_directories(Isolinear PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include/isolinear
)
