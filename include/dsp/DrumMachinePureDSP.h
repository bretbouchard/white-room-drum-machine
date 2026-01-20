/*
  ==============================================================================

    DrumMachinePureDSP.h
    Created: December 30, 2025
    Author: Bret Bouchard

    Pure DSP implementation of Drum Machine
    - Step sequencer with 16 tracks x 16 steps
    - Synthesized drum voices (kick, snare, hihat, clap, etc.)
    - Flam, roll, probability, and swing
    - Multiple drum kit types
    - Pattern chaining support
    - Factory-creatable for dynamic instantiation
    - Zero JUCE dependencies

  ==============================================================================
*/

#pragma once

#include "dsp/InstrumentDSP.h"
#include <vector>
#include <array>
#include <memory>
#include <cmath>
#include <functional>
#include <algorithm>
#include <string>

namespace DSP {

//==============================================================================
// Synthesized Drum Voices
//==============================================================================

// Kick Drum (sine wave + pitch envelope + transient) - Enhanced
struct KickVoice
{
    void prepare(double sampleRate);
    void reset();
    void trigger(float velocity);
    float processSample();
    bool isActive() const { return amplitude > 0.0001f; }

    void setPitch(float pitch);      // Base pitch
    void setDecay(float decay);      // Decay time
    void setClick(float click);      // Transient amount

private:
    double sampleRate = 48000.0;

    // Oscillator
    float phase = 0.0f;
    float frequency = 150.0f;

    // Pitch envelope
    float pitchEnvelope = 0.0f;
    float pitchDecay = 0.99f;
    float pitchAmount = 0.0f;

    // Amplitude envelope
    float amplitude = 0.0f;
    float decay = 0.995f;

    // Transient
    float transientPhase = 0.0f;
    float transientAmount = 0.3f;

    // Parameter smoothing (prevent zipper noise)
    float pitchSmoothing = 0.0f;
    float amplitudeSmoothing = 0.0f;
};

// Snare Drum (tuned noise + tone + snap) - Enhanced
struct SnareVoice
{
    void prepare(double sampleRate);
    void reset();
    void trigger(float velocity);
    float processSample();
    bool isActive() const { return toneAmplitude > 0.0001f || noiseAmplitude > 0.0001f || snapAmplitude > 0.0001f; }

    void setTone(float tone);        // Filter resonance
    void setDecay(float decay);      // Noise decay
    void setSnap(float snap);        // Transient snap

private:
    double sampleRate = 48000.0;

    // Tone (triangle wave)
    float tonePhase = 0.0f;
    float toneFreq = 180.0f;
    float toneAmplitude = 0.0f;
    float toneDecay = 0.99f;

    // Noise (filtered)
    float noiseAmplitude = 0.0f;
    float noiseDecay = 0.995f;

    // Filter state
    float filterState = 0.0f;
    float filterResonance = 0.7f;

    // Snap transient
    float snapAmplitude = 0.0f;
    float snapDecay = 0.9f;

    // Snare rattle (snares buzzing)
    float rattlePhase = 0.0f;

    // Parameter smoothing (prevent zipper noise)
    float filterSmoothing = 0.0f;
    float toneSmoothing = 0.0f;

    // PRNG state (deterministic)
    mutable unsigned noiseSeed = 42;
};

// Hi-Hat (high-pass filtered noise + metallic) - Enhanced
struct HiHatVoice
{
    void prepare(double sampleRate);
    void reset();
    void trigger(float velocity);
    float processSample();
    bool isActive() const { return amplitude > 0.0001f; }

    void setTone(float tone);        // High-pass frequency
    void setDecay(float decay);      // Decay time
    void setMetallic(float metallic); // Metallic overtones

private:
    double sampleRate = 48000.0;

    // Noise source
    float noisePhase = 0.0f;

    // Amplitude envelope
    float amplitude = 0.0f;
    float decay = 0.96f;

    // High-pass filter
    float filterState = 0.0f;
    float filterCoeff = 0.5f;

    // Metallic overtones (multiple FM oscillators)
    float metalPhase = 0.0f;
    float metalPhase2 = 0.0f;
    float metalPhase3 = 0.0f;
    float metalAmount = 0.1f;

    // Parameter smoothing (prevent zipper noise)
    float filterSmoothing = 0.0f;
    float amplitudeSmoothing = 0.0f;

    // PRNG state (deterministic)
    mutable unsigned noiseSeed = 43;
};

// Clap (filtered noise bursts) - Enhanced
struct ClapVoice
{
    void prepare(double sampleRate);
    void reset();
    void trigger(float velocity);
    float processSample();
    bool isActive() const { return amplitude > 0.0001f; }

    void setTone(float tone);        // Filter frequency
    void setDecay(float decay);      // Decay time
    void setNumImpulses(int num);    // Number of impulses

private:
    double sampleRate = 48000.0;

    // Multiple noise bursts
    float amplitude = 0.0f;
    float decay = 0.97f;
    int numImpulses = 3;
    int currentImpulse = 0;
    int impulseCounter = 0;
    int impulseSpacing = 500;

    // Filter
    float filterState = 0.0f;
    float filterCoeff = 0.6f;

    // Parameter smoothing (prevent zipper noise)
    float filterSmoothing = 0.0f;
    float amplitudeSmoothing = 0.0f;

    // PRNG state (deterministic)
    mutable unsigned noiseSeed = 44;
};

// Percussion (tom/cowbell type) - Enhanced
struct PercVoice
{
    void prepare(double sampleRate);
    void reset();
    void trigger(float velocity);
    float processSample();
    bool isActive() const { return amplitude > 0.0001f; }

    void setPitch(float pitch);
    void setDecay(float decay);
    void setTone(float tone);        // Tone vs noise mix

private:
    double sampleRate = 48000.0;

    // Tone (sine wave)
    float phase = 0.0f;
    float phase2 = 0.0f;  // Second oscillator for richer sound
    float frequency = 200.0f;

    // Amplitude
    float amplitude = 0.0f;
    float decay = 0.99f;

    // Tone/noise mix
    float toneMix = 0.7f;
    float noiseAmplitude = 0.0f;

    // Parameter smoothing (prevent zipper noise)
    float pitchSmoothing = 0.0f;
    float amplitudeSmoothing = 0.0f;

    // PRNG state (deterministic)
    mutable unsigned noiseSeed = 45;
};

// Cymbal (metallic noise with long decay) - Enhanced
struct CymbalVoice
{
    void prepare(double sampleRate);
    void reset();
    void trigger(float velocity);
    float processSample();
    bool isActive() const { return masterAmplitude > 0.0001f; }

    void setTone(float tone);        // Brightness
    void setDecay(float decay);      // Long decay
    void setMetallic(float metallic); // FM modulation depth

private:
    double sampleRate = 48000.0;

    // Multiple oscillators for metallic sound
    static constexpr int numOscillators = 6;
    float phases[numOscillators] = {0};
    float frequencies[numOscillators] = {0};
    float amplitudes[numOscillators] = {0};

    // Amplitude envelope
    float masterAmplitude = 0.0f;
    float decay = 0.999f;

    // Enhanced FM modulation (multiple oscillators)
    float fmDepth = 0.0f;
    float fmPhase = 0.0f;
    float fmPhase2 = 0.0f;  // Second FM oscillator for richer metallic sound

    // Parameter smoothing (prevent zipper noise)
    float amplitudeSmoothing = 0.0f;
};

//==============================================================================
// Step Sequencer
//==============================================================================

//==============================================================================
// Timing Role System
//==============================================================================

enum class TimingRole
{
    Pocket,   // steady / centered
    Push,     // slightly early
    Pull      // slightly late
};

struct RoleTimingParams
{
    float pocketOffset = 0.0f;   // usually 0.0
    float pushOffset = -0.04f;   // negative (early) - fraction of step
    float pullOffset = +0.06f;   // positive (late) - fraction of step
};

struct DillaState
{
    float drift = 0.0f;   // current accumulated offset (fraction of step)
};

struct DillaParams
{
    float amount = 0.6f;      // 0..1 overall strength
    float hatBias = 0.55f;    // 0=pull, 1=push for hats
    float snareLate = 0.8f;   // 0..1 how late snares lean
    float kickTight = 0.7f;   // 0..1 how stable kicks are
    float maxDrift = 0.15f;   // clamp, fraction of step
};

//==============================================================================
// Drill Mode (Aphex Twin / Drill'n'Bass)
//==============================================================================

// Deterministic RNG for drill mode
struct DeterministicRng
{
    uint32_t s = 0x12345678u;
    explicit DeterministicRng(uint32_t seed = 0x12345678u) : s(seed ? seed : 0x12345678u) {}

    uint32_t nextU32()
    {
        // xorshift32
        uint32_t x = s;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        s = x;
        return x;
    }

    float next01()
    {
        // [0,1)
        return (nextU32() >> 8) * (1.0f / 16777216.0f);
    }

    float nextSigned()
    {
        return (next01() * 2.0f) - 1.0f; // [-1,1)
    }

    int rangeInt(int lo, int hiInclusive)
    {
        if (hiInclusive <= lo) return lo;
        const uint32_t span = (uint32_t)(hiInclusive - lo + 1);
        return lo + (int)(nextU32() % span);
    }
};

// Drill grid subdivision types
enum class DrillGrid : uint8_t
{
    Straight,        // equal spacing across burst
    Triplet,         // 3 grid
    Quintuplet,      // 5 grid
    Septuplet,       // 7 grid
    RandomPrime      // chooses 5/7/11 per burst
};

// Drill mode parameters
struct DrillMode
{
    bool enabled = false;

    // Macro controls
    float amount = 0.0f;        // 0..1 overall intensity (scales everything)
    float mutationRate = 0.0f;  // 0..1 chance to mutate burst per hit
    float dropout = 0.0f;       // 0..1 chance to skip a micro-hit
    float chaos = 0.0f;         // 0..1 timing chaos inside burst
    float spread = 0.35f;       // 0..1 how much of step duration the burst spans
    float velDecay = 0.35f;     // 0..1 exponential-ish decay per micro hit
    float accentFlip = 0.0f;    // 0..1 random accent inversions
    float temporalAggression = 1.0f; // 0..1 scales burstCount, chaos, mutation, grid randomness

    // Burst sizing
    int minBurst = 1;
    int maxBurst = 8;

    // Grid
    DrillGrid grid = DrillGrid::Straight;

    // Transition smoothing (for groove<->drill crossfade)
    float transitionBeats = 0.5f; // how many beats to ramp into/out of drill
};

//==============================================================================
// Drill Intensity Automation (Compositional Sequencing)
//==============================================================================

// Automation point for drill amount over time
struct DrillAutomationPoint
{
    int bar = 0;         // Bar index (0-based)
    float amount = 0.0f; // Drill amount 0..1
};

// Automation lane for drill intensity as composition
struct DrillAutomationLane
{
    std::vector<DrillAutomationPoint> points; // Sorted by bar

    // Evaluate drill amount at given bar (step function, no interpolation yet)
    float evaluateAt(int bar) const
    {
        if (points.empty())
            return 0.0f;

        float lastAmount = 0.0f;
        for (const auto& p : points)
        {
            if (p.bar > bar)
                break;
            lastAmount = p.amount;
        }
        return lastAmount;
    }

    // Add point (keeps sorted)
    void addPoint(int bar, float amount)
    {
        DrillAutomationPoint p{bar, std::max(0.0f, std::min(1.0f, amount))};
        points.push_back(p);
        std::sort(points.begin(), points.end(),
                 [](const DrillAutomationPoint& a, const DrillAutomationPoint& b)
                 { return a.bar < b.bar; });
    }

    // Clear all automation
    void clear() { points.clear(); }
};

//==============================================================================
// Automatic Drill Fills (Context-Sensitive)
//==============================================================================

// Policy for automatic drill fills at bar ends
struct DrillFillPolicy
{
    bool enabled = false;

    int fillLengthSteps = 2;     // How many steps at bar end get fill
    float triggerChance = 0.7f;  // Probability per bar (0..1)
    float fillAmount = 0.8f;     // Drill amount during fill
    float decayPerStep = 0.15f;  // Linear decay across fill (0..1)
};

// Runtime state for drill fills
struct DrillFillState
{
    bool active = false;  // Is fill active this bar?
};

//==============================================================================
// Drill ↔ Silence Gating (Extreme IDM)
//==============================================================================

// Policy for drill/silence gating (broken transport effect)
struct DrillGatePolicy
{
    bool enabled = false;

    float silenceChance = 0.25f;  // Chance to start silent run (0..1)
    float burstChance = 0.5f;     // Chance silence becomes burst (0..1)
    int minSilentSteps = 1;       // Minimum steps in silent run
    int maxSilentSteps = 3;       // Maximum steps in silent run
};

// Runtime state for drill gate
struct DrillGateState
{
    int silentStepsRemaining = 0;  // Steps left in current silent run
};

//==============================================================================
// IDM Macro Presets (Behavioral Identities)
//==============================================================================

// Complete IDM behavior: drill + fills + gates as one identity
// These are "what the machine feels like", not individual parameters
struct IdmMacroPreset
{
    const char* name;              // Human-readable name
    DrillMode drill;               // Core drill behavior
    DrillFillPolicy fill;          // Automatic fill policy
    DrillGatePolicy gate;          // Silence gating policy

    // Helper to apply all components at once
    void applyTo(DrillMode& d, DrillFillPolicy& f, DrillGatePolicy& g) const
    {
        d = drill;
        f = fill;
        g = gate;
    }
};



//==============================================================================
// Bar-Aware Phrase Detection (Musical Intelligence)
//==============================================================================

// Phrase detector for 4/8/16 bar musical intelligence
struct PhraseDetector
{
    int barsPerPhrase = 4;  // 4, 8, or 16 bar phrases

    bool isPhraseEnd(int barIndex) const
    {
        return ((barIndex + 1) % barsPerPhrase) == 0;
    }

    bool isPhraseStart(int barIndex) const
    {
        return (barIndex % barsPerPhrase) == 0;
    }

    bool isEventBar(int barIndex, int eventInterval = 8) const
    {
        // For 8-bar "event bars" (big moments)
        return ((barIndex + 1) % eventInterval) == 0;
    }
};

// Safety: Maximum micro-hits per audio block (prevents audio thread DOS)
constexpr int kMaxMicroHitsPerBlock = 256;

// Rhythm feel mode (groove vs drill)
enum class RhythmFeelMode : uint8_t
{
    Groove,   // swing + pocket/push/pull + (optional dilla)
    Drill     // micro-bursts, overrides groove timing on burst hits
};

// Per-track drill override
struct TrackDrillOverride
{
    bool useOverride = false;
    DrillMode drill;
};

//==============================================================================
// Step Sequencer
//==============================================================================

//==============================================================================
// Drill-Aware Pattern Generation
//==============================================================================

// Drill intent: semantic tagging for where drill is musically appropriate
enum class DrillIntent : uint8_t
{
    None,        // Never drill - always use groove timing
    Optional,    // May drill if mode/amount > threshold (25%)
    Emphasize    // Prefer drill here (fills, accents) - lower threshold (5%)
};

struct StepCell
{
    bool active = false;
    uint8_t velocity = 100;
    float probability = 1.0f;
    bool hasFlam = false;
    bool isRoll = false;
    int rollNotes = 4;
    float timingOffset = 0.0f;  // accumulates swing + role + Dilla drift

    // Drill mode parameters
    bool useDrill = false;      // Enable drill for this step
    int burstCount = 1;         // Number of micro-hits (1-16)
    float burstChaos = 0.0f;    // Extra timing randomness (0-1)
    float burstDropout = 0.0f;  // Chance to skip micro-hits (0-1)

    // Drill-aware pattern generation
    DrillIntent drillIntent = DrillIntent::Optional;  // Semantic intent
};

struct Track
{
    enum class DrumType
    {
        Kick,
        Snare,
        HiHatClosed,
        HiHatOpen,
        Clap,
        TomLow,
        TomMid,
        TomHigh,
        Crash,
        Ride,
        Cowbell,
        Shaker,
        Tambourine,
        Percussion,
        Special
    };

    DrumType type = DrumType::Kick;
    TimingRole timingRole = TimingRole::Pocket;  // Pocket/Push/Pull
    std::array<StepCell, 16> steps{};
    float volume = 0.8f;
    float pan = 0.0f;
    int pitch = 0;  // MIDI pitch offset

    // Per-track drill override
    TrackDrillOverride drillOverride;
};

class StepSequencer
{
public:
    StepSequencer();
    ~StepSequencer() = default;

    void prepare(double sampleRate, int samplesPerBlock);
    void reset();

    void setTempo(float bpm);
    void setSwing(float swingAmount);  // 0.0 to 1.0

    void setPatternLength(int length);
    int getCurrentStep() const { return currentStep_; }

    void triggerTrack(int trackIndex, int stepIndex, float velocity);
    void triggerAllTracks(int stepIndex);

    bool isTrackTriggered(int trackIndex, int stepIndex) const;

    void advance(int numSamples);
    void processTrack(int trackIndex, float* output, int numSamples);

    void setTrack(int index, const Track& track);
    Track getTrack(int index) const;

    int getNumTracks() const { return static_cast<int>(tracks_.size()); }
    bool hasActiveVoices() const;  // Check if any drum voice is playing

    // Timing role system
    void setRoleTimingParams(const RoleTimingParams& params) { roleTimingParams_ = params; }
    RoleTimingParams getRoleTimingParams() const { return roleTimingParams_; }

    void setDillaParams(const DillaParams& params) { dillaParams_ = params; }
    DillaParams getDillaParams() const { return dillaParams_; }

    // Drill mode system
    void setDrillMode(const DrillMode& drill) { drillMode_ = drill; }
    DrillMode getDrillMode() const { return drillMode_; }

    void setRhythmFeelMode(RhythmFeelMode mode) { rhythmFeelMode_ = mode; }
    RhythmFeelMode getRhythmFeelMode() const { return rhythmFeelMode_; }

    // Drill intensity automation (compositional sequencing)
    void setDrillAutomation(const DrillAutomationLane& lane) { drillAutomation_ = lane; }
    DrillAutomationLane getDrillAutomation() const { return drillAutomation_; }
    void addDrillAutomationPoint(int bar, float amount) { drillAutomation_.addPoint(bar, amount); }
    void clearDrillAutomation() { drillAutomation_.clear(); }

    // Automatic drill fills
    void setDrillFillPolicy(const DrillFillPolicy& policy) { drillFillPolicy_ = policy; }
    DrillFillPolicy getDrillFillPolicy() const { return drillFillPolicy_; }

    // Drill ↔ Silence gating
    void setDrillGatePolicy(const DrillGatePolicy& policy) { drillGatePolicy_ = policy; }
    DrillGatePolicy getDrillGatePolicy() const { return drillGatePolicy_; }

    // Musical phrase intelligence
    void setPhraseDetector(const PhraseDetector& p) { phraseDetector_ = p; }
    PhraseDetector getPhraseDetector() const { return phraseDetector_; }
    int getBarsPerPhrase() const { return phraseDetector_.barsPerPhrase; }
    void setBarsPerPhrase(int bars) { phraseDetector_.barsPerPhrase = bars; }

    // IDM Macro Presets (behavioral identities)
    void applyIdmMacroPreset(const IdmMacroPreset& preset)
    {
        preset.applyTo(drillMode_, drillFillPolicy_, drillGatePolicy_);
    }

    // IDM macro preset loaders (complete behavioral identities)
    static IdmMacroPreset idmMacroGhostFill();
    static IdmMacroPreset idmMacroSnareHallucination();
    static IdmMacroPreset idmMacroBrokenTransport();
    static IdmMacroPreset idmMacroVenetianCollapse();
    static IdmMacroPreset idmMacroAntiGroove();

    // Preset loaders (individual drill modes)
    static DrillMode presetDrillLite();
    static DrillMode presetAphexSnareHell();
    static DrillMode presetVenetianMode();

    // A) Transitional / Musical (Groove ↔ Drill bridges)
    static DrillMode presetGlitchAccent();
    static DrillMode presetBrokenGroove();
    static DrillMode presetNeoIDMFill();
    static DrillMode presetGhostMachinery();

    // B) Aphex-Style Signature Presets
    static DrillMode presetAphexMicrofracture();
    static DrillMode presetWindowlickerSnare();
    static DrillMode presetPolygonWindow();
    static DrillMode presetClockDesync();

    // C) Drill'n'Bass / Venetian Snares Energy
    static DrillMode presetDrillNBassCore();
    static DrillMode presetVenetianGhosts();
    static DrillMode presetAmenShredder();
    static DrillMode presetOverclockedSnare();

    // D) Noise / Experimental / Brutal
    static DrillMode presetTimeGrinder();
    static DrillMode presetDigitalSeizure();
    static DrillMode presetStaticEngine();

    // E) Rhythmic Control / Utility
    static DrillMode presetRatchetBuilder();
    static DrillMode presetFillGenerator();

private:
    double sampleRate_ = 48000.0;
    float samplesPerBeat_ = 0.0f;
    float samplesPerStep_ = 0.0f;
    double position_ = 0.0;
    int currentStep_ = 0;
    int patternLength_ = 16;

    float swingAmount_ = 0.0f;
    float tempo_ = 120.0f;

    // Timing system
    RoleTimingParams roleTimingParams_;
    DillaParams dillaParams_;
    std::array<DillaState, 16> dillaStates_;  // One per track

    // Drill mode system
    DrillMode drillMode_;
    RhythmFeelMode rhythmFeelMode_ = RhythmFeelMode::Groove;
    DeterministicRng drillRng_;  // RNG for drill mode
    int microHitsThisBlock_ = 0;  // Safety counter for audio thread protection

    // Drill intensity automation (compositional sequencing)
    DrillAutomationLane drillAutomation_;
    int currentBar_ = 0;  // Track current bar for automation

    // Automatic drill fills (context-sensitive)
    DrillFillPolicy drillFillPolicy_;
    DrillFillState drillFillState_;

    // Drill ↔ Silence gating (extreme IDM)
    DrillGatePolicy drillGatePolicy_;
    DrillGateState drillGateState_;

    // Musical phrase intelligence
    PhraseDetector phraseDetector_;

    std::array<Track, 16> tracks_;

    // Drum voices (one per track type)
    KickVoice kick_;
    SnareVoice snare_;
    HiHatVoice hihatClosed_;
    HiHatVoice hihatOpen_;
    ClapVoice clap_;
    PercVoice tomLow_;
    PercVoice tomMid_;
    PercVoice tomHigh_;
    CymbalVoice crash_;
    CymbalVoice ride_;
    PercVoice cowbell_;
    HiHatVoice shaker_;
    HiHatVoice tambourine_;
    PercVoice percussion_;
    SnareVoice special_;

    // PRNG state for probability checks (deterministic)
    mutable unsigned probSeed = 123;

    float processDrumVoice(Track::DrumType type, float velocity);
    void advanceStep();

    // Timing system helpers
    void updateDillaDrift(int trackIndex, TimingRole role);
    void applyTimingLayers(int trackIndex, int stepIndex);
    float getSwingOffset(int stepIndex) const;

    // Drill mode helpers
    bool trackWantsDrill(Track::DrumType type) const;
    bool cellWantsDrill(const StepCell& cell, const DrillMode& drill, float globalDrillAmount) const;
    void scheduleMicroBurst(int trackIndex, const StepCell& cell,
                           double stepStartSeconds, double stepDurationSeconds,
                           float effectiveDrillAmount = -1.0f); // -1 means use drillMode_.amount
    int chooseGridDivisor(DrillGrid grid);

    // Drill fill helpers
    bool isFillStep(int stepIndex, int stepsPerBar, const DrillFillPolicy& policy) const;
    void updateFillState(const DrillFillPolicy& policy);

    // Drill gate helpers
    bool shouldGateStep(const DrillGatePolicy& policy);

    // Bar tracking for automation
    void updateBarIndex();
    int getStepsPerBar() const { return 16; } // 16 steps = 4 beats at 16th note resolution
};

//==============================================================================
// Main Drum Machine Instrument
//==============================================================================

//==============================================================================
// Preset System
//==============================================================================

enum PresetSection : int
{
    PRESET_GLOBAL = 1 << 0,     // Global parameters
    PRESET_PATTERN = 1 << 1,    // Pattern data (rhythms)
    PRESET_KIT = 1 << 2,        // Drum kit (voice parameters)
    PRESET_ALL = PRESET_GLOBAL | PRESET_PATTERN | PRESET_KIT
};

// Voice parameters for saving kit presets
struct VoiceParams
{
    // Kick
    float kickPitch = 0.5f;
    float kickDecay = 0.5f;
    float kickClick = 0.3f;

    // Snare
    float snareTone = 0.7f;
    float snareDecay = 0.5f;
    float snareSnap = 0.5f;

    // HiHat Closed
    float hihatClosedTone = 0.5f;
    float hihatClosedDecay = 0.3f;
    float hihatClosedMetallic = 0.1f;

    // HiHat Open
    float hihatOpenTone = 0.5f;
    float hihatOpenDecay = 0.7f;
    float hihatOpenMetallic = 0.1f;

    // Clap
    float clapTone = 0.6f;
    float clapDecay = 0.5f;
    int clapNumImpulses = 3;

    // Tom Low
    float tomLowPitch = 0.2f;
    float tomLowDecay = 0.6f;
    float tomLowTone = 0.7f;

    // Tom Mid
    float tomMidPitch = 0.5f;
    float tomMidDecay = 0.6f;
    float tomMidTone = 0.7f;

    // Tom High
    float tomHighPitch = 0.8f;
    float tomHighDecay = 0.6f;
    float tomHighTone = 0.7f;

    // Crash
    float crashTone = 0.5f;
    float crashDecay = 0.8f;
    float crashMetallic = 0.1f;

    // Ride
    float rideTone = 0.5f;
    float rideDecay = 0.9f;
    float rideMetallic = 0.1f;

    // Cowbell
    float cowbellPitch = 0.5f;
    float cowbellDecay = 0.7f;
    float cowbellTone = 0.7f;

    // Shaker
    float shakerTone = 0.5f;
    float shakerDecay = 0.4f;
    float shakerMetallic = 0.05f;

    // Tambourine
    float tambourineTone = 0.5f;
    float tambourineDecay = 0.5f;
    float tambourineMetallic = 0.05f;

    // Percussion
    float percussionPitch = 0.5f;
    float percussionDecay = 0.5f;
    float percussionTone = 0.7f;

    // Special
    float specialTone = 0.7f;
    float specialDecay = 0.5f;
    float specialSnap = 0.5f;
};

//==============================================================================
// Main Drum Machine Instrument
//==============================================================================

class DrumMachinePureDSP : public InstrumentDSP
{
public:
    DrumMachinePureDSP();
    ~DrumMachinePureDSP() override;

    bool prepare(double sampleRate, int blockSize) override;
    void reset() override;
    void process(float** outputs, int numChannels, int numSamples) override;
    void handleEvent(const ScheduledEvent& event) override;

    float getParameter(const char* paramId) const override;
    void setParameter(const char* paramId, float value) override;

    // Base class interface implementations (call enhanced versions)
    bool savePreset(char* jsonBuffer, int jsonBufferSize) const override;
    bool loadPreset(const char* jsonData) override;

    // Enhanced preset system with section-based save/load
    bool savePresetEx(char* jsonBuffer, int jsonBufferSize, int sections) const;
    bool loadPresetEx(const char* jsonData, int sections);

    // Convenience methods for pattern-only save/load
    bool savePattern(char* jsonBuffer, int jsonBufferSize) const;
    bool loadPattern(const char* jsonData);

    // Convenience methods for kit-only save/load
    bool saveKit(char* jsonBuffer, int jsonBufferSize) const;
    bool loadKit(const char* jsonData);

    int getActiveVoiceCount() const override;
    int getMaxPolyphony() const override { return 16; }  // 16 tracks

    const char* getInstrumentName() const override { return "DrumMachine"; }
    const char* getInstrumentVersion() const override { return "1.0.0"; }

private:
    StepSequencer sequencer_;

    struct Parameters
    {
        float tempo = 120.0f;
        float swing = 0.0f;
        float masterVolume = 0.8f;
        float patternLength = 16.0f;

        // Role timing parameters
        float pocketOffset = 0.0f;
        float pushOffset = -0.04f;
        float pullOffset = +0.06f;

        // Dilla parameters
        float dillaAmount = 0.6f;
        float dillaHatBias = 0.55f;
        float dillaSnareLate = 0.8f;
        float dillaKickTight = 0.7f;
        float dillaMaxDrift = 0.15f;

        // Structure (Mutable Instruments-style harmonic complexity)
        // 0.0 = simple, straight beats (minimal variation, clean patterns)
        // 0.5 = balanced (default)
        // 1.0 = complex, intricate (voice mixing, timing complexity, effects depth)
        float structure = 0.5f;

        // Stereo Enhancement
        float stereoWidth = 0.5f;     // 0=mono, 1=full stereo
        float roomWidth = 0.3f;       // Room reverb stereo width
        float effectsWidth = 0.7f;    // Effects returns stereo width

        // Per-track volumes
        float trackVolumes[16] = {0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f,
                                  0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f};
    } params_;

    VoiceParams voiceParams_;  // Drum voice parameters for kit presets

    double sampleRate_ = 48000.0;
    int blockSize_ = 512;

    // JSON helper methods
    bool writeJsonParameter(const char* name, double value, char* buffer,
                            int& offset, int bufferSize) const;
    bool writeJsonString(const char* name, const char* value, char* buffer,
                         int& offset, int bufferSize) const;
    bool parseJsonParameter(const char* json, const char* param, double& value) const;
    bool parseJsonString(const char* json, const char* param, char* value, int valueSize) const;

    // Voice parameter synchronization
    void syncVoiceParamsFromDSP();
    void syncVoiceParamsToDSP();

    std::array<bool, 16> activeVoices_{};  // Track MIDI-triggered voices
};

//==============================================================================
// Inline Helper Functions
//==============================================================================

inline float lerp(float a, float b, float t) { return a + t * (b - a); }
inline float clamp(float x, float min, float max) { return (x < min) ? min : (x > max) ? max : x; }
inline float randomFloat() { return static_cast<float>(rand()) / static_cast<float>(RAND_MAX); }

} // namespace DSP
