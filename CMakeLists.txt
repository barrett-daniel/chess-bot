cmake_minimum_required(VERSION 3.21)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

project(ChessBot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Main Application -----

file(GLOB_RECURSE ${PROJECT_NAME}_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c++"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h++"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hxx"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)
add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

find_package(SDL3 REQUIRED CONFIG)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)

find_package(SDL3_image CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE $<IF:$<TARGET_EXISTS:SDL3_image::SDL3_image-shared>,SDL3_image::SDL3_image-shared,SDL3_image::SDL3_image-static>)

find_package(boost_container CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::container)

# copy assets to build
add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)

# TESTS -----

file(GLOB_RECURSE ${PROJECT_NAME}_TEST_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp"
)

set(${PROJECT_NAME}_SOURCES_NO_MAIN ${${PROJECT_NAME}_SOURCES})
list(FILTER ${PROJECT_NAME}_SOURCES_NO_MAIN EXCLUDE REGEX ".*/main\\.cpp$")

add_executable(${PROJECT_NAME}_TEST
        ${${PROJECT_NAME}_TEST_SOURCES}
        ${${PROJECT_NAME}_SOURCES_NO_MAIN}
)
target_compile_definitions(${PROJECT_NAME}_TEST PRIVATE SDL_MAIN_HANDLED)
target_include_directories(${PROJECT_NAME}_TEST PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/)

find_package(SDL3 REQUIRED CONFIG)
target_link_libraries(${PROJECT_NAME}_TEST PRIVATE SDL3::SDL3)

find_package(SDL3_image CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME}_TEST PRIVATE $<IF:$<TARGET_EXISTS:SDL3_image::SDL3_image-shared>,SDL3_image::SDL3_image-shared,SDL3_image::SDL3_image-static>)

find_package(doctest CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME}_TEST PRIVATE doctest::doctest)

find_package(boost_container CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::container)

add_custom_command(
        TARGET ${PROJECT_NAME}_TEST POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:${PROJECT_NAME}_TEST>/assets
)

include(doctest)
doctest_discover_tests(${PROJECT_NAME}_TEST)