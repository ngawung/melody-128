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

message(STATUS "Downloading pugixml library")
include(FetchContent)
FetchContent_Declare(pugixml URL https://github.com/zeux/pugixml/releases/download/v1.12.1/pugixml-1.12.1.zip)
FetchContent_MakeAvailable(pugixml)

######## main configuration ########
project(${PROJECT_NAME} VERSION 1.0)
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${VENDOR_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE "src" ${VENDOR_INCLUDE})
target_link_libraries(${PROJECT_NAME} PUBLIC raylib pugixml)

target_compile_definitions(${PROJECT_NAME} PUBLIC RESOURCE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resource/")
#target_compile_definitions(${PROJECT_NAME} PUBLIC RESOURCE_PATH="./resource/")