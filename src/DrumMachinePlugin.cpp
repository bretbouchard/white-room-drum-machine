/*
  ==============================================================================

    DrumMachinePlugin.cpp
    Created: 19 Jan 2026 5:00:00pm
    Author:  White Room Audio

    JUCE AudioProcessor wrapper for Drum Machine step sequencer.

  ==============================================================================
*/

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "dsp/DrumMachinePureDSP.h"

//==============================================================================
class DrumMachinePlugin  : public juce::AudioProcessor
{
public:
    //==============================================================================
    DrumMachinePlugin()
        : AudioProcessor (BusesProperties()
                           .withInput ("Input",  juce::AudioChannelSet::stereo())
                           .withOutput ("Output",  juce::AudioChannelSet::stereo())),
          currentPresetIndex (0),
          sampleRate (48000.0)
    {
        // Initialize global parameters
        addParameter (tempoParam = new juce::AudioParameterFloat ("tempo", "Tempo", 60.0f, 200.0f, 120.0f));
        addParameter (swingParam = new juce::AudioParameterFloat ("swing", "Swing", 0.0f, 1.0f, 0.0f));
        addParameter (masterVolumeParam = new juce::AudioParameterFloat ("master", "Master", 0.0f, 1.0f, 0.8f));
        addParameter (patternLengthParam = new juce::AudioParameterFloat ("patternLength", "Pattern Length", 1.0f, 16.0f, 16.0f));

        // Role timing parameters
        addParameter (pocketOffsetParam = new juce::AudioParameterFloat ("pocketOffset", "Pocket Offset", -0.1f, 0.1f, 0.0f));
        addParameter (pushOffsetParam = new juce::AudioParameterFloat ("pushOffset", "Push Offset", -0.1f, 0.1f, -0.04f));
        addParameter (pullOffsetParam = new juce::AudioParameterFloat ("pullOffset", "Pull Offset", -0.1f, 0.1f, 0.06f));

        // Dilla timing parameters
        addParameter (dillaAmountParam = new juce::AudioParameterFloat ("dillaAmount", "Dilla Amount", 0.0f, 1.0f, 0.6f));
        addParameter (dillaHatBiasParam = new juce::AudioParameterFloat ("dillaHatBias", "Dilla Hat Bias", 0.0f, 1.0f, 0.55f));
        addParameter (dillaSnareLateParam = new juce::AudioParameterFloat ("dillaSnareLate", "Dilla Snare Late", 0.0f, 1.0f, 0.8f));
        addParameter (dillaKickTightParam = new juce::AudioParameterFloat ("dillaKickTight", "Dilla Kick Tight", 0.0f, 1.0f, 0.7f));
        addParameter (dillaMaxDriftParam = new juce::AudioParameterFloat ("dillaMaxDrift", "Dilla Max Drift", 0.0f, 0.3f, 0.15f));

        // Structure parameter
        addParameter (structureParam = new juce::AudioParameterFloat ("structure", "Structure", 0.0f, 1.0f, 0.5f));

        // Stereo enhancement
        addParameter (stereoWidthParam = new juce::AudioParameterFloat ("stereoWidth", "Stereo Width", 0.0f, 1.0f, 0.5f));
        addParameter (roomWidthParam = new juce::AudioParameterFloat ("roomWidth", "Room Width", 0.0f, 1.0f, 0.3f));
        addParameter (effectsWidthParam = new juce::AudioParameterFloat ("effectsWidth", "Effects Width", 0.0f, 1.0f, 0.7f));

        // Track volumes (16 tracks)
        for (int i = 0; i < 16; ++i)
        {
            auto paramName = "trackVolume_" + juce::String(i);
            auto paramLabel = "Track " + juce::String(i + 1) + " Vol";
            addParameter (trackVolumeParams[i] = new juce::AudioParameterFloat (paramName, paramLabel, 0.0f, 1.0f, 0.8f));
        }

        // Load factory presets
        loadFactoryPresets();

        // Apply default preset
        if (!factoryPresets.empty())
        {
            currentPreset = factoryPresets[0];
            applyPresetToDSP();
        }
    }

    ~DrumMachinePlugin() override = default;

    //==============================================================================
    void prepareToPlay (double newSampleRate, int samplesPerBlock) override
    {
        sampleRate = newSampleRate;
        drumMachine.prepare (sampleRate, samplesPerBlock);
    }

    void releaseResources() override
    {
        drumMachine.reset();
    }

    void processBlock (juce::AudioBuffer<float>& buffer,
                       juce::MidiBuffer& midiMessages) override
    {
        juce::ScopedNoDenormals noDenormals;
        auto totalNumInputChannels  = getTotalNumInputChannels();
        auto totalNumOutputChannels = getTotalNumOutputChannels();

        // Clear output buffer
        for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
            buffer.clear (i, 0, buffer.getNumSamples());

        // Update DSP parameters from host
        updateDSPParameters();

        // Process MIDI for sequencer control
        for (const auto metadata : midiMessages)
        {
            const auto message = metadata.getMessage();

            // Note On triggers drums (mapped to tracks)
            if (message.isNoteOn())
            {
                auto midiNote = message.getNoteNumber();
                auto velocity = message.getVelocity() / 127.0f;

                // Map MIDI notes to drum tracks
                int trackIndex = midiNote % 16;
                drumMachine.setParameter ("trackTrigger", static_cast<float>(trackIndex));
            }
        }

        // Process audio (stereo output)
        auto numSamples = buffer.getNumSamples();
        auto* outputLeft = buffer.getWritePointer(0);
        auto* outputRight = buffer.getWritePointer(1);

        // Create output array for DSP
        float* outputs[2] = { outputLeft, outputRight };
        drumMachine.process (outputs, 2, numSamples);

        // Clear remaining channels if any
        for (int channel = 2; channel < totalNumOutputChannels; ++channel)
            buffer.clear(channel, 0, numSamples);
    }

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override
    {
        return nullptr;
    }

    bool hasEditor() const override
    {
        return false;
    }

    //==============================================================================
    const juce::String getName() const override
    {
        return "Drum Machine";
    }

    bool acceptsMidi() const override
    {
        return true;
    }

    bool producesMidi() const override
    {
        return false;
    }

    double getTailLengthSeconds() const override
    {
        return 0.0;
    }

    //==============================================================================
    int getNumPrograms() override
    {
        return static_cast<int>(factoryPresets.size());
    }

    int getCurrentProgram() override
    {
        return currentPresetIndex;
    }

    void setCurrentProgram (int index) override
    {
        if (index >= 0 && index < static_cast<int>(factoryPresets.size()))
        {
            currentPresetIndex = index;
            currentPreset = factoryPresets[index];
            applyPresetToDSP();
        }
    }

    const juce::String getProgramName (int index) override
    {
        if (index >= 0 && index < static_cast<int>(factoryPresets.size()))
            return factoryPresets[index].name;

        return {};
    }

    void changeProgramName (int index, const juce::String& newName) override
    {
        if (index >= 0 && index < static_cast<int>(factoryPresets.size()))
            factoryPresets[index].name = newName;
    }

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override
    {
        // Save current preset and parameters
        juce::ValueTree state ("state");

        state.setProperty ("tempo", tempoParam->get(), nullptr);
        state.setProperty ("swing", swingParam->get(), nullptr);
        state.setProperty ("master", masterVolumeParam->get(), nullptr);
        state.setProperty ("patternLength", patternLengthParam->get(), nullptr);
        state.setProperty ("pocketOffset", pocketOffsetParam->get(), nullptr);
        state.setProperty ("pushOffset", pushOffsetParam->get(), nullptr);
        state.setProperty ("pullOffset", pullOffsetParam->get(), nullptr);
        state.setProperty ("dillaAmount", dillaAmountParam->get(), nullptr);
        state.setProperty ("dillaHatBias", dillaHatBiasParam->get(), nullptr);
        state.setProperty ("dillaSnareLate", dillaSnareLateParam->get(), nullptr);
        state.setProperty ("dillaKickTight", dillaKickTightParam->get(), nullptr);
        state.setProperty ("dillaMaxDrift", dillaMaxDriftParam->get(), nullptr);
        state.setProperty ("structure", structureParam->get(), nullptr);
        state.setProperty ("stereoWidth", stereoWidthParam->get(), nullptr);
        state.setProperty ("roomWidth", roomWidthParam->get(), nullptr);
        state.setProperty ("effectsWidth", effectsWidthParam->get(), nullptr);
        state.setProperty ("preset", currentPresetIndex, nullptr);

        juce::MemoryOutputStream stream (destData, false);
        state.writeToStream (stream);
    }

    void setStateInformation (const void* data, int sizeInBytes) override
    {
        auto xml = juce::XmlDocument::parse (juce::String ((const char*) data, sizeInBytes));

        if (xml != nullptr)
        {
            auto state = juce::ValueTree::fromXml (*xml);

            if (state.isValid())
            {
                *tempoParam = state.getProperty ("tempo", 120.0f);
                *swingParam = state.getProperty ("swing", 0.0f);
                *masterVolumeParam = state.getProperty ("master", 0.8f);
                *patternLengthParam = state.getProperty ("patternLength", 16.0f);
                *pocketOffsetParam = state.getProperty ("pocketOffset", 0.0f);
                *pushOffsetParam = state.getProperty ("pushOffset", -0.04f);
                *pullOffsetParam = state.getProperty ("pullOffset", 0.06f);
                *dillaAmountParam = state.getProperty ("dillaAmount", 0.6f);
                *dillaHatBiasParam = state.getProperty ("dillaHatBias", 0.55f);
                *dillaSnareLateParam = state.getProperty ("dillaSnareLate", 0.8f);
                *dillaKickTightParam = state.getProperty ("dillaKickTight", 0.7f);
                *dillaMaxDriftParam = state.getProperty ("dillaMaxDrift", 0.15f);
                *structureParam = state.getProperty ("structure", 0.5f);
                *stereoWidthParam = state.getProperty ("stereoWidth", 0.5f);
                *roomWidthParam = state.getProperty ("roomWidth", 0.3f);
                *effectsWidthParam = state.getProperty ("effectsWidth", 0.7f);

                currentPresetIndex = state.getProperty ("preset", 0);

                if (currentPresetIndex >= 0 && currentPresetIndex < static_cast<int>(factoryPresets.size()))
                {
                    currentPreset = factoryPresets[currentPresetIndex];
                }
            }
        }

        applyPresetToDSP();
    }

private:
    //==============================================================================
    /**
        Preset structure for Drum Machine
    */
    struct Preset
    {
        juce::String name;
        float tempo;
        float swing;
        float masterVolume;
        float pocketOffset;
        float pushOffset;
        float pullOffset;
        float dillaAmount;
        float dillaHatBias;
        float dillaSnareLate;
        float dillaKickTight;
        float dillaMaxDrift;
        float structure;
        float stereoWidth;
    };

    //==============================================================================
    /**
        Load all factory presets
    */
    void loadFactoryPresets()
    {
        factoryPresets.clear();

        // Preset 1: Basic 808
        Preset basic808;
        basic808.name = "Basic 808";
        basic808.tempo = 120.0f;
        basic808.swing = 0.0f;
        basic808.masterVolume = 0.8f;
        basic808.pocketOffset = 0.0f;
        basic808.pushOffset = -0.04f;
        basic808.pullOffset = 0.06f;
        basic808.dillaAmount = 0.0f;
        basic808.dillaHatBias = 0.5f;
        basic808.dillaSnareLate = 0.5f;
        basic808.dillaKickTight = 0.9f;
        basic808.dillaMaxDrift = 0.05f;
        basic808.structure = 0.3f;
        basic808.stereoWidth = 0.5f;
        factoryPresets.push_back (basic808);

        // Preset 2: J Dilla Style
        Preset dilla;
        dilla.name = "J Dilla Style";
        dilla.tempo = 95.0f;
        dilla.swing = 0.6f;
        dilla.masterVolume = 0.8f;
        dilla.pocketOffset = 0.0f;
        dilla.pushOffset = -0.05f;
        dilla.pullOffset = 0.08f;
        dilla.dillaAmount = 0.7f;
        dilla.dillaHatBias = 0.6f;
        dilla.dillaSnareLate = 0.9f;
        dilla.dillaKickTight = 0.6f;
        dilla.dillaMaxDrift = 0.12f;
        dilla.structure = 0.6f;
        dilla.stereoWidth = 0.6f;
        factoryPresets.push_back (dilla);

        // Preset 3: Tight House
        Preset house;
        house.name = "Tight House";
        house.tempo = 128.0f;
        house.swing = 0.0f;
        house.masterVolume = 0.85f;
        house.pocketOffset = 0.0f;
        house.pushOffset = 0.0f;
        house.pullOffset = 0.0f;
        house.dillaAmount = 0.0f;
        house.dillaHatBias = 0.5f;
        house.dillaSnareLate = 0.5f;
        house.dillaKickTight = 1.0f;
        house.dillaMaxDrift = 0.01f;
        house.structure = 0.2f;
        house.stereoWidth = 0.4f;
        factoryPresets.push_back (house);

        // Preset 4: Loose Hip Hop
        Preset hiphop;
        hiphop.name = "Loose Hip Hop";
        hiphop.tempo = 92.0f;
        hiphop.swing = 0.55f;
        hiphop.masterVolume = 0.8f;
        hiphop.pocketOffset = 0.02f;
        hiphop.pushOffset = -0.03f;
        hiphop.pullOffset = 0.07f;
        hiphop.dillaAmount = 0.5f;
        hiphop.dillaHatBias = 0.55f;
        hiphop.dillaSnareLate = 0.7f;
        hiphop.dillaKickTight = 0.5f;
        hiphop.dillaMaxDrift = 0.1f;
        hiphop.structure = 0.5f;
        hiphop.stereoWidth = 0.7f;
        factoryPresets.push_back (hiphop);

        // Preset 5: Drum & Bass
        Preset dnb;
        dnb.name = "Drum & Bass";
        dnb.tempo = 174.0f;
        dnb.swing = 0.1f;
        dnb.masterVolume = 0.8f;
        dnb.pocketOffset = 0.0f;
        dnb.pushOffset = -0.02f;
        dnb.pullOffset = 0.02f;
        dnb.dillaAmount = 0.3f;
        dnb.dillaHatBias = 0.5f;
        dnb.dillaSnareLate = 0.6f;
        dnb.dillaKickTight = 0.8f;
        dnb.dillaMaxDrift = 0.05f;
        dnb.structure = 0.7f;
        dnb.stereoWidth = 0.8f;
        factoryPresets.push_back (dnb);

        // Preset 6: IDM Drill
        Preset idm;
        idm.name = "IDM Drill";
        idm.tempo = 160.0f;
        idm.swing = 0.4f;
        idm.masterVolume = 0.75f;
        idm.pocketOffset = 0.0f;
        idm.pushOffset = -0.06f;
        idm.pullOffset = 0.1f;
        idm.dillaAmount = 0.8f;
        idm.dillaHatBias = 0.6f;
        idm.dillaSnareLate = 0.9f;
        idm.dillaKickTight = 0.4f;
        idm.dillaMaxDrift = 0.2f;
        idm.structure = 0.9f;
        idm.stereoWidth = 0.9f;
        factoryPresets.push_back (idm);

        // Preset 7: Techno
        Preset techno;
        techno.name = "Techno";
        techno.tempo = 130.0f;
        techno.swing = 0.0f;
        techno.masterVolume = 0.9f;
        techno.pocketOffset = 0.0f;
        techno.pushOffset = 0.0f;
        techno.pullOffset = 0.0f;
        techno.dillaAmount = 0.0f;
        techno.dillaHatBias = 0.5f;
        techno.dillaSnareLate = 0.5f;
        techno.dillaKickTight = 1.0f;
        techno.dillaMaxDrift = 0.0f;
        techno.structure = 0.4f;
        techno.stereoWidth = 0.6f;
        factoryPresets.push_back (techno);

        // Preset 8: Afrobeat
        Preset afrobeat;
        afrobeat.name = "Afrobeat";
        afrobeat.tempo = 110.0f;
        afrobeat.swing = 0.3f;
        afrobeat.masterVolume = 0.8f;
        afrobeat.pocketOffset = 0.0f;
        afrobeat.pushOffset = -0.01f;
        afrobeat.pullOffset = 0.03f;
        afrobeat.dillaAmount = 0.2f;
        afrobeat.dillaHatBias = 0.5f;
        afrobeat.dillaSnareLate = 0.5f;
        afrobeat.dillaKickTight = 0.7f;
        afrobeat.dillaMaxDrift = 0.08f;
        afrobeat.structure = 0.5f;
        afrobeat.stereoWidth = 0.7f;
        factoryPresets.push_back (afrobeat);

        // Preset 9: Breakbeat
        Preset breakbeat;
        breakbeat.name = "Breakbeat";
        breakbeat.tempo = 140.0f;
        breakbeat.swing = 0.5f;
        breakbeat.masterVolume = 0.8f;
        breakbeat.pocketOffset = 0.01f;
        breakbeat.pushOffset = -0.04f;
        breakbeat.pullOffset = 0.08f;
        breakbeat.dillaAmount = 0.6f;
        breakbeat.dillaHatBias = 0.55f;
        breakbeat.dillaSnareLate = 0.7f;
        breakbeat.dillaKickTight = 0.5f;
        breakbeat.dillaMaxDrift = 0.12f;
        breakbeat.structure = 0.7f;
        breakbeat.stereoWidth = 0.8f;
        factoryPresets.push_back (breakbeat);

        // Preset 10: Minimal
        Preset minimal;
        minimal.name = "Minimal";
        minimal.tempo = 125.0f;
        minimal.swing = 0.0f;
        minimal.masterVolume = 0.7f;
        minimal.pocketOffset = 0.0f;
        minimal.pushOffset = 0.0f;
        minimal.pullOffset = 0.0f;
        minimal.dillaAmount = 0.0f;
        minimal.dillaHatBias = 0.5f;
        minimal.dillaSnareLate = 0.5f;
        minimal.dillaKickTight = 1.0f;
        minimal.dillaMaxDrift = 0.0f;
        minimal.structure = 0.1f;
        minimal.stereoWidth = 0.3f;
        factoryPresets.push_back (minimal);
    }

    //==============================================================================
    /**
        Apply current preset to DSP
    */
    void applyPresetToDSP()
    {
        drumMachine.setParameter ("tempo", currentPreset.tempo);
        drumMachine.setParameter ("swing", currentPreset.swing);
        drumMachine.setParameter ("masterVolume", currentPreset.masterVolume);
        drumMachine.setParameter ("pocketOffset", currentPreset.pocketOffset);
        drumMachine.setParameter ("pushOffset", currentPreset.pushOffset);
        drumMachine.setParameter ("pullOffset", currentPreset.pullOffset);
        drumMachine.setParameter ("dillaAmount", currentPreset.dillaAmount);
        drumMachine.setParameter ("dillaHatBias", currentPreset.dillaHatBias);
        drumMachine.setParameter ("dillaSnareLate", currentPreset.dillaSnareLate);
        drumMachine.setParameter ("dillaKickTight", currentPreset.dillaKickTight);
        drumMachine.setParameter ("dillaMaxDrift", currentPreset.dillaMaxDrift);
        drumMachine.setParameter ("structure", currentPreset.structure);
        drumMachine.setParameter ("stereoWidth", currentPreset.stereoWidth);
    }

    //==============================================================================
    /**
        Update DSP parameters from host automation
    */
    void updateDSPParameters()
    {
        drumMachine.setParameter ("tempo", tempoParam->get());
        drumMachine.setParameter ("swing", swingParam->get());
        drumMachine.setParameter ("masterVolume", masterVolumeParam->get());
        drumMachine.setParameter ("patternLength", patternLengthParam->get());
        drumMachine.setParameter ("pocketOffset", pocketOffsetParam->get());
        drumMachine.setParameter ("pushOffset", pushOffsetParam->get());
        drumMachine.setParameter ("pullOffset", pullOffsetParam->get());
        drumMachine.setParameter ("dillaAmount", dillaAmountParam->get());
        drumMachine.setParameter ("dillaHatBias", dillaHatBiasParam->get());
        drumMachine.setParameter ("dillaSnareLate", dillaSnareLateParam->get());
        drumMachine.setParameter ("dillaKickTight", dillaKickTightParam->get());
        drumMachine.setParameter ("dillaMaxDrift", dillaMaxDriftParam->get());
        drumMachine.setParameter ("structure", structureParam->get());
        drumMachine.setParameter ("stereoWidth", stereoWidthParam->get());
        drumMachine.setParameter ("roomWidth", roomWidthParam->get());
        drumMachine.setParameter ("effectsWidth", effectsWidthParam->get());

        // Update track volumes
        for (int i = 0; i < 16; ++i)
        {
            auto paramName = "trackVolume_" + std::to_string(i);
            drumMachine.setParameter (paramName.c_str(), trackVolumeParams[i]->get());
        }
    }

    //==============================================================================
    // DSP instance
    DSP::DrumMachinePureDSP drumMachine;

    // Global parameters
    juce::AudioParameterFloat* tempoParam;
    juce::AudioParameterFloat* swingParam;
    juce::AudioParameterFloat* masterVolumeParam;
    juce::AudioParameterFloat* patternLengthParam;

    // Role timing parameters
    juce::AudioParameterFloat* pocketOffsetParam;
    juce::AudioParameterFloat* pushOffsetParam;
    juce::AudioParameterFloat* pullOffsetParam;

    // Dilla timing parameters
    juce::AudioParameterFloat* dillaAmountParam;
    juce::AudioParameterFloat* dillaHatBiasParam;
    juce::AudioParameterFloat* dillaSnareLateParam;
    juce::AudioParameterFloat* dillaKickTightParam;
    juce::AudioParameterFloat* dillaMaxDriftParam;

    // Structure and stereo
    juce::AudioParameterFloat* structureParam;
    juce::AudioParameterFloat* stereoWidthParam;
    juce::AudioParameterFloat* roomWidthParam;
    juce::AudioParameterFloat* effectsWidthParam;

    // Track volumes (16 tracks)
    juce::AudioParameterFloat* trackVolumeParams[16];

    // Preset system
    std::vector<Preset> factoryPresets;
    Preset currentPreset;
    int currentPresetIndex;

    // State
    double sampleRate;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DrumMachinePlugin)
};

//==============================================================================
// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DrumMachinePlugin();
}
