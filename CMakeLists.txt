cmake_minimum_required (VERSION 2.6)
project (isogen)


set (ISOGEN_SRCS
    app.c
    voxelgrid.c
    settings.c
    renderer.c
    utils.c
    cli.c
    gui.c
)

set (isogen_VERSION_MAJOR 0)
set (isogen_VERSION_MINOR 1)

configure_file(
    "${PROJECT_SOURCE_DIR}/isogen_config.h.in"
    "${PROJECT_BINARY_DIR}/isogen_config.h"
)

include_directories ("${PROJECT_BINARY_DIR}")

find_package (PNG REQUIRED)
include_directories (${PNG_INCLUDE_DIR})


add_executable (isogen ${ISOGEN_SRCS})
target_link_libraries (isogen ${PNG_LIBRARY})

set(CMAKE_BUILD_TYPE Debug)
