//
//  DrumMachineDSP.h
//  SharedDSP
//
//  C++ wrapper for Drum Machine DSP
//  Provides interface for AUv3 extension to access DSP functionality
//

#ifndef DrumMachineDSP_h
#define DrumMachineDSP_h

#include <AudioToolbox/AudioToolbox.h>
#include <memory>

class DrumMachineDSP {
public:
    DrumMachineDSP();
    ~DrumMachineDSP();

    // Initialization
    void initialize(double sampleRate, int maximumFramesToRender);

    // DSP Processing
    void process(AUAudioFrameCount frameCount,
                AudioBufferList *outputBufferList,
                const AUEventSampleTime *timestamp,
                AUAudioFrameCount inputBusNumber = 0);

    // Parameters
    enum ParameterAddress : AUParameterAddress {
        // Global parameters
        Tempo = 0,
        Swing,
        MasterVolume,
        PatternLength,

        // Timing roles
        PocketOffset,
        PushOffset,
        PullOffset,

        // Dilla parameters
        DillaAmount,
        DillaHatBias,
        DillaSnareLate,
        DillaKickTight,
        DillaMaxDrift,

        // Structure
        Structure,

        // Stereo enhancement
        StereoWidth,
        RoomWidth,
        EffectsWidth,

        // Per-track volumes (16 tracks)
        TrackVolume0,
        TrackVolume1,
        TrackVolume2,
        TrackVolume3,
        TrackVolume4,
        TrackVolume5,
        TrackVolume6,
        TrackVolume7,
        TrackVolume8,
        TrackVolume9,
        TrackVolume10,
        TrackVolume11,
        TrackVolume12,
        TrackVolume13,
        TrackVolume14,
        TrackVolume15,

        // Voice parameters (Kick)
        KickPitch,
        KickDecay,
        KickClick,

        // Voice parameters (Snare)
        SnareTone,
        SnareDecay,
        SnareSnap,

        // Voice parameters (HiHat Closed)
        HiHatClosedTone,
        HiHatClosedDecay,
        HiHatClosedMetallic,

        // Voice parameters (HiHat Open)
        HiHatOpenTone,
        HiHatOpenDecay,
        HiHatOpenMetallic,

        // Voice parameters (Clap)
        ClapTone,
        ClapDecay,
        ClapNumImpulses,

        // Voice parameters (Tom Low)
        TomLowPitch,
        TomLowDecay,
        TomLowTone,

        // Voice parameters (Tom Mid)
        TomMidPitch,
        TomMidDecay,
        TomMidTone,

        // Voice parameters (Tom High)
        TomHighPitch,
        TomHighDecay,
        TomHighTone,

        // Voice parameters (Crash)
        CrashTone,
        CrashDecay,
        CrashMetallic,

        // Voice parameters (Ride)
        RideTone,
        RideDecay,
        RideMetallic,

        // Voice parameters (Cowbell)
        CowbellPitch,
        CowbellDecay,
        CowbellTone,

        // Voice parameters (Shaker)
        ShakerTone,
        ShakerDecay,
        ShakerMetallic,

        // Voice parameters (Tambourine)
        TambourineTone,
        TambourineDecay,
        TambourineMetallic,

        // Voice parameters (Percussion)
        PercussionPitch,
        PercussionDecay,
        PercussionTone,

        // Voice parameters (Special)
        SpecialTone,
        SpecialDecay,
        SpecialSnap,

        // Transport control
        TransportPlay,
        TransportStop,
        TransportRecord,

        // Pattern control
        PatternClear,
        PatternRandomize,
    };

    void setParameter(AUParameterAddress address, float value);
    float getParameter(AUParameterAddress address) const;

    // MIDI
    void handleMIDIEvent(const uint8_t *message, uint8_t messageSize);

    // Step sequencer control
    void setStep(int track, int step, bool active, uint8_t velocity);
    bool getStep(int track, int step) const;
    uint8_t getStepVelocity(int track, int step) const;

    // Presets
    void setState(const char *stateData);
    const char *getState() const;

    // Pattern save/load
    bool savePattern(char *jsonBuffer, int jsonBufferSize) const;
    bool loadPattern(const char *jsonData);

    // Kit save/load
    bool saveKit(char *jsonBuffer, int jsonBufferSize) const;
    bool loadKit(const char *jsonData);

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

#endif /* DrumMachineDSP_h */
