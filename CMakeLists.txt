cmake_minimum_required(VERSION 3.27)
project(bracelet_maker C CXX)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

# Adding Raylib
include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY "https://github.com/raysan5/raylib.git"
    GIT_TAG "master"
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(raylib)

# Fetch Clay
FetchContent_Declare(
    clay
    GIT_REPOSITORY "https://github.com/nicbarker/clay.git"
    GIT_TAG "main"
)

FetchContent_GetProperties(clay)
if(NOT clay_POPULATED)
    FetchContent_Populate(clay)
    # Modify Clay's CMakeLists.txt to remove examples
    file(READ ${clay_SOURCE_DIR}/CMakeLists.txt CLAY_CMAKE_CONTENTS)
    string(REPLACE "add_subdirectory(examples)" "" CLAY_CMAKE_CONTENTS "${CLAY_CMAKE_CONTENTS}")
    file(WRITE ${clay_SOURCE_DIR}/CMakeLists.txt "${CLAY_CMAKE_CONTENTS}")
    add_subdirectory(${clay_SOURCE_DIR} ${clay_BINARY_DIR})
endif()

# Main executable
add_executable(bracelet_maker 
    main.c
    bracelet.c
    bead.c
    clay_renderer_raylib.c
    circle_menu.cpp
    surreal_client.c
)

# Enable C++ in the target
set_target_properties(bracelet_maker PROPERTIES
    LINKER_LANGUAGE CXX
)

# Compiler options
target_compile_options(bracelet_maker PUBLIC)
target_include_directories(bracelet_maker PUBLIC 
    .
    ${clay_SOURCE_DIR}
    ${raylib_SOURCE_DIR}/src
    ${raylib_SOURCE_DIR}/src/extras
)

# Link libraries
target_link_libraries(bracelet_maker PUBLIC raylib)

# Compiler flags
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -DCLAY_DEBUG")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -DCLAY_DEBUG")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif()

# Create resources directory if it doesn't exist
add_custom_command(
    TARGET bracelet_maker PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
    ${CMAKE_CURRENT_BINARY_DIR}/resources
)

# Copy resources if they exist
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/resources)
    add_custom_command(
        TARGET bracelet_maker POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resources
        ${CMAKE_CURRENT_BINARY_DIR}/resources
    )
endif()

# Add to your existing CMakeLists.txt
set_source_files_properties(
    circle_menu.cpp
    PROPERTIES
    COMPILE_FLAGS "-x c++"
)

# Make sure bracelet.c is compiled as C
set_source_files_properties(
    bracelet.c
    PROPERTIES
    COMPILE_FLAGS "-x c"
)

# Add to your CMakeLists.txt
find_package(CURL REQUIRED)
target_link_libraries(bracelet_maker PUBLIC CURL::libcurl)

# Add after raylib setup
# Setup raygui
set(RAYGUI_SUPPORT TRUE)

FetchContent_Declare(
    raygui
    GIT_REPOSITORY https://github.com/raysan5/raygui.git
    GIT_TAG "master"
    GIT_SHALLOW TRUE
)

FetchContent_GetProperties(raygui)
if(NOT raygui_POPULATED)
    FetchContent_Populate(raygui)
endif()

target_include_directories(bracelet_maker PUBLIC 
    ${raygui_SOURCE_DIR}/src
)

# Add tinyfiledialogs
FetchContent_Declare(
    tinyfiledialogs
    GIT_REPOSITORY https://git.code.sf.net/p/tinyfiledialogs/code
    GIT_TAG master
)

FetchContent_GetProperties(tinyfiledialogs)
if(NOT tinyfiledialogs_POPULATED)
    FetchContent_Populate(tinyfiledialogs)
    add_library(tinyfiledialogs STATIC 
        ${tinyfiledialogs_SOURCE_DIR}/tinyfiledialogs.c
    )
    target_include_directories(tinyfiledialogs PUBLIC 
        ${tinyfiledialogs_SOURCE_DIR}
    )
endif()

target_link_libraries(bracelet_maker PUBLIC tinyfiledialogs)
