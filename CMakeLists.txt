cmake_minimum_required(VERSION 3.21)

project(ChessBot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_executable(${PROJECT_NAME})

file(GLOB_RECURSE ${PROJECT_NAME}_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c++"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h++"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hxx"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)
target_sources(${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

find_package(SDL3 REQUIRED CONFIG)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)

find_package(SDL3_image CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE $<IF:$<TARGET_EXISTS:SDL3_image::SDL3_image-shared>,SDL3_image::SDL3_image-shared,SDL3_image::SDL3_image-static>)

# copy assets to build
add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)