cmake_minimum_required (VERSION 2.6)
project (isogen)

set (ISOGEN_SRCS
    app.c
    voxelgrid.c
    sphere_generator.c
    generator.c
    settings.c
    renderer.c
    utils.c
    cli.c
)

set (isogen_VERSION_MAJOR 0)
set (isogen_VERSION_MINOR 1)

include_directories ("${PROJECT_BINARY_DIR}")

find_package (PNG REQUIRED)
include_directories (${PNG_INCLUDE_DIR})

find_package (GTK2)
if (GTK2_FOUND)
    include_directories (${GTK2_INCLUDE_DIRS})
    set (isogen_USE_GTK 1)

    set (ISOGEN_GTK_SRCS gui.c)
else ()
    set (isogen_USE_GTK 0)
endif ()


configure_file(
    "${PROJECT_SOURCE_DIR}/isogen_config.h.in"
    "${PROJECT_BINARY_DIR}/isogen_config.h"
)

add_executable (isogen ${ISOGEN_SRCS} ${ISOGEN_GTK_SRCS})

add_library(math STATIC IMPORTED)

target_link_libraries (isogen ${PNG_LIBRARY})
target_link_libraries (isogen ${GTK2_LIBRARIES})
target_link_libraries (isogen m)

set(CMAKE_BUILD_TYPE Debug)
