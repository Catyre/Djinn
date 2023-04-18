cmake_minimum_required(VERSION 3.25.1)
project(
  Djinn
  VERSION 1.0
  DESCRIPTION "My attempt at a physics engine"
  LANGUAGES CXX)

# Set C++ standard to C++11 (Necessary for header-only spdlog)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE RelWithDebInfo)

# Define output directory for the executables
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)

# Tell CMake to output the compile_commands.json file for NeoVim
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Shorter variable
set(DJINN_DIR ${CMAKE_CURRENT_LIST_DIR}/djinn)
set(DJINN_INC ${DJINN_DIR}/include)
set(DJINN_SRC ${DJINN_DIR}/src)
set(DJINN_DEMOS ${DJINN_SRC}/demos)

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

#set(djinn ${DJINN_DIR})
include_directories(${DJINN_DIR}/include)
#add_subdirectory(djinn)
#link_directories(${DJINN_DIR}/lib/djinn)

#find_library(DJINN_L NAMES djinn HINTS ${DJINN_DIR}/lib/djinn)

# Install library headers
string(APPEND HEADERS "${DJINN_INC}/rlFPCamera.h;" 
                      "${DJINN_INC}/rlHelper.h;"
                      "${DJINN_INC}/djinn/core.h;"
                      "${DJINN_INC}/djinn/numerical.h;"
                      "${DJINN_INC}/djinn/particle.h;"
                      "${DJINN_INC}/djinn/pcontacts.h;"
                      "${DJINN_INC}/djinn/pfgen.h;"
                      "${DJINN_INC}/djinn/plinks.h;"
                      "${DJINN_INC}/djinn/potgen.h;"
                      "${DJINN_INC}/djinn/precision.h;"
                      "${DJINN_INC}/djinn/pworld.h;")

# Adding our source files
string(APPEND PROJECT_SOURCES "${DJINN_SRC}/numerical.cpp;"
                              "${DJINN_SRC}/particle.cpp;"
                              "${DJINN_SRC}/pcontacts.cpp;"
                              "${DJINN_SRC}/pfgen.cpp;"
                              "${DJINN_SRC}/plinks.cpp;"
                              "${DJINN_SRC}/potgen.cpp;"
                              "${DJINN_SRC}/pworld.cpp;"
                              "${DJINN_SRC}/rlFPCamera.cpp;"
                              "${DJINN_SRC}/rlHelper.cpp;") # Define PROJECT_SOURCES as a list of all source files

string(APPEND PROJECT_DEMOS "${DJINN_DEMOS}/bouncyball.cpp;"
                            "${DJINN_DEMOS}/lunarorbit.cpp;"
                            "${DJINN_DEMOS}/solarsystem.cpp;"
                            "${DJINN_DEMOS}/springmass.cpp;"
                            "${DJINN_DEMOS}/thermo.cpp;"
                            "${DJINN_DEMOS}/RK4ND.cpp")

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
