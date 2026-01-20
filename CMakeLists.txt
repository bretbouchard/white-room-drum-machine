cmake_minimum_required(VERSION 3.26)
project(drummachine VERSION 1.0.0 LANGUAGES C CXX)

#==============================================================================
# MANDATORY: Use centralized JUCE detection
#==============================================================================
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake")
include(FindJUCE)

if(NOT JUCE_FOUND)
    message(FATAL_ERROR "JUCE is required but was not found. See juce_backend/cmake/FindJUCE.cmake for installation instructions.")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find JUCE
set(JUCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../external/JUCE")
if(NOT EXISTS "${JUCE_DIR}/CMakeLists.txt")
    message(FATAL_ERROR "JUCE not found at ${JUCE_DIR}")
endif()

add_subdirectory(${JUCE_DIR} "${CMAKE_BINARY_DIR}/juce")

# Drum Machine Plugin - All 7 Required Formats
juce_add_plugin("drummachine"
    COMPANY_NAME "SchillingerEcosystem"
    BUNDLE_ID "com.schillingerEcosystem.drummachine"
    PLUGIN_IS_A_SYNTH 1
    NEEDS_MIDI_INPUT 1
    FORMATS VST3 AU CLAP LV2 Standalone
    LV2URI "http://schillingerEcosystem.com/plugins/drummachine"
)

target_compile_definitions(drummachine PRIVATE
    JUCE_IGNORE_VST3_MISMATCHED_PARAMETER_ID_WARNING=1
)

target_sources(drummachine
    PRIVATE
        src/DrumMachinePlugin.cpp
        src/dsp/DrumMachinePureDSP.cpp
        include/dsp/DrumMachinePureDSP.h
        ../../include/dsp/LookupTables.cpp
)

target_include_directories(drummachine PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include  # For parent InstrumentDSP.h
)

target_link_libraries(drummachine PRIVATE
    juce::juce_audio_plugin_client
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_data_structures
    juce::juce_dsp
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
)
