cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Isolinear VERSION 1.0)
add_executable(Isolinear src/main.cpp)

target_include_directories(Isolinear PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(Isolinear -lSDL2)
target_link_libraries(Isolinear -lSDL2_ttf)
target_link_libraries(Isolinear -lSDL2_gfx)
target_link_libraries(Isolinear -lmpdclient)
