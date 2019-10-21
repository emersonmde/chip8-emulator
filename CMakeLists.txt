cmake_minimum_required(VERSION 3.15)
project(chip8_emulator C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${chip8_emulator_SOURCE_DIR}/cmake")

find_package(SDL2 REQUIRED)
#find_package(SDL2_image REQUIRED)
#find_package(SDL2_ttf REQUIRED)

add_executable(chip8_emulator main.c src/cpu.c include/cpu.h src/memory.c include/memory.h src/display.c include/display.h src/io.c include/io.h)
include_directories(include)

include_directories(${SDL2_INCLUDE_DIR})
#        ${SDL2_IMAGE_INCLUDE_DIR}
#        ${SDL2_TTF_INCLUDE_DIR})
target_link_libraries(chip8_emulator ${SDL2_LIBRARY})
#        ${SDL2_IMAGE_LIBRARIES}
#        ${SDL2_TTF_LIBRARIES})

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})