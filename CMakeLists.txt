cmake_minimum_required(VERSION 3.25.1)
project(Djinn VERSION 1.0
              DESCRIPTION "My attempt at a physics engine"
              LANGUAGES CXX)

# Set C++ standard to C++11 (Necessary for header-only spdlog)
set(CMAKE_CXX_STANDARD 11)

# Define output directory for the executables
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)

# Tell CMake to output the compile_commands.json file for NeoVim
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Shorter variable 
set(DJINN_DIR ${CMAKE_CURRENT_LIST_DIR}/djinn)

# Install raylib
set(raylib ${DJINN_DIR}/lib/raylib)
include_directories(${raylib}/include)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
add_subdirectory(${raylib}/raylib)

# Find spdlog
set(spdlog ${DJINN_DIR}/lib/spdlog)
include_directories(${spdlog}/include)
add_subdirectory(${spdlog})
#find_package(spdlog REQUIRED)
#include_directories(${spdlog_DIR})

# Build Djinn library
file(GLOB DJINN_LIB "${DJINN_DIR}/src/*.cpp"
                    "${DJINN_DIR}/include/*.h" 
                    "${DJINN_DIR}/include/djinn/*.h")
include_directories(djinn/include)
include_directories(djinn/include/djinn)
add_library(djinn STATIC ${DJINN_LIB})
target_link_libraries(djinn PUBLIC raylib spdlog)


# Install Djinn
install(TARGETS djinn DESTINATION ${DJINN_DIR}/lib/djinn)
include_directories(${DJINN_DIR}/lib/djinn)
message(STATUS "Installed Djinn to ${DJINN_DIR}/lib/djinn")

# Install library headers
file(GLOB HEADERS ${DJINN_DIR}/include/djinn/*.h)
install(FILES ${HEADERS} DESTINATION ${DJINN_DIR}/include/djinn)

# Adding our source files
file(GLOB PROJECT_SOURCES CONFIGURE_DEPENDS "${DJINN_DIR}/src/*.cpp") # Define PROJECT_SOURCES as a list of all source files
file(GLOB PROJECT_DEMOS CONFIGURE_DEPENDS "${DJINN_DIR}/src/demos/*.cpp")

# Iterate through all demo files and create a make command and executable for each
foreach( testsourcefile ${PROJECT_DEMOS} )
    # Strip the .cpp from the filename for the executable name
    get_filename_component(DEMO ${testsourcefile} NAME_WE)
    message(STATUS "Creating executable for ${DEMO}")
    # Create the executable demo
    add_executable( ${DEMO} ${testsourcefile} ${PROJECT_SOURCES} djinn ${raylib}/lib/libraylib.a)
    
    # Make sure Djinn + dependencies is linked to each app
    set_target_properties( ${DEMO} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} )
    target_link_libraries( ${DEMO} PRIVATE raylib spdlog)

    # Checks if OSX and links appropriate frameworks (Only required on MacOS)
    if (APPLE)
        target_link_libraries(${DEMO} PRIVATE "-framework IOKit")
        target_link_libraries(${DEMO} PRIVATE "-framework Cocoa")
        target_link_libraries(${DEMO} PRIVATE "-framework OpenGL")
    endif()

endforeach( testsourcefile ${PROJECT_DEMOS} )

