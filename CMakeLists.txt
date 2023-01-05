cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# note for future me... set project dlu sebelum find_package
set(PROJECT_NAME melody-128 CACHE BOOL "")
set(PROJECT_VERSION 1.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION})

if (EMSCRIPTEN)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/resource@resource/")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

##############################################################

message(STATUS "Downloading raylib library")
include(FetchContent)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
FetchContent_Declare(raylib URL https://github.com/raysan5/raylib/archive/refs/tags/4.2.0.zip)
FetchContent_MakeAvailable(raylib)

message(STATUS "Downloading raygui library")
include(FetchContent)
FetchContent_Declare(raygui URL https://github.com/raysan5/raygui/archive/refs/tags/3.2.zip)
FetchContent_MakeAvailable(raygui)

message(STATUS "Downloading mINI library")
include(FetchContent)
FetchContent_Declare(mini URL https://github.com/pulzed/mINI/archive/refs/tags/0.9.14.zip)
FetchContent_MakeAvailable(mini)

message(STATUS "Downloading fmt library")
include(FetchContent)
FetchContent_Declare(fmt URL https://github.com/fmtlib/fmt/releases/download/9.1.0/fmt-9.1.0.zip)
FetchContent_MakeAvailable(fmt)

message(STATUS "Downloading imgui library")
include(FetchContent)
FetchContent_Declare(imgui URL https://github.com/ocornut/imgui/archive/refs/tags/v1.89.1.zip)
FetchContent_MakeAvailable(imgui)
file(GLOB imgui_SOURCES "${imgui_SOURCE_DIR}/*.cpp")

message(STATUS "Downloading rlImGui library")
include(FetchContent)
FetchContent_Declare(rlimgui URL https://github.com/raylib-extras/rlImGui/archive/refs/heads/main.zip)
FetchContent_MakeAvailable(rlimgui)
file(GLOB rlImGui_SOURCES ${rlimgui_SOURCE_DIR}/*.cpp)

if (NOT DEFINED EMSCRIPTEN)
    message(STATUS "Downloading nfd library")
    include(FetchContent)
    FetchContent_Declare(nfd URL https://github.com/btzy/nativefiledialog-extended/archive/refs/tags/v1.0.0.zip)
    FetchContent_MakeAvailable(nfd)
endif()

# message(STATUS "Downloading tweeny library")
# include(FetchContent)
# FetchContent_Declare(tweeny URL https://github.com/mobius3/tweeny/archive/refs/tags/v3.2.0.zip)
# FetchContent_MakeAvailable(tweeny)

##############################################################

# yes... im using glob... dont judge me....
file(GLOB PROJECT_SOURCES CONFIGURE_DEPENDS
    "src/*.cpp"
    "src/**/*.cpp"
    "vendor/melody/melody/*.cpp"
    "vendor/melody/melody/**/*.cpp")

set(PROJECT_INCLUDE
    "src"
    "vendor/melody"
    "${raygui_SOURCE_DIR}/src"
    "${mini_SOURCE_DIR}/src"
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${rlimgui_SOURCE_DIR})

set(PROJECT_LIBRARY
    fmt
    vendor)

###################################
add_library(vendor STATIC ${imgui_SOURCES} ${rlImGui_SOURCES})
target_include_directories(vendor PRIVATE ${PROJECT_INCLUDE})
target_link_libraries(vendor PUBLIC raylib)
##################################

if (EMSCRIPTEN)
    set(PROJECT_DEFINITION RESOURCE_PATH="resource/")
else()
    set(PROJECT_LIBRARY ${PROJECT_LIBRARY} nfd)
    set(PROJECT_DEFINITION RESOURCE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resource/")
endif ()

##############################################################

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_INCLUDE})
target_link_libraries(${PROJECT_NAME} PUBLIC ${PROJECT_LIBRARY})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_DEFINITION})
