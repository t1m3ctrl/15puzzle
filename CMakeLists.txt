cmake_minimum_required(VERSION 3.21.0)

project(15-puzzle)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)

set(SFML_DIR ${PROJECT_SOURCE_DIR}/3rdparty/SFML/lib/cmake/SFML)
set(SFML_SHARED_LIBRARIES_DIR ${PROJECT_SOURCE_DIR}/3rdparty/SFML/bin)

set(SFML_BUILD_NETWORK OFF)
set(SFML_BUILD_EXAMPLES OFF)
set(SFML_BUILD_DOC OFF)

find_package(SFML 2.5 COMPONENTS system graphics audio REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(
    15-puzzle
    ${PROJECT_SOURCE_DIR}/src/main.cpp
)

target_link_libraries(
    15-puzzle
    sfml-main
    sfml-graphics
    sfml-audio
    sfml-window
    sfml-system
)

add_custom_command(
    TARGET ${CMAKE_PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${SFML_SHARED_LIBRARIES_DIR}"
        "$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>"    
)