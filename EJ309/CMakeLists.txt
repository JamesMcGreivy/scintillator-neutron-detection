
# Author -- James McGreivy
# Date -- June 26th 2021

# Checks cmake version

cmake_minimum_required(VERSION 3.20)

# Sets the name of the project, and stores it in the variable PROJECT_NAME

project(EJ309)

# Finds Geant4 package, activating all available UI and Vis drivers

set(Geant4_DIR /Users/jamesmcgreivy/Geant4/)
find_package(Geant4 REQUIRED ui_all vis_all)


# Sets up Geant4 include directories and compile definitions
# Geant4_USE_FILE is defined by default

include(${Geant4_USE_FILE})

# Sets up include directory for this project
# PROJECT_SOURCE_DIR should be included in command line

include_directories(${PROJECT_SOURCE_DIR}/include)

# Locates the sources and headers for this project

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)


# Add the executable, along with project sources and headers
# Also, link the project to the Geant4 libraries

add_executable(EJ309 EJ309.cc ${sources} ${headers})
target_link_libraries(EJ309 ${Geant4_LIBRARIES})


# Copy all scripts to the build directory. 
# This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.

# A list of all of the scripts which should go to the build directory.
set(EJ309_SCRIPTS
  init_vis.mac
  vis.mac
  )

# Loop through all of these scripts and copy them to the new location
foreach(_script ${EJ309_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()


# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
install(TARGETS EJ309 DESTINATION bin)
