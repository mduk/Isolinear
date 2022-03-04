cmake_minimum_required(VERSION 3.10)

set(CMAKE_BUILD_TYPE Debug)
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

add_library(LibMiso INTERFACE)
target_include_directories(LibMiso INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/include/miso
)

add_library(LibMPDXX INTERFACE)
target_include_directories(LibMPDXX INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/include/mpdxx
)

add_library(LibIsoMPD INTERFACE)
target_include_directories(LibIsoMPD INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/include/isompd
)

add_executable(IsoMPD src/isompd.cpp)
target_link_libraries(IsoMPD
  ${CONAN_LIBS}
  LibMiso
  LibMPDXX
  LibIsoMPD
  -lSDL2_gfx
)

add_executable(mpdxx src/mpdxx.cpp)
target_link_libraries(mpdxx
  ${CONAN_LIBS}
  LibMiso
  LibMPDXX
)

target_include_directories(IsoMPD PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include/isolinear
)

add_executable(timers src/timers.cpp)
target_link_libraries(timers
  ${CONAN_LIBS}
  LibMiso
  -lSDL2_gfx
)

target_include_directories(timers PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include/isolinear
)

add_executable(testregion src/testregion.cpp)
target_include_directories(testregion PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/isolinear)
target_link_libraries(testregion ${CONAN_LIBS} LibMiso -lSDL2_gfx)

add_executable(testgrid src/testgrid.cpp)
target_include_directories(testgrid PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/isolinear)
target_link_libraries(testgrid ${CONAN_LIBS} LibMiso -lSDL2_gfx)
