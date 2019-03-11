cmake_minimum_required(VERSION 3.10)

project(ProjetM2)
set(exe_name "viewer")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# CMAKE QT OPTIONS
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# CMAKE CPP FLAGS
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic-errors")

set(OpenGL_GL_PREFERENCE "LEGACY")

# PACKAGES REQUIRED
find_package(
    Qt5 REQUIRED COMPONENTS
    Widgets
    Core
    Gui
    OpenGL
)

find_package(OpenGL REQUIRED)

# FIND OPENMESH LIBRARIES
find_library(
    LIB_OPENMESH_CORE
    NAMES libOpenMeshCore.so
    HINTS "libs/OpenMesh-7.1/lib"
)

find_library(
    LIB_OPENMESH_TOOLS
    NAMES libOpenMeshTools.so
    HINTS "libs/OpenMesh-7.1/lib"
)

# CPP SOURCES FILES
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/meshviewerwidget.cpp
    src/drawableobject.cpp
    src/axis.cpp
    src/light.cpp
    src/arcball.cpp
    src/meshobject.cpp
)

# HEADERS FILES
set(HEADERS
    include/mainwindow.h
    include/meshviewerwidget.h
    include/drawableobject.h
    include/axis.h
    include/light.h
    include/arcball.h
    include/meshobject.h
)

set(UI_FORMS
    include/mainwindow.ui
)

# EXE + LINKING
add_executable(
    ${exe_name}
    ${SOURCES}
    ${HEADERS}
    ${UI_FORMS}
)

# -I
target_include_directories(
    ${exe_name} PUBLIC
    # OPENMESH HEADERS
    libs/OpenMesh-7.1/include
)

# -l
target_link_libraries(
    ${exe_name} PUBLIC

    Qt5::Widgets
    Qt5::Core
    Qt5::Gui
    Qt5::OpenGL

    ${OPENGL_LIBRARIES}
    ${LIB_OPENMESH_CORE}
    ${LIB_OPENMESH_TOOLS}
)
