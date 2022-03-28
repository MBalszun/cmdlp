# -----------------------------------------------------------------------------
# @brief  : Root cmake file.
# @author : Enrico Fraccaroli
# @create : 03/01/2022
# @update : 03/01/2022
# -----------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# Set the minimum CMake version, the project name and default build type.
# -----------------------------------------------------------------------------
cmake_minimum_required(VERSION 2.8...3.22)

# Set the project name.
project(cmdlp CXX)

# -----------------------------------------------------------------------------
# Set the compilation flags.
# -----------------------------------------------------------------------------

# Only meddle with non-required compilation flags, if we are the top level
# project to ensure consistency with parent project otherwise
get_directory_property(hasParent PARENT_DIRECTORY)

# There is no parent directory.
if(NOT hasParent)
    # Set the default build type to Debug.
    if(NOT CMAKE_BUILD_TYPE)
        message(STATUS "Setting build type to 'Debug' as none was specified.")
        set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
    endif()

    # Enable standards-conformance mode for MSVC compilers that support this
    # flag (Visual C++ 2017 and later).
    if(MSVC)
        add_compile_options(/permissive-)
    endif()
    
    # Add debugging options.
    add_compile_options($<$<CONFIG:Debug>:-g3>)
    add_compile_options($<$<CONFIG:Debug>:-ggdb>)
    add_compile_options($<$<CONFIG:Debug>:-O0>)

    # Set Warning flags
    if(MSVC)
        add_compile_options(/W4)
    else()
        add_compile_options(-Wall)
        add_compile_options(-Werror)
        add_compile_options(-pedantic)
        add_compile_options(-pedantic-errors)
    endif()
endif(NOT hasParent)

# -----------------------------------------------------------------------------
# LIBRARY
# -----------------------------------------------------------------------------
# Add the C++ library.
add_library(${PROJECT_NAME} INTERFACE)
# Inlcude header directories.
target_include_directories(${PROJECT_NAME} INTERFACE ${PROJECT_SOURCE_DIR}/include)

# -----------------------------------------------------------------------------
# Testing programs.
# -----------------------------------------------------------------------------
enable_testing()
add_subdirectory(test)