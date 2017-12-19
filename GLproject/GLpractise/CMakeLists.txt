cmake_minimum_required(VERSION 3.9.4)
project(pakki-engine)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wno-c++11-narrowing -std=c++11")

set(GLAD_LIBRARIES dl)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
include_directories(${GLFW_INCLUDE_DIRS})

find_package (Boost 1.65.1 COMPONENTS REQUIRED chrono filesystem)
include_directories(${Boost_INCLUDE_DIRS})

find_package(Freetype REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})


include_directories(includes)
include_directories(includes/freetype)
include_directories(includes/freetype/config)
include_directories(includes/freetype/internal)
include_directories(includes/glad)
include_directories(includes/KHR)


include_directories(../../include)
include_directories(../../include/glm)

include_directories(../../include/fmod/lowlevel/inc)

if (MSVC)
    include_directories(../../include/mingw)
    file(GLOB PROJECT_HEADERS ../../include/win_only/*.h)
    file(GLOB PROJECT_SOURCES ../../src/win_only/*.cpp)
endif (MSVC)

file(GLOB PROJECT_HEADERS ../../include/*.h)

file(GLOB PROJECT_SOURCES ../../src/*.cpp
                            main.cpp
                            glad.c
                            freetype.cpp)

file(GLOB FMOD_LIBRARIES ../../include/fmod/lowlevel/lib/x86_64/)

source_group("Headers" FILES ${PROJECT_HEADERS})
source_group("Sources" FILES ${PROJECT_SOURCES})
source_group("FMOD Libraries" FILES ${FMOD_LIBRARIES})

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS} ../../src/soundsystem.cpp ../../include/soundsystem.h)

target_link_libraries(${PROJECT_NAME}
        ${GLFW_LIBRARIES}
        ${GLAD_LIBRARIES}
        ${FREETYPE_LIBRARIES}
        ${Boost_LIBRARIES}
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_CHRONO_LIBRARY}
        ${FMOD_LIBRARIES})
