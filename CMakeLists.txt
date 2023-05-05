cmake_minimum_required(VERSION 3.22)
project(Isolinear VERSION 1.0 LANGUAGES CXX)


set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


add_library(LibIsolinear INTERFACE)
target_include_directories(LibIsolinear INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/isolinear)

add_library(LibMiso INTERFACE)
target_include_directories(LibMiso INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/miso)

add_library(LibSDL2 INTERFACE)
target_link_libraries(LibSDL2 INTERFACE -lSDL2 -lSDL2_gfx -lSDL2_ttf)

add_library(LibFmt INTERFACE)
target_link_libraries(LibFmt INTERFACE -lfmt)



add_executable(testregion src/testregion.cpp)
target_link_libraries(testregion LibSDL2 LibMiso LibFmt LibIsolinear)

add_executable(testgrid src/testgrid.cpp)
target_link_libraries(testgrid LibSDL2 LibMiso LibFmt LibIsolinear)

add_executable(testdialog src/testdialog.cpp)
target_link_libraries(testdialog LibSDL2 LibMiso LibFmt LibIsolinear)

add_executable(kitchensink src/kitchensink.cpp)
target_link_libraries(kitchensink LibSDL2 LibMiso LibFmt LibIsolinear)

add_executable(boids src/boids.cpp)
target_link_libraries(boids LibSDL2 LibMiso LibFmt LibIsolinear)
