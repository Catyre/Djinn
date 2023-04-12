macro(print_all_variables)
    message(STATUS "print_all_variables------------------------------------------{")
    get_cmake_property(_variableNames VARIABLES)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
    message(STATUS "print_all_variables------------------------------------------}")
endmacro()

cmake_minimum_required(VERSION 3.25.1)
project(
  Djinn
  VERSION 1.0
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
set(BUILD_EXAMPLES
    OFF
    CACHE BOOL "" FORCE) # don't build the supplied examples
add_subdirectory(${raylib}/raylib)

# Find spdlog
set(spdlog ${DJINN_DIR}/lib/spdlog)
include_directories(${spdlog}/include)
add_subdirectory(${spdlog})

set(djinn ${DJINN_DIR})
include_directories(${DJINN_DIR}/include)
add_subdirectory(djinn)
link_directories(${DJINN_DIR}/lib/djinn)

find_library(DJINN_L NAMES djinn HINTS ${DJINN_DIR}/lib/djinn)

# Install library headers
string(APPEND HEADERS "${DJINN_DIR}/include/rlFPCamera.h;"
                      "${DJINN_DIR}/include/rlHelper.h;"
                      "${DJINN_DIR}/include/djinn/core.h;"
                      "${DJINN_DIR}/include/djinn/numerical.h;"
                      "${DJINN_DIR}/include/djinn/particle.h;"
                      "${DJINN_DIR}/include/djinn/pcontacts.h;"
                      "${DJINN_DIR}/include/djinn/pfgen.h;"
                      "${DJINN_DIR}/include/djinn/plinks.h;"
                      "${DJINN_DIR}/include/djinn/potgen.h;"
                      "${DJINN_DIR}/include/djinn/precision.h;"
                      "${DJINN_DIR}/include/djinn/pworld.h;")

# Adding our source files
string(APPEND PROJECT_SOURCES "${DJINN_DIR}/src/numerical.cpp;"
                              "${DJINN_DIR}/src/particle.cpp;"
                              "${DJINN_DIR}/src/pcontacts.cpp;"
                              "${DJINN_DIR}/src/pfgen.cpp;"
                              "${DJINN_DIR}/src/plinks.cpp;"
                              "${DJINN_DIR}/src/potgen.cpp;"
                              "${DJINN_DIR}/src/pworld.cpp;"
                              "${DJINN_DIR}/src/rlFPCamera.cpp;"
                              "${DJINN_DIR}/src/rlHelper.cpp;") # Define PROJECT_SOURCES as a list of all source files

string(APPEND PROJECT_DEMOS "${DJINN_DIR}/src/demos/bouncyball.cpp;"
                            "${DJINN_DIR}/src/demos/lunarorbit.cpp;"
                            "${DJINN_DIR}/src/demos/solarsystem.cpp;"
                            "${DJINN_DIR}/src/demos/springmass.cpp;"
                            "${DJINN_DIR}/src/demos/thermo.cpp")

# Iterate through all demo files and create a make command and executable for
# each
foreach(testsourcefile ${PROJECT_DEMOS})
  # Strip the .cpp from the filename for the executable name
  get_filename_component(DEMO ${testsourcefile} NAME_WE)
  message(STATUS "Creating executable for ${DEMO}")
  # Create the executable demo
  add_executable(${DEMO} ${testsourcefile} ${PROJECT_SOURCES} ${HEADERS})

  # Make sure Djinn + dependencies is linked to each app
  set_target_properties(${DEMO} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
  target_link_libraries(${DEMO} PUBLIC raylib spdlog)
  #target_include_directories(${DEMO} PUBLIC "${DJINN_DIR}/include/djinn" "${DJINN_DIR}/include")

  # Checks if OSX and links appropriate frameworks (Only required on MacOS)
  if(APPLE)
    target_link_libraries(${DEMO} PUBLIC "-framework IOKit")
    target_link_libraries(${DEMO} PUBLIC "-framework Cocoa")
    target_link_libraries(${DEMO} PUBLIC "-framework OpenGL")
  endif()

endforeach(testsourcefile ${PROJECT_DEMOS})

print_all_variables()
