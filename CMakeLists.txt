cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Isolinear VERSION 1.0)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(Isolinear src/main.cpp)
target_link_libraries(Isolinear ${CONAN_LIBS})

target_include_directories(Isolinear PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include/miso
  ${CMAKE_CURRENT_SOURCE_DIR}/include/isolinear
)
target_link_libraries(Isolinear -lSDL2_gfx)
target_link_libraries(Isolinear -lmpdclient)
