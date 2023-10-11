cmake_minimum_required(VERSION 3.22)
project(isolinear)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_library(LibSDL2 INTERFACE)
target_link_libraries(LibSDL2 INTERFACE -lSDL2 -lSDL2_gfx -lSDL2_ttf)

add_library(LibFmt INTERFACE)
target_link_libraries(LibFmt INTERFACE -lfmt)

add_library(LibMiso INTERFACE)
target_include_directories(LibMiso INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/miso)

add_library(LibIsolinear INTERFACE)
target_include_directories(LibIsolinear INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/isolinear)

add_executable(testregion src/testregion.cpp)
target_link_libraries(testregion LibSDL2 LibFmt LibMiso LibIsolinear)

add_executable(testdialog src/testdialog.cpp)
target_link_libraries(testdialog LibSDL2 LibFmt LibMiso LibIsolinear)

add_executable(gameoflife src/gameoflife.cpp)
target_link_libraries(gameoflife LibSDL2 LibFmt LibMiso LibIsolinear)