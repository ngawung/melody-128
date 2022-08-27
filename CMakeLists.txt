cmake_minimum_required(VERSION 3.24)

set(PROJECT_NAME melody-128 CACHE BOOL "")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# yes... im using glob... dont judge me....
file(GLOB PROJECT_SOURCES CONFIGURE_DEPENDS 
    "src/*.cpp"
    "src/**/*.cpp")

file(GLOB VENDOR_SOURCES CONFIGURE_DEPENDS 
    "vendor/melody/melody/*.cpp" 
    "vendor/melody/melody/**/*.cpp")

set(VENDOR_INCLUDE 
    "vendor/melody")

###################################
message(STATUS "Downloading raylib library")
include(FetchContent)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # don't build the supplied example games
FetchContent_Declare(raylib URL https://github.com/raysan5/raylib/archive/refs/tags/4.2.0.zip)
FetchContent_MakeAvailable(raylib)

message(STATUS "Downloading raygui library")
include(FetchContent)
FetchContent_Declare(raygui URL https://github.com/raysan5/raygui/archive/refs/tags/3.2.zip)
FetchContent_MakeAvailable(raygui)

message(STATUS "Downloading yaml-cpp library")
include(FetchContent)
FetchContent_Declare(yaml-cpp URL https://github.com/jbeder/yaml-cpp/archive/refs/tags/yaml-cpp-0.7.0.zip)
FetchContent_MakeAvailable(yaml-cpp)

message(STATUS "Downloading imgui library")
include(FetchContent)
FetchContent_Declare(imgui URL https://github.com/ocornut/imgui/archive/refs/tags/v1.88.zip)
FetchContent_MakeAvailable(imgui)
file(GLOB imgui_SOURCES ${imgui_SOURCE_DIR}/*.cpp)

message(STATUS "Downloading rlImGui library")
include(FetchContent)
FetchContent_Declare(rlimgui URL https://github.com/raylib-extras/rlImGui/archive/refs/heads/main.zip)
FetchContent_MakeAvailable(rlimgui)
file(GLOB rlImGui_SOURCES ${rlimgui_SOURCE_DIR}/*.cpp)

######## main configuration ########
project(${PROJECT_NAME} VERSION 1.0)
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${VENDOR_SOURCES} ${imgui_SOURCES} ${rlImGui_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE "src" ${VENDOR_INCLUDE} "${raygui_SOURCE_DIR}/src" ${imgui_SOURCE_DIR} ${rlimgui_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC raylib yaml-cpp)

target_compile_definitions(${PROJECT_NAME} PUBLIC RESOURCE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resource/")
#target_compile_definitions(${PROJECT_NAME} PUBLIC RESOURCE_PATH="./resource/")
