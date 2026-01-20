cmake_minimum_required(VERSION 3.15)
project(DrumMachineDSP)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add DSP library
add_library(DrumMachineDSP STATIC
    DrumMachineDSP.cpp
    DrumMachineDSP.h
)

# Include directories
target_include_directories(DrumMachineDSP PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ../../../../include  # For InstrumentDSP.h and DrumMachinePureDSP.h
)

# Link with JUCE (if needed)
# target_link_libraries(DrumMachineDSP PUBLIC juce_dsp)
