cmake_minimum_required(VERSION 2.8)

project(rts)

# Specific compiler options
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
endif()

set(EXECUTABLE_NAME "rts")
add_executable(${EXECUTABLE_NAME} 
    src/main.cpp
    src/rts/application.cpp
)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
find_package(SFML 2 COMPONENTS window graphics network audio system REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES})
include_directories(${SFML_INCULDE_DIR})
