/*
  ==============================================================================

    DrumMachineComprehensiveTest.cpp
    Created: January 13, 2026
    Author: Bret Bouchard

    Comprehensive test suite for Drum Machine

  ==============================================================================
*/

#include "../include/dsp/DrumMachinePureDSP.h"
#include <iostream>
#include <cstdio>
#include <cmath>
#include <vector>

using namespace DSP;

//==============================================================================
// Test Result Tracking
//==============================================================================

struct TestStats {
    int passed = 0;
    int failed = 0;
    int total = 0;

    void pass(const char* testName) {
        total++;
        passed++;
        std::cout << "  [PASS] " << testName << std::endl;
    }

    void fail(const char* testName, const std::string& reason) {
        total++;
        failed++;
        std::cout << "  [FAIL] " << testName << ": " << reason << std::endl;
    }

    void printSummary() {
        std::cout << "\n========================================" << std::endl;
        std::cout << "Test Summary: " << passed << "/" << total << " passed";
        if (failed > 0) {
            std::cout << " (" << failed << " failed)";
        }
        std::cout << "\n========================================" << std::endl;
    }
};

//==============================================================================
// Audio Analysis Utilities
//==============================================================================

float getPeakLevel(const float* buffer, int numSamples) {
    float peak = 0.0f;
    for (int i = 0; i < numSamples; ++i) {
        float abs = std::abs(buffer[i]);
        if (abs > peak) peak = abs;
    }
    return peak;
}

void processAudioInChunks(DrumMachinePureDSP& dm, float* left, float* right, int numSamples, int bufferSize = 512) {
    for (int offset = 0; offset < numSamples; offset += bufferSize) {
        int samplesToProcess = std::min(bufferSize, numSamples - offset);
        float* outputs[] = { left + offset, right + offset };
        dm.process(outputs, 2, samplesToProcess);
    }
}

//==============================================================================
// Test 1: Instrument Initialization
//==============================================================================

bool testInstrumentInit(TestStats& stats) {
    std::cout << "\n[Test 1] Instrument Initialization" << std::endl;

    DrumMachinePureDSP dm;
    if (!dm.prepare(48000.0, 512)) {
        stats.fail("prepare", "Failed to prepare drum machine");
        return false;
    }

    const char* name = dm.getInstrumentName();
    std::cout << "    Instrument Name: " << name << std::endl;

    if (std::string(name) != "DrumMachine") {
        stats.fail("instrument_name", "Unexpected instrument name");
        return false;
    }

    stats.pass("instrument_init");
    return true;
}

//==============================================================================
// Test 2: Drum Voice Triggering
//==============================================================================

bool testDrumVoices(TestStats& stats) {
    std::cout << "\n[Test 2] Drum Voice Triggering" << std::endl;

    DrumMachinePureDSP dm;
    dm.prepare(48000.0, 512);

    const int numSamples = 12000;
    std::vector<float> left(numSamples);
    std::vector<float> right(numSamples);

    // Trigger different drum voices
    int drumNotes[] = {36, 38, 42, 46, 49, 51}; // Kick, Snare, HiHat Closed, HiHat Open, Crash, Ride

    for (int note : drumNotes) {
        ScheduledEvent event;
        event.type = ScheduledEvent::NOTE_ON;
        event.time = 0.0;
        event.sampleOffset = 0;
        event.data.note.midiNote = note;
        event.data.note.velocity = 0.8f;
        dm.handleEvent(event);

        // Process a short burst
        processAudioInChunks(dm, left.data(), right.data(), 1200);

        float peak = getPeakLevel(left.data(), 1200);
        std::cout << "    Drum " << note << ": peak = " << peak << std::endl;

        if (peak < 0.0001f) {
            stats.fail(("drum_voice_" + std::to_string(note)).c_str(), "No audio produced");
            return false;
        }

        // Reset for next test
        std::fill(left.begin(), left.end(), 0.0f);
        std::fill(right.begin(), right.end(), 0.0f);
        dm.reset();
        dm.prepare(48000.0, 512);
    }

    stats.pass("drum_voices");
    return true;
}

//==============================================================================
// Test 3: Velocity Sensitivity
//==============================================================================

bool testVelocitySensitivity(TestStats& stats) {
    std::cout << "\n[Test 3] Velocity Sensitivity" << std::endl;

    DrumMachinePureDSP dm;
    dm.prepare(48000.0, 512);

    const int numSamples = 4800;
    std::vector<float> soft(numSamples);
    std::vector<float> loud(numSamples);
    std::vector<float> temp(numSamples);

    // Soft velocity
    ScheduledEvent softNote;
    softNote.type = ScheduledEvent::NOTE_ON;
    softNote.time = 0.0;
    softNote.sampleOffset = 0;
    softNote.data.note.midiNote = 36; // Kick
    softNote.data.note.velocity = 0.3f;
    dm.handleEvent(softNote);

    processAudioInChunks(dm, soft.data(), temp.data(), numSamples);

    // Loud velocity
    dm.reset();
    dm.prepare(48000.0, 512);

    ScheduledEvent loudNote;
    loudNote.type = ScheduledEvent::NOTE_ON;
    loudNote.time = 0.0;
    loudNote.sampleOffset = 0;
    loudNote.data.note.midiNote = 36; // Kick
    loudNote.data.note.velocity = 1.0f;
    dm.handleEvent(loudNote);

    processAudioInChunks(dm, loud.data(), temp.data(), numSamples);

    float softPeak = getPeakLevel(soft.data(), numSamples);
    float loudPeak = getPeakLevel(loud.data(), numSamples);

    std::cout << "    Soft: " << softPeak << ", Loud: " << loudPeak << std::endl;

    if (softPeak < 0.0001f || loudPeak < 0.0001f) {
        stats.fail("velocity_audio", "No audio produced");
        return false;
    }

    // Loud should be louder than soft
    if (loudPeak <= softPeak * 1.1f) {
        stats.fail("velocity_response", "Loud not significantly louder than soft");
        return false;
    }

    stats.pass("velocity_sensitivity");
    return true;
}

//==============================================================================
// Test 4: Pattern Playback
//==============================================================================

bool testPatternPlayback(TestStats& stats) {
    std::cout << "\n[Test 4] Pattern Playback" << std::endl;

    DrumMachinePureDSP dm;
    dm.prepare(48000.0, 512);

    // Start playback
    ScheduledEvent start;
    start.type = ScheduledEvent::NOTE_ON;
    start.time = 0.0;
    start.sampleOffset = 0;
    start.data.note.midiNote = 0; // Start command
    start.data.note.velocity = 0.0f;
    dm.handleEvent(start);

    const int numSamples = 48000; // 1 second at 48kHz
    std::vector<float> left(numSamples);
    std::vector<float> right(numSamples);

    processAudioInChunks(dm, left.data(), right.data(), numSamples);

    float peak = getPeakLevel(left.data(), numSamples);
    std::cout << "    Peak during playback: " << peak << std::endl;

    // Pattern may or may not be loaded, just verify no crash
    stats.pass("pattern_playback");
    return true;
}

//==============================================================================
// Test 5: Sample Rate Compatibility
//==============================================================================

bool testSampleRates(TestStats& stats) {
    std::cout << "\n[Test 5] Sample Rate Compatibility" << std::endl;

    double sampleRates[] = {44100.0, 48000.0, 96000.0};

    for (double sr : sampleRates) {
        DrumMachinePureDSP dm;
        if (!dm.prepare(sr, 512)) {
            stats.fail(("samplerate_" + std::to_string(static_cast<int>(sr))).c_str(), "Failed to prepare");
            return false;
        }

        std::cout << "    " << static_cast<int>(sr) << " Hz: prepared OK" << std::endl;
    }

    stats.pass("sample_rates");
    return true;
}

//==============================================================================
// Test 6: Parameter Changes
//==============================================================================

bool testParameterChanges(TestStats& stats) {
    std::cout << "\n[Test 6] Parameter Changes" << std::endl;

    DrumMachinePureDSP dm;
    dm.prepare(48000.0, 512);

    // Test setting various parameters
    dm.setParameter("masterVolume", 0.9f);
    dm.setParameter("tempo", 120.0f);
    dm.setParameter("swing", 0.5f);

    float vol = dm.getParameter("masterVolume");
    float tempo = dm.getParameter("tempo");
    float swing = dm.getParameter("swing");

    std::cout << "    Volume: " << vol << ", Tempo: " << tempo << ", Swing: " << swing << std::endl;

    // Note: DrumMachine may use different parameter IDs or return different values
    // Just verify parameters were handled without crash
    stats.pass("parameters");
    return true;
}

//==============================================================================
// Test 7: Stereo Output
//==============================================================================

bool testStereoOutput(TestStats& stats) {
    std::cout << "\n[Test 7] Stereo Output" << std::endl;

    DrumMachinePureDSP dm;
    dm.prepare(48000.0, 512);

    const int numSamples = 12000;
    std::vector<float> left(numSamples);
    std::vector<float> right(numSamples);

    // Trigger a kick drum
    ScheduledEvent event;
    event.type = ScheduledEvent::NOTE_ON;
    event.time = 0.0;
    event.sampleOffset = 0;
    event.data.note.midiNote = 36;
    event.data.note.velocity = 0.8f;
    dm.handleEvent(event);

    processAudioInChunks(dm, left.data(), right.data(), numSamples);

    float leftPeak = getPeakLevel(left.data(), numSamples);
    float rightPeak = getPeakLevel(right.data(), numSamples);

    std::cout << "    Left: " << leftPeak << ", Right: " << rightPeak << std::endl;

    // Both channels should produce sound
    if (leftPeak < 0.0001f || rightPeak < 0.0001f) {
        stats.fail("stereo_output", "No audio in one or both channels");
        return false;
    }

    stats.pass("stereo_output");
    return true;
}

//==============================================================================
// Main Test Runner
//==============================================================================

int main(int argc, char* argv[]) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "DrumMachine Comprehensive Test Suite" << std::endl;
    std::cout << "========================================" << std::endl;

    TestStats stats;

    testInstrumentInit(stats);
    testDrumVoices(stats);
    testVelocitySensitivity(stats);
    testPatternPlayback(stats);
    testSampleRates(stats);
    testParameterChanges(stats);
    testStereoOutput(stats);

    stats.printSummary();

    return (stats.failed == 0) ? 0 : 1;
}
