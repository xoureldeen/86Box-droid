cmake_minimum_required(VERSION 3.4.1)

add_subdirectory(${CMAKE_SOURCE_DIR}/86Box)
add_subdirectory(${CMAKE_SOURCE_DIR}/SDL2)
add_subdirectory(${CMAKE_SOURCE_DIR}/libpng)
add_subdirectory(${CMAKE_SOURCE_DIR}/libsndfile)

include_directories(${CMAKE_SOURCE_DIR}/SDL2/include)
include_directories(${CMAKE_SOURCE_DIR}/libpng/include)
include_directories(${CMAKE_SOURCE_DIR}/libsndfile/include)

find_package(SDL2 REQUIRED)
find_package(PNG REQUIRED)
find_package(SNDFILE REQUIRED)

target_link_libraries(86Box SDL2)
target_link_libraries(86Box png)
target_link_libraries(86Box libsndfile)

option(RELEASE      "Release build"                                              OFF)
option(DYNAREC      "Dynamic recompiler"                                         OFF)
option(OPENAL       "OpenAL"                                                     OFF)
option(RTMIDI       "RtMidi"                                                     OFF)
option(FLUIDSYNTH   "FluidSynth"                                                 OFF)
option(MUNT         "MUNT"                                                       OFF)
option(VNC          "VNC renderer"                                               OFF)
option(NEW_DYNAREC  "Use the PCem v15 (\"new\") dynamic recompiler"              OFF)
option(MINITRACE    "Enable Chrome tracing using the modified minitrace library" OFF)
option(GDBSTUB      "Enable GDB stub server for debugging"                       OFF)
option(DEV_BRANCH   "Development branch"                                         OFF)
option(DISCORD      "Discord Rich Presence support"                              OFF)
option(DEBUGREGS486 "Enable debug register operation on 486+ CPUs"               OFF)

set(QT OFF)
option(CPPTHREADS "C++11 threads" OFF)

set(RELEASE_BUILD   OFF)
set(BETA_BUILD      OFF)
set(ALPHA_BUILD     OFF)
