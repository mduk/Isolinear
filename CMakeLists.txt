cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Isolinear
  VERSION 1.0
  LANGUAGES CXX
)

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

add_library(LibMPDXX INTERFACE)
target_include_directories(LibMPDXX INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/include/mpdxx
)

add_executable(IsoMPD src/isompd.cpp)
target_link_libraries(IsoMPD
  ${CONAN_LIBS}
  ${LIBMPDCLIENT_LIBRARY}
  LibMiso
  LibMPDXX
  -lSDL2_gfx
  )

target_include_directories(IsoMPD PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include/isolinear
)

