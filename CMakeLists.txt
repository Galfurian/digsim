# -----------------------------------------------------------------------------
# @brief  : Root cmake file.
# @author : Enrico Fraccaroli
# -----------------------------------------------------------------------------
# Set the minimum CMake version, the project name and default build type.
cmake_minimum_required(VERSION 3.1...3.18)

# Set the project name.
project(digsim CXX)

# Set the default build type to Debug.
if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting build type to 'Debug' as none was specified.")
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
endif()

# -----------------------------------------------------------------------------
# ENABLE FETCH CONTENT
# -----------------------------------------------------------------------------

# We need this in order to import external projects.
include(FetchContent)
# Hide fetchcontent variables.
mark_as_advanced(FORCE
    FETCHCONTENT_QUIET
    FETCHCONTENT_BASE_DIR
    FETCHCONTENT_FULLY_DISCONNECTED
    FETCHCONTENT_UPDATES_DISCONNECTED
)

# -----------------------------------------------------------------------------
# OPTIONS
# -----------------------------------------------------------------------------

option(BUILD_TESTS "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" ON)
option(STRICT_WARNINGS "Enable strict compiler warnings" ON)
option(WARNINGS_AS_ERRORS "Treat all warnings as errors" OFF)

# -----------------------------------------------------------------------------
# DEPENDENCIES
# -----------------------------------------------------------------------------

# We want doxygen for the documentation.
find_package(Doxygen)

find_program(CLANG_TIDY_EXE NAMES clang-tidy)

# -----------------------------------------------------------------------------
# LIBRARY
# -----------------------------------------------------------------------------

# Add the C++ library.
add_library(${PROJECT_NAME}
    ${PROJECT_SOURCE_DIR}/src/clock.cpp
    ${PROJECT_SOURCE_DIR}/src/common.cpp
    ${PROJECT_SOURCE_DIR}/src/dependency_graph.cpp
    ${PROJECT_SOURCE_DIR}/src/isignal.cpp
    ${PROJECT_SOURCE_DIR}/src/logger.cpp
    ${PROJECT_SOURCE_DIR}/src/module.cpp
    ${PROJECT_SOURCE_DIR}/src/scheduler.cpp
)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
# Inlcude header directories.
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
# Set the library to use c++-17
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

# -----------------------------------------------------------------------------
# Set the compilation flags.
# -----------------------------------------------------------------------------

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # Disable warnings for MSVC-specific "safe" functions like strcpy_s, etc.,
    # which are not portable and may clutter warning logs.
    target_compile_definitions(${PROJECT_NAME} PUBLIC _CRT_SECURE_NO_WARNINGS)

    # Disable warning C4702: unreachable code.
    add_compile_options(/wd4702)

    if(WARNINGS_AS_ERRORS)
        # Treat all warnings as errors to enforce stricter code quality.
        target_compile_options(${PROJECT_NAME} PUBLIC /WX)
    endif()

    if(STRICT_WARNINGS)
        # Enable external header management to suppress warnings in system and
        # external headers, making it easier to focus on project-specific issues.
        target_compile_options(${PROJECT_NAME} PUBLIC /experimental:external)
        target_compile_options(${PROJECT_NAME} PUBLIC /external:I ${CMAKE_BINARY_DIR})
        target_compile_options(${PROJECT_NAME} PUBLIC /external:anglebrackets)
        target_compile_options(${PROJECT_NAME} PUBLIC /external:W0)

        # Use a high warning level to catch as many potential issues as possible.
        target_compile_options(${PROJECT_NAME} PUBLIC /W4)

        # Enforce standards-compliant behavior to avoid relying on MSVC-specific extensions.
        target_compile_options(${PROJECT_NAME} PUBLIC /permissive-)
    endif()

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if(WARNINGS_AS_ERRORS)
        # Treat all warnings as errors to enforce stricter code quality.
        target_compile_options(${PROJECT_NAME} PUBLIC -Werror)
    endif()

    if(STRICT_WARNINGS)
        # Enable a broad set of warnings to catch common and subtle issues:
        target_compile_options(${PROJECT_NAME} PUBLIC
            -Wall                # Enable most general-purpose warnings.
            -Wextra              # Enable extra warnings not included in -Wall.
            -Wconversion         # Warn about implicit type conversions that may lose data.
            -pedantic            # Enforce strict compliance with the C++ standard.
            -Wshadow             # Warn about variable shadowing, which can cause subtle bugs.
            -Wnon-virtual-dtor   # Warn when a class with virtual functions lacks a virtual destructor.
            -Wnull-dereference   # Warn about potential null pointer dereferences.
            -Wformat=2           # Enable strict checks for printf/scanf format strings.
            -Woverloaded-virtual # Warn when a derived class function hides a base class virtual function.
            -Wfloat-equal        # Warn about direct comparisons of floating-point values, which can be imprecise.
        )

    endif()
endif()

# -----------------------------------------------------------------------------
# EXAMPLES
# -----------------------------------------------------------------------------

if(BUILD_EXAMPLES)
  
    # Add the example.
    add_executable(${PROJECT_NAME}_example1 ${PROJECT_SOURCE_DIR}/examples/example1.cpp)
    target_include_directories(${PROJECT_NAME}_example1 PRIVATE ${PROJECT_SOURCE_DIR}/models)
    target_link_libraries(${PROJECT_NAME}_example1 PRIVATE ${PROJECT_NAME})
    
    add_executable(${PROJECT_NAME}_example2 ${PROJECT_SOURCE_DIR}/examples/example2.cpp)
    target_include_directories(${PROJECT_NAME}_example2 PRIVATE ${PROJECT_SOURCE_DIR}/models)
    target_link_libraries(${PROJECT_NAME}_example2 PRIVATE ${PROJECT_NAME})
    
    add_executable(${PROJECT_NAME}_example3 ${PROJECT_SOURCE_DIR}/examples/example3.cpp)
    target_include_directories(${PROJECT_NAME}_example3 PRIVATE ${PROJECT_SOURCE_DIR}/models)
    target_link_libraries(${PROJECT_NAME}_example3 PRIVATE ${PROJECT_NAME})
    
    add_executable(${PROJECT_NAME}_example4 ${PROJECT_SOURCE_DIR}/examples/example4.cpp)
    target_include_directories(${PROJECT_NAME}_example4 PRIVATE ${PROJECT_SOURCE_DIR}/models)
    target_link_libraries(${PROJECT_NAME}_example4 PRIVATE ${PROJECT_NAME})
    
    add_executable(${PROJECT_NAME}_example5 ${PROJECT_SOURCE_DIR}/examples/example5.cpp)
    target_include_directories(${PROJECT_NAME}_example5 PRIVATE ${PROJECT_SOURCE_DIR}/models)
    target_link_libraries(${PROJECT_NAME}_example5 PRIVATE ${PROJECT_NAME})
    
    add_executable(${PROJECT_NAME}_example6 ${PROJECT_SOURCE_DIR}/examples/example6.cpp)
    target_include_directories(${PROJECT_NAME}_example6 PRIVATE ${PROJECT_SOURCE_DIR}/models)
    target_link_libraries(${PROJECT_NAME}_example6 PRIVATE ${PROJECT_NAME})
    
    add_executable(${PROJECT_NAME}_example7 ${PROJECT_SOURCE_DIR}/examples/example7.cpp)
    target_include_directories(${PROJECT_NAME}_example7 PRIVATE ${PROJECT_SOURCE_DIR}/models)
    target_link_libraries(${PROJECT_NAME}_example7 PRIVATE ${PROJECT_NAME})
    
    add_executable(${PROJECT_NAME}_example8 ${PROJECT_SOURCE_DIR}/examples/example8.cpp)
    target_include_directories(${PROJECT_NAME}_example8 PRIVATE ${PROJECT_SOURCE_DIR}/models)
    target_link_libraries(${PROJECT_NAME}_example8 PRIVATE ${PROJECT_NAME})
    
    add_executable(${PROJECT_NAME}_example9 ${PROJECT_SOURCE_DIR}/examples/example9.cpp)
    target_include_directories(${PROJECT_NAME}_example9 PRIVATE ${PROJECT_SOURCE_DIR}/models)
    target_link_libraries(${PROJECT_NAME}_example9 PRIVATE ${PROJECT_NAME})
    
    add_executable(${PROJECT_NAME}_example10 ${PROJECT_SOURCE_DIR}/examples/example10.cpp)
    target_include_directories(${PROJECT_NAME}_example10 PRIVATE ${PROJECT_SOURCE_DIR}/models)
    target_link_libraries(${PROJECT_NAME}_example10 PRIVATE ${PROJECT_NAME})
    
    # add_executable(${PROJECT_NAME}_example11 ${PROJECT_SOURCE_DIR}/examples/example11.cpp)
    # target_include_directories(${PROJECT_NAME}_example1 PRIVATE ${PROJECT_SOURCE_DIR}/models)
    # target_link_libraries(${PROJECT_NAME}_example11 PRIVATE ${PROJECT_NAME})
    
    # add_executable(${PROJECT_NAME}_example12 ${PROJECT_SOURCE_DIR}/examples/example12.cpp)
    # target_include_directories(${PROJECT_NAME}_example1 PRIVATE ${PROJECT_SOURCE_DIR}/models)
    # target_link_libraries(${PROJECT_NAME}_example12 PRIVATE ${PROJECT_NAME})
    
    # add_executable(${PROJECT_NAME}_example14 ${PROJECT_SOURCE_DIR}/examples/example14.cpp)
    # target_link_libraries(${PROJECT_NAME}_example14 PRIVATE ${PROJECT_NAME})
    
    # add_executable(${PROJECT_NAME}_example15 ${PROJECT_SOURCE_DIR}/examples/example15.cpp)
    # target_link_libraries(${PROJECT_NAME}_example15 PRIVATE ${PROJECT_NAME})
    
    # add_executable(${PROJECT_NAME}_example16 ${PROJECT_SOURCE_DIR}/examples/example16.cpp)
    # target_link_libraries(${PROJECT_NAME}_example16 PRIVATE ${PROJECT_NAME})
    
    # add_executable(${PROJECT_NAME}_example17 ${PROJECT_SOURCE_DIR}/examples/example17.cpp)
    # target_link_libraries(${PROJECT_NAME}_example17 PRIVATE ${PROJECT_NAME})
    
    # add_executable(${PROJECT_NAME}_example18 ${PROJECT_SOURCE_DIR}/examples/example18.cpp)
    # target_link_libraries(${PROJECT_NAME}_example18 PRIVATE ${PROJECT_NAME})

endif()

# -----------------------------------------------------------------------------
# TESTS
# -----------------------------------------------------------------------------

if(BUILD_TESTS)

    # CMake has support for adding tests to a project.
    enable_testing()

    # Add the test.
    add_executable(test_ram ${PROJECT_SOURCE_DIR}/tests/test_ram.cpp)
    target_include_directories(test_ram PRIVATE ${PROJECT_SOURCE_DIR}/models)
    target_link_libraries(test_ram ${PROJECT_NAME})
    add_test(test_ram_run test_ram)
    
    add_executable(test_alu ${PROJECT_SOURCE_DIR}/tests/test_alu.cpp)
    target_include_directories(test_alu PRIVATE ${PROJECT_SOURCE_DIR}/models)
    target_link_libraries(test_alu ${PROJECT_NAME})
    add_test(test_alu_run test_alu)
    
    add_executable(test_pc ${PROJECT_SOURCE_DIR}/tests/test_pc.cpp)
    target_include_directories(test_pc PRIVATE ${PROJECT_SOURCE_DIR}/models)
    target_link_libraries(test_pc ${PROJECT_NAME})
    add_test(test_pc_run test_pc)
    
    add_executable(test_rom ${PROJECT_SOURCE_DIR}/tests/test_rom.cpp)
    target_include_directories(test_rom PRIVATE ${PROJECT_SOURCE_DIR}/models)
    target_link_libraries(test_rom ${PROJECT_NAME})
    add_test(test_rom_run test_rom)
    
    add_executable(test_reg ${PROJECT_SOURCE_DIR}/tests/test_reg.cpp)
    target_include_directories(test_reg PRIVATE ${PROJECT_SOURCE_DIR}/models)
    target_link_libraries(test_reg ${PROJECT_NAME})
    add_test(test_reg_run test_reg)
    
    add_executable(test_cu ${PROJECT_SOURCE_DIR}/tests/test_cu.cpp)
    target_include_directories(test_cu PRIVATE ${PROJECT_SOURCE_DIR}/models)
    target_link_libraries(test_cu ${PROJECT_NAME})
    add_test(test_cu_run test_cu)
    
    add_executable(test_decoder ${PROJECT_SOURCE_DIR}/tests/test_decoder.cpp)
    target_include_directories(test_decoder PRIVATE ${PROJECT_SOURCE_DIR}/models)
    target_link_libraries(test_decoder ${PROJECT_NAME})
    add_test(test_decoder_run test_decoder)
    
    add_executable(test_multiplexer ${PROJECT_SOURCE_DIR}/tests/test_multiplexer.cpp)
    target_include_directories(test_multiplexer PRIVATE ${PROJECT_SOURCE_DIR}/models)
    target_link_libraries(test_multiplexer ${PROJECT_NAME})
    add_test(test_multiplexer_run test_multiplexer)
    
    add_executable(test_phase_fsm ${PROJECT_SOURCE_DIR}/tests/test_phase_fsm.cpp)
    target_include_directories(test_phase_fsm PRIVATE ${PROJECT_SOURCE_DIR}/models)
    target_link_libraries(test_phase_fsm ${PROJECT_NAME})
    add_test(test_test_phase_fsm test_phase_fsm)
    
    add_executable(test_cpu ${PROJECT_SOURCE_DIR}/tests/test_cpu.cpp)
    target_include_directories(test_cpu PRIVATE ${PROJECT_SOURCE_DIR}/models)
    target_link_libraries(test_cpu ${PROJECT_NAME})
    add_test(test_cpu_run test_cpu)
    
    add_executable(test_cpu_support ${PROJECT_SOURCE_DIR}/tests/test_cpu_support.cpp)
    target_include_directories(test_cpu_support PRIVATE ${PROJECT_SOURCE_DIR}/models)
    target_link_libraries(test_cpu_support ${PROJECT_NAME})
    add_test(test_cpu_support_run test_cpu_support)

endif()

# -----------------------------------------------------------------------------
# CODE ANALYSIS
# -----------------------------------------------------------------------------

if(CLANG_TIDY_EXE)
    file(GLOB_RECURSE PROJECT_HEADERS_AND_SOURCES
        "${PROJECT_SOURCE_DIR}/include/**/*.hpp"
        "${PROJECT_SOURCE_DIR}/include/**/*.hh"
        "${PROJECT_SOURCE_DIR}/include/**/*.h"
        "${PROJECT_SOURCE_DIR}/include/*.hpp"
        "${PROJECT_SOURCE_DIR}/include/*.hh"
        "${PROJECT_SOURCE_DIR}/include/*.h"
        "${PROJECT_SOURCE_DIR}/src/**/*.cpp"
        "${PROJECT_SOURCE_DIR}/src/**/*.cc"
        "${PROJECT_SOURCE_DIR}/src/**/*.c"
        "${PROJECT_SOURCE_DIR}/src/*.cpp"
        "${PROJECT_SOURCE_DIR}/src/*.cc"
        "${PROJECT_SOURCE_DIR}/src/*.c"
    )
    add_custom_target(
        ${PROJECT_NAME}_clang_tidy
        COMMAND ${CLANG_TIDY_EXE}
        --p=${CMAKE_BINARY_DIR}
        ${PROJECT_HEADERS_AND_SOURCES}
        COMMENT "Running clang-tidy"
        VERBATIM
    )
    add_custom_target(
        ${PROJECT_NAME}_clang_tidy_fix
        COMMAND ${CLANG_TIDY_EXE}
        --fix --fix-errors
        --p=${CMAKE_BINARY_DIR}
        ${PROJECT_HEADERS_AND_SOURCES}
        COMMENT "Running clang-tidy-fix"
        VERBATIM
    )
endif()

# -----------------------------------------------------------------------------
# DOCUMENTATION
# -----------------------------------------------------------------------------

if(DOXYGEN_FOUND)

    # FetchContent: Doxygen Awesome CSS
    FetchContent_Declare(doxygenawesome
        GIT_REPOSITORY https://github.com/jothepro/doxygen-awesome-css
        GIT_TAG main
    )
    FetchContent_MakeAvailable(doxygenawesome)

    # Hide FetchContent variables to avoid clutter in ccmake.
    mark_as_advanced(FORCE
        FETCHCONTENT_UPDATES_DISCONNECTED_DOXYGENAWESOME
        FETCHCONTENT_SOURCE_DIR_DOXYGENAWESOME
    )

    # Read the file with the version.
    file(READ ${PROJECT_SOURCE_DIR}/include/digsim/digsim.hpp version_file)
    # Extract the version.
    string(REGEX MATCH "DIGSIM_MAJOR_VERSION ([0-9]*)" _ ${version_file})
    set(PROJECT_MAJOR_VERSION ${CMAKE_MATCH_1})
    string(REGEX MATCH "DIGSIM_MINOR_VERSION ([0-9]*)" _ ${version_file})
    set(PROJECT_MINOR_VERSION ${CMAKE_MATCH_1})
    string(REGEX MATCH "DIGSIM_MICRO_VERSION ([0-9]*)" _ ${version_file})
    set(PROJECT_MICRO_VERSION ${CMAKE_MATCH_1})

    # Customization: Doxygen Configuration
    set(DOXYGEN_WARN_FORMAT "$file:$line:1: $text")
    set(DOXYGEN_PROJECT_NAME DigSim)
    set(DOXYGEN_PROJECT_BRIEF "Event-driven logic simulator for digital systems.")
    set(DOXYGEN_PROJECT_NUMBER "${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION}.${PROJECT_MICRO_VERSION}")
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE ${PROJECT_SOURCE_DIR}/README.md)
    set(DOXYGEN_MARKDOWN_SUPPORT YES)
    set(DOXYGEN_AUTOLINK_REFERENCES YES)
    set(DOXYGEN_SHOW_INCLUDE_FILES NO)
    set(DOXYGEN_GENERATE_TREEVIEW YES)
    set(DOXYGEN_GENERATE_LATEX NO)
    set(DOXYGEN_GENERATE_MAN NO)

    # Styling and UX enhancements using Doxygen Awesome
    set(DOXYGEN_HTML_HEADER ${doxygenawesome_SOURCE_DIR}/doxygen-custom/header.html)
    set(DOXYGEN_HTML_EXTRA_STYLESHEET ${doxygenawesome_SOURCE_DIR}/doxygen-awesome.css)
    set(DOXYGEN_HTML_EXTRA_FILES
        ${doxygenawesome_SOURCE_DIR}/doxygen-awesome-fragment-copy-button.js
        ${doxygenawesome_SOURCE_DIR}/doxygen-awesome-paragraph-link.js
        ${doxygenawesome_SOURCE_DIR}/doxygen-awesome-darkmode-toggle.js
    )

    # Set stricter warnings for better documentation quality
    set(DOXYGEN_WARN_IF_UNDOCUMENTED YES)
    set(DOXYGEN_WARN_IF_DOC_ERROR YES)
    set(DOXYGEN_WARN_NO_PARAMDOC YES)
    set(DOXYGEN_WARN_AS_ERROR NO)
   
    # Exclude certain files or directories from documentation (if needed)
    set(DOXYGEN_EXCLUDE_PATTERNS "${PROJECT_SOURCE_DIR}/tests/*" "${PROJECT_SOURCE_DIR}/examples/*")

    # Add Doxygen documentation target.
    file(GLOB_RECURSE PROJECT_HEADERS_AND_SOURCES
        "${PROJECT_SOURCE_DIR}/include/**/*.hpp"
        "${PROJECT_SOURCE_DIR}/include/**/*.hh"
        "${PROJECT_SOURCE_DIR}/include/**/*.h"
        "${PROJECT_SOURCE_DIR}/include/*.hpp"
        "${PROJECT_SOURCE_DIR}/include/*.hh"
        "${PROJECT_SOURCE_DIR}/include/*.h"
        "${PROJECT_SOURCE_DIR}/src/**/*.cpp"
        "${PROJECT_SOURCE_DIR}/src/**/*.cc"
        "${PROJECT_SOURCE_DIR}/src/**/*.c"
        "${PROJECT_SOURCE_DIR}/src/*.cpp"
        "${PROJECT_SOURCE_DIR}/src/*.cc"
        "${PROJECT_SOURCE_DIR}/src/*.c"
    )
    doxygen_add_docs(
        ${PROJECT_NAME}_documentation
        ${PROJECT_SOURCE_DIR}/README.md
        ${PROJECT_SOURCE_DIR}/LICENSE.md
        ${PROJECT_HEADERS_AND_SOURCES}
        COMMENT "Generating Doxygen documentation"
    )
endif()
