cmake_minimum_required(VERSION 3.10)

project(ProjetM2)
set(exe_name "viewer")

# Automatically add the current source
# and build directories to the include path.
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# CMAKE QT OPTIONS
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(OpenGL_GL_PREFERENCE "LEGACY")

# PROJECT SOURCES & HEADERS
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

# Get the available number of processors
include(ProcessorCount)
ProcessorCount(NPROC)
if(NOT NPROC EQUAL 0)
    set(PROC_FLAG "-j${NPROC}")
endif()

####### PACKAGES REQUIRED #######
find_package(
    Qt5 REQUIRED COMPONENTS
    Widgets
    Core
    Gui
    OpenGL
)

find_package(OpenGL REQUIRED)

############# OPENMESH ##############
set(OPENMESH_STR "OpenMesh")
set(OPENMESH_VERSION "8.0")
set(OPENMESH_RELEASE "${OPENMESH_STR}-${OPENMESH_VERSION}")

set(OPENMESH_URL "https://www.openmesh.org/media/Releases")

set(OPENMESH_BIN "${CMAKE_CURRENT_BINARY_DIR}/${OPENMESH_RELEASE}")
set(OPENMESH_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/${OPENMESH_RELEASE}")

set(OPENMESH_LIB_CORE "${OPENMESH_DIR}/lib/libOpenMeshCore.so")
set(OPENMESH_LIB_TOOLS "${OPENMESH_DIR}/lib/libOpenMeshTools.so")

# Download, configure, compile & install OpenMesh library
include(ExternalProject)
ExternalProject_Add(OpenMesh
    URL ${OPENMESH_URL}/${OPENMESH_VERSION}/${OPENMESH_RELEASE}.tar.gz
    BUILD_ALWAYS TRUE
    CMAKE_GENERATOR "Unix Makefiles"
    CMAKE_ARGS
        "-DCMAKE_INSTALL_PREFIX=${OPENMESH_DIR}"
        "-DCMAKE_BUILD_TYPE=Release"
        "-DBUILD_APPS=OFF"
    BINARY_DIR "${OPENMESH_BIN}"
    BUILD_COMMAND make ${PROC_FLAG}
    INSTALL_COMMAND make install
    # Tell CMake Generator that we are going to create
    # library files which will be included into the main target
    BUILD_BYPRODUCTS
        ${OPENMESH_LIB_CORE}
        ${OPENMESH_LIB_TOOLS}
)
################################################################

######### BUILD ##########
# EXE + LINKING
add_executable(
    ${exe_name}
    ${SOURCES}
    ${HEADERS}
    ${UI_FORMS}
)

add_dependencies(
    ${exe_name}
    OpenMesh
)

# g++ compile options
target_compile_options(
    ${exe_name} PUBLIC
    -std=c++11
    -Wall
    -Wextra
    -pedantic-errors
)

# -I
target_include_directories(
    ${exe_name} PUBLIC
    "${OPENMESH_DIR}/include" #OPENMESH HEADERS
)

# -l
target_link_libraries(
    ${exe_name} PUBLIC

    Qt5::Widgets
    Qt5::Core
    Qt5::Gui
    Qt5::OpenGL

    ${OPENGL_LIBRARIES}

    ${OPENMESH_LIB_CORE}
    ${OPENMESH_LIB_TOOLS}
)
