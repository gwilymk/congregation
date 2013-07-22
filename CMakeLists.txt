cmake_minimum_required(VERSION 2.8)

project(rts)

# Specific compiler options
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
endif()

set(SFML_STATIC_LIBRARIES FALSE CACHE BOOL "Use static SFML librares")

set(EXECUTABLE_NAME "rts")
add_executable(${EXECUTABLE_NAME} 
    src/main.cpp
    src/rts/application.cpp
    src/rts/common.cpp
    src/rts/gui/button.cpp
    src/rts/gui/container.cpp
    src/rts/gui/element.cpp
    src/rts/gui/label.cpp
    src/rts/lua/state.cpp
    src/rts/states/loading_state.cpp
    src/rts/states/main_menu.cpp
    src/rts/states/state.cpp
    src/rts/state_stack.cpp
    src/rts/states/title_state.cpp
)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
find_package(SFML 2 COMPONENTS audio graphics window network system REQUIRED)
find_package(Lua51 REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES} ${LUA_LIBRARIES})
include_directories(${SFML_INCULDE_DIR} ${LUA_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/src)
