//
//  DrumMachineDSP.cpp
//  SharedDSP
//
//  C++ wrapper implementation for Drum Machine DSP
//

#include "DrumMachineDSP.h"
#include "dsp/DrumMachinePureDSP.h"
#include <cstring>
#include <cstdlib>
#include <cmath>

class DrumMachineDSP::Impl {
public:
    Impl() : dsp_(nullptr) {
        dsp_ = new DSP::DrumMachinePureDSP();
        dsp_->prepare(48000.0, 512);
    }

    ~Impl() {
        delete dsp_;
    }

    void initialize(double sampleRate, int maximumFramesToRender) {
        if (dsp_) {
            dsp_->prepare(sampleRate, maximumFramesToRender);
        }
    }

    void process(AUAudioFrameCount frameCount,
                AudioBufferList *outputBufferList,
                const AUEventSampleTime *timestamp,
                AUAudioFrameCount inputBusNumber) {
        if (!dsp_) return;

        // Process audio
        float* outputs[2] = {
            static_cast<float*>(outputBufferList->mBuffers[0].mData),
            static_cast<float*>(outputBufferList->mBuffers[1].mData)
        };

        // Render stereo output
        float** outputPtr = outputs;
        dsp_->process(outputPtr, 2, static_cast<int>(frameCount));
    }

    void setParameter(AUParameterAddress address, float value) {
        if (!dsp_) return;

        // Map parameter address to DSP parameter
        switch (static_cast<ParameterAddress>(address)) {
            case Tempo:
                dsp_->setParameter("tempo", value);
                break;
            case Swing:
                dsp_->setParameter("swing", value);
                break;
            case MasterVolume:
                dsp_->setParameter("masterVolume", value);
                break;
            case PatternLength:
                dsp_->setParameter("patternLength", value);
                break;
            case PocketOffset:
                dsp_->setParameter("pocketOffset", value);
                break;
            case PushOffset:
                dsp_->setParameter("pushOffset", value);
                break;
            case PullOffset:
                dsp_->setParameter("pullOffset", value);
                break;
            case DillaAmount:
                dsp_->setParameter("dillaAmount", value);
                break;
            case DillaHatBias:
                dsp_->setParameter("dillaHatBias", value);
                break;
            case DillaSnareLate:
                dsp_->setParameter("dillaSnareLate", value);
                break;
            case DillaKickTight:
                dsp_->setParameter("dillaKickTight", value);
                break;
            case DillaMaxDrift:
                dsp_->setParameter("dillaMaxDrift", value);
                break;
            case Structure:
                dsp_->setParameter("structure", value);
                break;
            case StereoWidth:
                dsp_->setParameter("stereoWidth", value);
                break;
            case RoomWidth:
                dsp_->setParameter("roomWidth", value);
                break;
            case EffectsWidth:
                dsp_->setParameter("effectsWidth", value);
                break;

            // Per-track volumes
            case TrackVolume0: case TrackVolume1: case TrackVolume2: case TrackVolume3:
            case TrackVolume4: case TrackVolume5: case TrackVolume6: case TrackVolume7:
            case TrackVolume8: case TrackVolume9: case TrackVolume10: case TrackVolume11:
            case TrackVolume12: case TrackVolume13: case TrackVolume14: case TrackVolume15:
            {
                int trackIndex = static_cast<int>(address) - static_cast<int>(TrackVolume0);
                char paramId[32];
                snprintf(paramId, sizeof(paramId), "trackVolume_%d", trackIndex);
                dsp_->setParameter(paramId, value);
                break;
            }

            // Voice parameters (Kick)
            case KickPitch:
                dsp_->setParameter("kickPitch", value);
                break;
            case KickDecay:
                dsp_->setParameter("kickDecay", value);
                break;
            case KickClick:
                dsp_->setParameter("kickClick", value);
                break;

            // Voice parameters (Snare)
            case SnareTone:
                dsp_->setParameter("snareTone", value);
                break;
            case SnareDecay:
                dsp_->setParameter("snareDecay", value);
                break;
            case SnareSnap:
                dsp_->setParameter("snareSnap", value);
                break;

            // Voice parameters (HiHat Closed)
            case HiHatClosedTone:
                dsp_->setParameter("hihatClosedTone", value);
                break;
            case HiHatClosedDecay:
                dsp_->setParameter("hihatClosedDecay", value);
                break;
            case HiHatClosedMetallic:
                dsp_->setParameter("hihatClosedMetallic", value);
                break;

            // Voice parameters (HiHat Open)
            case HiHatOpenTone:
                dsp_->setParameter("hihatOpenTone", value);
                break;
            case HiHatOpenDecay:
                dsp_->setParameter("hihatOpenDecay", value);
                break;
            case HiHatOpenMetallic:
                dsp_->setParameter("hihatOpenMetallic", value);
                break;

            // Voice parameters (Clap)
            case ClapTone:
                dsp_->setParameter("clapTone", value);
                break;
            case ClapDecay:
                dsp_->setParameter("clapDecay", value);
                break;
            case ClapNumImpulses:
                dsp_->setParameter("clapNumImpulses", value);
                break;

            // Voice parameters (Tom Low)
            case TomLowPitch:
                dsp_->setParameter("tomLowPitch", value);
                break;
            case TomLowDecay:
                dsp_->setParameter("tomLowDecay", value);
                break;
            case TomLowTone:
                dsp_->setParameter("tomLowTone", value);
                break;

            // Voice parameters (Tom Mid)
            case TomMidPitch:
                dsp_->setParameter("tomMidPitch", value);
                break;
            case TomMidDecay:
                dsp_->setParameter("tomMidDecay", value);
                break;
            case TomMidTone:
                dsp_->setParameter("tomMidTone", value);
                break;

            // Voice parameters (Tom High)
            case TomHighPitch:
                dsp_->setParameter("tomHighPitch", value);
                break;
            case TomHighDecay:
                dsp_->setParameter("tomHighDecay", value);
                break;
            case TomHighTone:
                dsp_->setParameter("tomHighTone", value);
                break;

            // Voice parameters (Crash)
            case CrashTone:
                dsp_->setParameter("crashTone", value);
                break;
            case CrashDecay:
                dsp_->setParameter("crashDecay", value);
                break;
            case CrashMetallic:
                dsp_->setParameter("crashMetallic", value);
                break;

            // Voice parameters (Ride)
            case RideTone:
                dsp_->setParameter("rideTone", value);
                break;
            case RideDecay:
                dsp_->setParameter("rideDecay", value);
                break;
            case RideMetallic:
                dsp_->setParameter("rideMetallic", value);
                break;

            // Voice parameters (Cowbell)
            case CowbellPitch:
                dsp_->setParameter("cowbellPitch", value);
                break;
            case CowbellDecay:
                dsp_->setParameter("cowbellDecay", value);
                break;
            case CowbellTone:
                dsp_->setParameter("cowbellTone", value);
                break;

            // Voice parameters (Shaker)
            case ShakerTone:
                dsp_->setParameter("shakerTone", value);
                break;
            case ShakerDecay:
                dsp_->setParameter("shakerDecay", value);
                break;
            case ShakerMetallic:
                dsp_->setParameter("shakerMetallic", value);
                break;

            // Voice parameters (Tambourine)
            case TambourineTone:
                dsp_->setParameter("tambourineTone", value);
                break;
            case TambourineDecay:
                dsp_->setParameter("tambourineDecay", value);
                break;
            case TambourineMetallic:
                dsp_->setParameter("tambourineMetallic", value);
                break;

            // Voice parameters (Percussion)
            case PercussionPitch:
                dsp_->setParameter("percussionPitch", value);
                break;
            case PercussionDecay:
                dsp_->setParameter("percussionDecay", value);
                break;
            case PercussionTone:
                dsp_->setParameter("percussionTone", value);
                break;

            // Voice parameters (Special)
            case SpecialTone:
                dsp_->setParameter("specialTone", value);
                break;
            case SpecialDecay:
                dsp_->setParameter("specialDecay", value);
                break;
            case SpecialSnap:
                dsp_->setParameter("specialSnap", value);
                break;

            default:
                break;
        }
    }

    float getParameter(AUParameterAddress address) const {
        if (!dsp_) return 0.0f;

        // Map parameter address to DSP parameter
        const char* paramId = nullptr;
        switch (static_cast<ParameterAddress>(address)) {
            case Tempo: paramId = "tempo"; break;
            case Swing: paramId = "swing"; break;
            case MasterVolume: paramId = "masterVolume"; break;
            case PatternLength: paramId = "patternLength"; break;
            case PocketOffset: paramId = "pocketOffset"; break;
            case PushOffset: paramId = "pushOffset"; break;
            case PullOffset: paramId = "pullOffset"; break;
            case DillaAmount: paramId = "dillaAmount"; break;
            case DillaHatBias: paramId = "dillaHatBias"; break;
            case DillaSnareLate: paramId = "dillaSnareLate"; break;
            case DillaKickTight: paramId = "dillaKickTight"; break;
            case DillaMaxDrift: paramId = "dillaMaxDrift"; break;
            case Structure: paramId = "structure"; break;
            case StereoWidth: paramId = "stereoWidth"; break;
            case RoomWidth: paramId = "roomWidth"; break;
            case EffectsWidth: paramId = "effectsWidth"; break;

            case KickPitch: paramId = "kickPitch"; break;
            case KickDecay: paramId = "kickDecay"; break;
            case KickClick: paramId = "kickClick"; break;

            case SnareTone: paramId = "snareTone"; break;
            case SnareDecay: paramId = "snareDecay"; break;
            case SnareSnap: paramId = "snareSnap"; break;

            case HiHatClosedTone: paramId = "hihatClosedTone"; break;
            case HiHatClosedDecay: paramId = "hihatClosedDecay"; break;
            case HiHatClosedMetallic: paramId = "hihatClosedMetallic"; break;

            case HiHatOpenTone: paramId = "hihatOpenTone"; break;
            case HiHatOpenDecay: paramId = "hihatOpenDecay"; break;
            case HiHatOpenMetallic: paramId = "hihatOpenMetallic"; break;

            case ClapTone: paramId = "clapTone"; break;
            case ClapDecay: paramId = "clapDecay"; break;
            case ClapNumImpulses: paramId = "clapNumImpulses"; break;

            case TomLowPitch: paramId = "tomLowPitch"; break;
            case TomLowDecay: paramId = "tomLowDecay"; break;
            case TomLowTone: paramId = "tomLowTone"; break;

            case TomMidPitch: paramId = "tomMidPitch"; break;
            case TomMidDecay: paramId = "tomMidDecay"; break;
            case TomMidTone: paramId = "tomMidTone"; break;

            case TomHighPitch: paramId = "tomHighPitch"; break;
            case TomHighDecay: paramId = "tomHighDecay"; break;
            case TomHighTone: paramId = "tomHighTone"; break;

            case CrashTone: paramId = "crashTone"; break;
            case CrashDecay: paramId = "crashDecay"; break;
            case CrashMetallic: paramId = "crashMetallic"; break;

            case RideTone: paramId = "rideTone"; break;
            case RideDecay: paramId = "rideDecay"; break;
            case RideMetallic: paramId = "rideMetallic"; break;

            case CowbellPitch: paramId = "cowbellPitch"; break;
            case CowbellDecay: paramId = "cowbellDecay"; break;
            case CowbellTone: paramId = "cowbellTone"; break;

            case ShakerTone: paramId = "shakerTone"; break;
            case ShakerDecay: paramId = "shakerDecay"; break;
            case ShakerMetallic: paramId = "shakerMetallic"; break;

            case TambourineTone: paramId = "tambourineTone"; break;
            case TambourineDecay: paramId = "tambourineDecay"; break;
            case TambourineMetallic: paramId = "tambourineMetallic"; break;

            case PercussionPitch: paramId = "percussionPitch"; break;
            case PercussionDecay: paramId = "percussionDecay"; break;
            case PercussionTone: paramId = "percussionTone"; break;

            case SpecialTone: paramId = "specialTone"; break;
            case SpecialDecay: paramId = "specialDecay"; break;
            case SpecialSnap: paramId = "specialSnap"; break;

            default:
                // Handle per-track volumes
                if (address >= static_cast<AUParameterAddress>(TrackVolume0) &&
                    address <= static_cast<AUParameterAddress>(TrackVolume15)) {
                    int trackIndex = static_cast<int>(address) - static_cast<int>(TrackVolume0);
                    char paramIdBuffer[32];
                    snprintf(paramIdBuffer, sizeof(paramIdBuffer), "trackVolume_%d", trackIndex);
                    return dsp_->getParameter(paramIdBuffer);
                }
                return 0.0f;
        }

        return dsp_->getParameter(paramId);
    }

    void handleMIDIEvent(const uint8_t *message, uint8_t messageSize) {
        if (!dsp_ || messageSize < 3) return;

        uint8_t status = message[0];
        uint8_t data1 = message[1];
        uint8_t data2 = message[2];

        // Map MIDI note to track (C2 = Kick, D2 = Snare, etc.)
        int trackIndex = -1;
        if (status >= 0x90 && status < 0xA0) { // Note On
            uint8_t note = data1;
            uint8_t velocity = data2;

            // MIDI note to drum mapping
            // C2 (36) = Kick, D2 (38) = Snare, etc.
            if (note >= 36 && note < 52) {
                trackIndex = note - 36; // Map to tracks 0-15
            }

            if (trackIndex >= 0 && trackIndex < 16) {
                DSP::ScheduledEvent event;
                event.type = DSP::ScheduledEventType::NoteOn;
                event.note = trackIndex;
                event.velocity = velocity / 127.0f;
                dsp_->handleEvent(event);
            }
        } else if (status >= 0x80 && status < 0x90) { // Note Off
            uint8_t note = data1;

            if (note >= 36 && note < 52) {
                trackIndex = note - 36;
            }

            if (trackIndex >= 0 && trackIndex < 16) {
                DSP::ScheduledEvent event;
                event.type = DSP::ScheduledEventType::NoteOff;
                event.note = trackIndex;
                dsp_->handleEvent(event);
            }
        }
    }

    void setStep(int track, int step, bool active, uint8_t velocity) {
        if (!dsp_ || track < 0 || track >= 16 || step < 0 || step >= 16) return;
        // Step sequencer control would go here
        // This requires access to the sequencer's internal state
    }

    bool getStep(int track, int step) const {
        if (!dsp_ || track < 0 || track >= 16 || step < 0 || step >= 16) return false;
        // Would return step state from sequencer
        return false;
    }

    uint8_t getStepVelocity(int track, int step) const {
        if (!dsp_ || track < 0 || track >= 16 || step < 0 || step >= 16) return 0;
        // Would return step velocity from sequencer
        return 100;
    }

    void setState(const char *stateData) {
        if (!dsp_ || !stateData) return;
        dsp_->loadPreset(stateData);
    }

    const char *getState() const {
        if (!dsp_) return nullptr;
        static char stateBuffer[65536];
        if (dsp_->savePreset(stateBuffer, sizeof(stateBuffer))) {
            return stateBuffer;
        }
        return nullptr;
    }

    bool savePattern(char *jsonBuffer, int jsonBufferSize) const {
        if (!dsp_) return false;
        return dsp_->savePattern(jsonBuffer, jsonBufferSize);
    }

    bool loadPattern(const char *jsonData) {
        if (!dsp_ || !jsonData) return false;
        return dsp_->loadPattern(jsonData);
    }

    bool saveKit(char *jsonBuffer, int jsonBufferSize) const {
        if (!dsp_) return false;
        return dsp_->saveKit(jsonBuffer, jsonBufferSize);
    }

    bool loadKit(const char *jsonData) {
        if (!dsp_ || !jsonData) return false;
        return dsp_->loadKit(jsonData);
    }

private:
    DSP::DrumMachinePureDSP* dsp_;
};

// Public interface implementation
DrumMachineDSP::DrumMachineDSP() : impl(std::make_unique<Impl>()) {}

DrumMachineDSP::~DrumMachineDSP() = default;

void DrumMachineDSP::initialize(double sampleRate, int maximumFramesToRender) {
    impl->initialize(sampleRate, maximumFramesToRender);
}

void DrumMachineDSP::process(AUAudioFrameCount frameCount,
                            AudioBufferList *outputBufferList,
                            const AUEventSampleTime *timestamp,
                            AUAudioFrameCount inputBusNumber) {
    impl->process(frameCount, outputBufferList, timestamp, inputBusNumber);
}

void DrumMachineDSP::setParameter(AUParameterAddress address, float value) {
    impl->setParameter(address, value);
}

float DrumMachineDSP::getParameter(AUParameterAddress address) const {
    return impl->getParameter(address);
}

void DrumMachineDSP::handleMIDIEvent(const uint8_t *message, uint8_t messageSize) {
    impl->handleMIDIEvent(message, messageSize);
}

void DrumMachineDSP::setStep(int track, int step, bool active, uint8_t velocity) {
    impl->setStep(track, step, active, velocity);
}

bool DrumMachineDSP::getStep(int track, int step) const {
    return impl->getStep(track, step);
}

uint8_t DrumMachineDSP::getStepVelocity(int track, int step) const {
    return impl->getStepVelocity(track, step);
}

void DrumMachineDSP::setState(const char *stateData) {
    impl->setState(stateData);
}

const char *DrumMachineDSP::getState() const {
    return impl->getState();
}

bool DrumMachineDSP::savePattern(char *jsonBuffer, int jsonBufferSize) const {
    return impl->savePattern(jsonBuffer, jsonBufferSize);
}

bool DrumMachineDSP::loadPattern(const char *jsonData) {
    return impl->loadPattern(jsonData);
}

bool DrumMachineDSP::saveKit(char *jsonBuffer, int jsonBufferSize) const {
    return impl->saveKit(jsonBuffer, jsonBufferSize);
}

bool DrumMachineDSP::loadKit(const char *jsonData) {
    return impl->loadKit(jsonData);
}
