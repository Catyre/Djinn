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

# Find raylib
find_package(raylib REQUIRED)
include_directories(${raylib_INCLUDE_DIR})
link_libraries(${raylib_LIBRARY})

# Find spdlog
find_package(spdlog REQUIRED)
include_directories(${spdlog_DIR})

# Build Djinn library
file(GLOB DJINN_LIB "${CMAKE_CURRENT_LIST_DIR}/djinn/src/*.cpp"
                    "${CMAKE_CURRENT_LIST_DIR}/djinn/include/*.h" 
                    "${CMAKE_CURRENT_LIST_DIR}/djinn/include/djinn/*.h")
include_directories(djinn/include)
include_directories(djinn/include/djinn)
add_library(djinn SHARED ${DJINN_LIB})

# Install Djinn
install(TARGETS djinn DESTINATION ${CMAKE_CURRENT_LIST_DIR}/djinn/lib/djinn)

# Install library headers
file(GLOB HEADERS ${CMAKE_CURRENT_LIST_DIR}/djinn/include/djinn/*.h)
install(FILES ${HEADERS} DESTINATION ${CMAKE_CURRENT_LIST_DIR}/djinn/include/djinn)

# Adding our source files
file(GLOB PROJECT_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/djinn/src/*.cpp") # Define PROJECT_SOURCES as a list of all source files
file(GLOB PROJECT_DEMOS CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/djinn/src/demos/*.cpp")
# file(GLOB_RECURSE PROJECT_INCLUDE CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/djinn/include/*.h")

foreach( testsourcefile ${PROJECT_DEMOS} )
    # Strip the .cpp from the filename for the executable name
    get_filename_component(DEMO ${testsourcefile} NAME_WE)

    # Create the executable demo
    add_executable( ${DEMO} ${testsourcefile} ${PROJECT_SOURCES} ${djinn})
    
    # Make sure Djinn + dependencies is linked to each app
    set_target_properties( ${DEMO} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} )
    target_link_libraries( ${DEMO} PRIVATE ${djinn} ${raylib_LIBRARY} spdlog::spdlog_header_only)
endforeach( testsourcefile ${PROJECT_DEMOS} )

