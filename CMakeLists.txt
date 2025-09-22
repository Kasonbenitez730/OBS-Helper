cmake_minimum_required(VERSION 3.16)
project(design-overlay VERSION 1.0.0)

# Find libobs include directory
find_path(LIBOBS_INCLUDE_DIR
    NAMES obs-module.h
    HINTS
        ${CMAKE_PREFIX_PATH}/libobs
        ${CMAKE_PREFIX_PATH}/include
        ${CMAKE_SOURCE_DIR}/../libobs
        ${CMAKE_SOURCE_DIR}/../../libobs
        ENV LIBOBS_INCLUDE_DIR
    PATH_SUFFIXES libobs include
    DOC "Path to libobs include directory"
)

# Find obsconfig.h
find_path(LIBOBS_CONFIG_DIR
    NAMES obsconfig.h
    HINTS
        ${CMAKE_PREFIX_PATH}
        ${CMAKE_PREFIX_PATH}/config
        ${CMAKE_SOURCE_DIR}/../../build64
        ${CMAKE_SOURCE_DIR}/../../build64/config
        ENV LIBOBS_CONFIG_DIR
    PATH_SUFFIXES config .
    DOC "Path to obsconfig.h directory"
)

# Find libobs library
find_library(LIBOBS_LIB
    NAMES obs libobs
    HINTS
        ${CMAKE_PREFIX_PATH}/libobs/Release
        ${CMAKE_PREFIX_PATH}/libobs/Debug
        ${CMAKE_PREFIX_PATH}/bin
        ${CMAKE_PREFIX_PATH}/lib
        ${CMAKE_SOURCE_DIR}/../../build64/libobs/Release
        ${CMAKE_SOURCE_DIR}/../../build64/libobs/Debug
        ${CMAKE_SOURCE_DIR}/../../build64/bin
        ENV LIBOBS_LIB_DIR
    PATH_SUFFIXES lib bin Release Debug
    DOC "Path to libobs library"
)

# Error checking
if(NOT LIBOBS_INCLUDE_DIR)
    message(FATAL_ERROR "libobs include directory not found! Set CMAKE_PREFIX_PATH to OBS build directory")
endif()

if(NOT LIBOBS_LIB)
    message(FATAL_ERROR "libobs library not found! Set CMAKE_PREFIX_PATH to OBS build directory")
endif()

if(NOT LIBOBS_CONFIG_DIR)
    message(WARNING "obsconfig.h not found, trying common locations...")
    if(EXISTS "${CMAKE_PREFIX_PATH}/obsconfig.h")
        set(LIBOBS_CONFIG_DIR "${CMAKE_PREFIX_PATH}")
    elseif(EXISTS "${CMAKE_SOURCE_DIR}/../../build64/obsconfig.h")
        set(LIBOBS_CONFIG_DIR "${CMAKE_SOURCE_DIR}/../../build64")
    else()
        message(FATAL_ERROR "obsconfig.h not found! Check OBS build directory")
    endif()
endif()

# Create plugin target
add_library(${PROJECT_NAME} MODULE design-overlay.c)

# Set properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    PREFIX ""
    FOLDER "plugins"
    C_STANDARD 11
    C_STANDARD_REQUIRED YES
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${LIBOBS_INCLUDE_DIR}
    ${LIBOBS_CONFIG_DIR}
)

# Link libraries
target_link_libraries(${PROJECT_NAME} ${LIBOBS_LIB})

# Platform specific settings
if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE 
        _CRT_SECURE_NO_WARNINGS
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )
    
    # Architecture detection
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(ARCH_NAME "64bit")
    else()
        set(ARCH_NAME "32bit")
    endif()
    
    # Install rules for Windows
    install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION "obs-plugins/${ARCH_NAME}/"
        RUNTIME DESTINATION "obs-plugins/${ARCH_NAME}/"
    )
    
elseif(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        BUNDLE TRUE
        BUNDLE_EXTENSION "so"
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist"
    )
    
    target_compile_options(${PROJECT_NAME} PRIVATE -Wno-error=shorten-64-to-32)
    
    install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION "obs-plugins/"
    )
    
elseif(UNIX)
    target_compile_options(${PROJECT_NAME} PRIVATE -fPIC)
    
    install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION "lib/obs-plugins/"
    )
endif()

# Compiler warnings
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE 
        /W3 
        /wd4996  # Disable deprecated function warnings
    )
else()
    target_compile_options(${PROJECT_NAME} PRIVATE 
        -Wall 
        -Wextra 
        -Wno-unused-parameter
    )
endif()

# Debug information
message(STATUS "=== Design Overlay Plugin Configuration ===")
message(STATUS "libobs include: ${LIBOBS_INCLUDE_DIR}")
message(STATUS "libobs config: ${LIBOBS_CONFIG_DIR}")
message(STATUS "libobs library: ${LIBOBS_LIB}")
if(WIN32)
    message(STATUS "Install destination: obs-plugins/${ARCH_NAME}/")
endif()
message(STATUS "==========================================")

# Add custom target for easy installation
if(WIN32)
    add_custom_target(install-plugin
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target install
        DEPENDS ${PROJECT_NAME}
        COMMENT "Installing plugin to OBS plugins directory"
    )
endif()