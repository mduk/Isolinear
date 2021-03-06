cmake_minimum_required(VERSION 3.10)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


project(Isolinear VERSION 1.0 LANGUAGES CXX)


add_library(LibMiso INTERFACE)
target_include_directories(LibMiso INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/miso)

add_library(LibMPDXX INTERFACE)
target_include_directories(LibMPDXX INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/mpdxx)

add_library(LibIsoMPD INTERFACE)
target_include_directories(LibIsoMPD INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/isompd)

add_library(LibSDL2 INTERFACE)
target_link_libraries(LibSDL2 INTERFACE -lSDL2 -lSDL2_gfx -lSDL2_ttf)

add_library(LibFmt INTERFACE)
target_link_libraries(LibFmt INTERFACE -lfmt)



add_executable(isompd src/isompd.cpp include/isolinear/event.h)
target_link_libraries(isompd LibSDL2 LibFmt LibMiso LibMPDXX LibIsoMPD)
target_include_directories(isompd PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/isolinear)

add_executable(mpdxx src/mpdxx.cpp)
target_include_directories(mpdxx PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/isolinear)
target_link_libraries(mpdxx LibSDL2 LibFmt LibMiso LibMPDXX)

add_executable(timers src/timers.cpp)
target_link_libraries(timers LibSDL2 LibFmt LibMiso)
target_include_directories(timers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/isolinear)

add_executable(testwindows src/testwindows.cpp)
target_link_libraries(testwindows LibSDL2 LibFmt LibMiso)
target_include_directories(testwindows PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/isolinear)



add_executable(testregion src/testregion.cpp)
target_include_directories(testregion PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/isolinear)
target_link_libraries(testregion LibSDL2 LibMiso LibFmt)

add_executable(testgrid src/testgrid.cpp)
target_include_directories(testgrid PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/isolinear)
target_link_libraries(testgrid LibSDL2 LibMiso LibFmt)
