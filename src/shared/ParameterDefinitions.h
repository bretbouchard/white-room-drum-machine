/*
  ==============================================================================

    ParameterDefinitions.h
    Template for defining instrument parameters

    This file shows the pattern for defining parameters that map between
    the JUCE plugin UI and the DSP engine.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
// Parameter ID Enum
//==============================================================================
namespace @INSTRUMENT_ID@::Parameters
{
    // Define all parameter IDs here
    // Example:
    // enum ID
    // {
    //     OSC_WAVEFORM,
    //     OSC_DETUNE,
    //     OSC_LEVEL,
    //     FILTER_CUTOFF,
    //     FILTER_RESONANCE,
    //     ENV_ATTACK,
    //     ENV_DECAY,
    //     ENV_SUSTAIN,
    //     ENV_RELEASE,
    //     MASTER_VOLUME,
    //     PITCH_BEND_RANGE
    // };

    //==========================================================================
    // Parameter Categories (for organization)
    //==========================================================================
    // Oscillator parameters
    // Filter parameters
    // Envelope parameters
    // Effect parameters
    // Master parameters

    //==========================================================================
    // Create JUCE Parameter Layout
    //==========================================================================
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

        // Example parameters:

        // // Oscillator Waveform (0-4 integer)
        // params.push_back (std::make_unique<juce::AudioParameterChoice>(
        //     juce::ParameterID ("osc_waveform", 1),
        //     "Oscillator Waveform",
        //     juce::StringArray {"Saw", "Square", "Triangle", "Sine", "Noise"},
        //     0
        // ));

        // // Oscillator Detune (-100 to 100 cents)
        // params.push_back (std::make_unique<juce::AudioParameterFloat>(
        //     juce::ParameterID ("osc_detune", 1),
        //     "Oscillator Detune",
        //     juce::NormalisableRange<float> (-100.0f, 100.0f, 1.0f),
        //     0.0f,
        //     juce::AudioParameterFloatAttributes().withLabel ("cents")
        // ));

        // // Oscillator Level (0.0 to 1.0)
        // params.push_back (std::make_unique<juce::AudioParameterFloat>(
        //     juce::ParameterID ("osc_level", 1),
        //     "Oscillator Level",
        //     juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f),
        //     0.8f,
        //     juce::AudioParameterFloatAttributes().withLabel ("%")
        //     .withStringFromValueFunction ([](float value, int) { return juce::String (value * 100.0f) + "%"; })
        // ));

        // // Filter Cutoff (20 to 20000 Hz, logarithmic)
        // params.push_back (std::make_unique<juce::AudioParameterFloat>(
        //     juce::ParameterID ("filter_cutoff", 1),
        //     "Filter Cutoff",
        //     juce::NormalisableRange<float> (20.0f, 20000.0f, 1.0f, 0.3f),
        //     2000.0f,
        //     juce::AudioParameterFloatAttributes().withLabel ("Hz")
        //     .withStringFromValueFunction ([](float value, int) {
        //         if (value < 1000.0f) return juce::String (value, 0) + " Hz";
        //         return juce::String (value / 1000.0f, 1) + " kHz";
        //     })
        // ));

        // // Filter Resonance (0.0 to 20.0)
        // params.push_back (std::make_unique<juce::AudioParameterFloat>(
        //     juce::ParameterID ("filter_resonance", 1),
        //     "Filter Resonance",
        //     juce::NormalisableRange<float> (0.0f, 20.0f, 0.1f),
        //     1.0f,
        //     juce::AudioParameterFloatAttributes().withLabel ("Q")
        // ));

        // // Envelope Attack (0.0 to 10.0 seconds)
        // params.push_back (std::make_unique<juce::AudioParameterFloat>(
        //     juce::ParameterID ("env_attack", 1),
        //     "Envelope Attack",
        //     juce::NormalisableRange<float> (0.0f, 10.0f, 0.01f, 0.3f),
        //     0.01f,
        //     juce::AudioParameterFloatAttributes().withLabel ("s")
        //     .withStringFromValueFunction ([](float value, int) { return juce::String (value, 2) + " s"; })
        // ));

        // // Envelope Decay (0.0 to 10.0 seconds)
        // params.push_back (std::make_unique<juce::AudioParameterFloat>(
        //     juce::ParameterID ("env_decay", 1),
        //     "Envelope Decay",
        //     juce::NormalisableRange<float> (0.0f, 10.0f, 0.01f, 0.3f),
        //     0.1f,
        //     juce::AudioParameterFloatAttributes().withLabel ("s")
        //     .withStringFromValueFunction ([](float value, int) { return juce::String (value, 2) + " s"; })
        // ));

        // // Envelope Sustain (0.0 to 1.0)
        // params.push_back (std::make_unique<juce::AudioParameterFloat>(
        //     juce::ParameterID ("env_sustain", 1),
        //     "Envelope Sustain",
        //     juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f),
        //     0.7f,
        //     juce::AudioParameterFloatAttributes().withLabel ("%")
        //     .withStringFromValueFunction ([](float value, int) { return juce::String (value * 100.0f) + "%"; })
        // ));

        // // Envelope Release (0.0 to 10.0 seconds)
        // params.push_back (std::make_unique<juce::AudioParameterFloat>(
        //     juce::ParameterID ("env_release", 1),
        //     "Envelope Release",
        //     juce::NormalisableRange<float> (0.0f, 10.0f, 0.01f, 0.3f),
        //     0.2f,
        //     juce::AudioParameterFloatAttributes().withLabel ("s")
        //     .withStringFromValueFunction ([](float value, int) { return juce::String (value, 2) + " s"; })
        // ));

        // // Master Volume (0.0 to 1.0)
        // params.push_back (std::make_unique<juce::AudioParameterFloat>(
        //     juce::ParameterID ("master_volume", 1),
        //     "Master Volume",
        //     juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f),
        //     0.8f,
        //     juce::AudioParameterFloatAttributes().withLabel ("dB")
        //     .withStringFromValueFunction ([](float value, int) {
        //         float dB = juce::Decibels::gainToDecibels (value);
        //         return juce::String (dB, 1) + " dB";
        //     })
        // ));

        // // Pitch Bend Range (0 to 24 semitones)
        // params.push_back (std::make_unique<juce::AudioParameterInt>(
        //     juce::ParameterID ("pitch_bend_range", 1),
        //     "Pitch Bend Range",
        //     0, 24, 2,
        //     juce::AudioParameterIntAttributes().withLabel ("st")
        // ));

        return { params.begin(), params.end() };
    }

    //==========================================================================
    // Update DSP from APVTS Parameters
    //==========================================================================
    // void updateDSPFromParameters (juce::AudioProcessorValueTreeState& apvts, @DSP_CLASS@& dsp)
    // {
    //     // Oscillator parameters
    //     dsp.setOscillatorWaveform (apvts.getRawParameterValue ("osc_waveform")->load());
    //     dsp.setOscillatorDetune (apvts.getRawParameterValue ("osc_detune")->load());
    //     dsp.setOscillatorLevel (apvts.getRawParameterValue ("osc_level")->load());
    //
    //     // Filter parameters
    //     dsp.setFilterCutoff (apvts.getRawParameterValue ("filter_cutoff")->load());
    //     dsp.setFilterResonance (apvts.getRawParameterValue ("filter_resonance")->load());
    //
    //     // Envelope parameters
    //     dsp.setEnvelopeAttack (apvts.getRawParameterValue ("env_attack")->load());
    //     dsp.setEnvelopeDecay (apvts.getRawParameterValue ("env_decay")->load());
    //     dsp.setEnvelopeSustain (apvts.getRawParameterValue ("env_sustain")->load());
    //     dsp.setEnvelopeRelease (apvts.getRawParameterValue ("env_release")->load());
    //
    //     // Master parameters
    //     dsp.setMasterVolume (apvts.getRawParameterValue ("master_volume")->load());
    //     dsp.setPitchBendRange (static_cast<int> (apvts.getRawParameterValue ("pitch_bend_range")->load()));
    // }
}
