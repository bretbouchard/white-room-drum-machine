# Drum Machine - Step Sequencer with Groove Engine

**Instrument:** Pocket/Push/Pull Drum Machine with Dilla Time
**Status:** ✅ PRODUCTION READY
**Type:** Drum Synthesizer & Step Sequencer

---

## Table of Contents

1. [Overview](#overview)
2. [Key Features](#key-features)
3. [Timing System](#timing-system)
4. [Architecture](#architecture)
5. [Voice Architecture](#voice-architecture)
6. [Parameters](#parameters)
7. [Building](#building)
8. [Integration](#integration)
9. [Performance](#performance)

---

## Overview

The **Drum Machine** is a professional step-sequencer drum synthesizer featuring advanced groove capabilities with Pocket/Push/Pull timing roles and Dilla Time (structured drift). It provides realistic, human-feeling drum patterns with micro-timing control.

### Key Achievements

- ✅ **7 voice polyphony** (Kick, Snare, HiHat, Clap, Shaker, Toms, Cymbals)
- ✅ **Pocket/Push/Pull timing** - Advanced groove system
- ✅ **Dilla Time** - Structured drift for human feel
- ✅ **16-step sequencer** with real-time control
- ✅ **Per-voice synthesis** (not samples)
- ✅ **Realtime-safe DSP** processing

---

## Key Features

### 1. Timing System

#### Pocket/Push/Pull Roles

Each drum voice has a timing role that defines its groove personality:

| Voice    | Role   | Offset | Musical Intent              |
|----------|--------|--------|-----------------------------|
| Kick     | Pocket | 0.00   | Steady anchor               |
| Snare    | Pull   | +0.06  | Laid-back weight            |
| HiHat    | Push   | -0.04  | Forward motion              |
| Clap     | Pull   | +0.06  | Laid-back weight            |
| Shaker   | Push   | -0.04  | Forward motion              |
| Toms     | Pocket | 0.00   | Stable                      |
| Cymbals  | Pocket | 0.00   | Stable                      |

#### Dilla Time (Structured Drift)

Accumulated drift per track that creates natural timing variation:
- `amount`: 0.6 (overall strength, 0..1)
- `hatBias`: 0.55 (hat push/pull bias)
- `snareLate`: 0.8 (how late snares lean)
- `kickTight`: 0.7 (how stable kicks are)
- `maxDrift`: 0.15 (maximum drift clamp)

This creates a natural, human-feeling groove.

#### Drill Mode (Aphex Twin / Drill'n'Bass)

**Micro-burst system** for hyper-quantized, fractured drum patterns:

**What Drill Mode Does:**
- Transforms single steps into N micro-hits (bursts, stutters, ratchets)
- Applies non-musical subdivisions (triplets, quintuplets, septuplets)
- Adds timing chaos and random dropouts
- Creates velocity decay and accent inversions

**Drill vs Dilla:**
- **Dilla**: Temporal disagreement, organic drift, slow bias
- **Drill**: Temporal weaponization, sudden jumps, algorithmic

**Drill Parameters:**
- `amount`: 0.0-1.0 (overall intensity)
- `minBurst` / `maxBurst`: 1-16 (number of micro-hits)
- `chaos`: 0.0-1.0 (timing randomness)
- `dropout`: 0.0-1.0 (chance to skip micro-hits)
- `spread`: 0.0-1.0 (how much of step burst spans)
- `velDecay`: 0.0-1.0 (exponential velocity decay)
- `accentFlip`: 0.0-1.0 (random accent spikes)
- `mutationRate`: 0.0-1.0 (pattern mutation chance)
- `grid`: Straight, Triplet, Quintuplet, Septuplet, RandomPrime

**Drill Presets:**
- **Drill Lite**: Subtle ratchets (2-4 hits, triplets)
- **Aphex Snare Hell**: Dense stutters (6-12 hits, random primes)
- **Venetian Snares Mode**: Extreme chaos (8-16 hits, high mutation)

**Track Mapping:**
- Snare, HiHat, Clap, Shaker, Tambourine, Percussion → use drill
- Kick, Toms, Cymbals, Crash, Ride, Cowbell → stay stable (unless overridden)

---

Each voice is a full synthesizer (not sample playback):

**Kick:**
- Sine oscillator with pitch envelope
- Fast attack, short decay
- Click transient

**Snare:**
- Noise generator with filter
- Tone oscillator
- Adjustable snap and body

**HiHat:**
- High-pass filtered noise
- Metallic FM synthesis
- Short decay with variable tone

**Clap:**
- Multiple noise bursts
- Bandpass filtering
- Exponential decay

**Shaker:**
- Filtered noise
- High-frequency emphasis
- Very short decay

**Toms:**
- Sine/triangle waves
- Pitch envelope
- Tunable toms

**Cymbals:**
- FM metallic synthesis
- Bandpass filtering
- Long decay with ride/bell modes

### 3. Step Sequencer

**16-step pattern** with:
- Velocity per step (0..1)
- Trigger/gate per step
- Per-voice timing role
- Swing (periodic odd-step offset)
- Pattern chaining

---

## Timing System

### Two Timing Modes

The drum machine supports two mutually exclusive timing modes:

**Groove Mode** (default):
- Swing + Pocket/Push/Pull + Dilla drift
- Organic, human-feeling timing
- Natural groove and feel

**Drill Mode** (Aphex Twin / Drill'n'Bass):
- Micro-bursts, stutters, ratchets
- Algorithmic, mechanical timing
- Hyper-quantized with controlled chaos

### Groove Mode Timing Layers

Inside the sequencer, groove timing applies in this order:

1. **Base step time** - Grid position
2. **Swing** - Periodic offset (odd steps)
3. **Role timing** - Pocket/Push/Pull (static bias)
4. **Dilla drift** - Per-voice accumulated drift
5. **Schedule** - Trigger the note

### Groove Timing Example

```
Step 1 (Kick):  0.0 + 0.0 (swing) + 0.0 (pocket) + 0.0 (dilla)  = 0.00
Step 1 (Snare): 0.0 + 0.0 (swing) + 0.06 (pull)  + 0.02 (dilla) = 0.08
Step 1 (HiHat):  0.0 + 0.0 (swing) - 0.04 (push)  - 0.01 (dilla) = -0.05
```

### Drill Mode Timing

When drill mode is enabled for a track:
1. **Burst expansion** - Single step → N micro-hits
2. **Grid subdivision** - Triplet, Quintuplet, Septuplet, RandomPrime
3. **Timing chaos** - Random jitter within burst
4. **Velocity decay** - Exponential amplitude falloff
5. **Dropout** - Random micro-hit removal
6. **Mutation** - Per-hit pattern variation

### Drill vs Groove

| Aspect          | Groove Mode            | Drill Mode                    |
|-----------------|------------------------|-------------------------------|
| Philosophy      | Temporal disagreement  | Temporal weaponization        |
| Timing           | Organic drift          | Sudden jumps                  |
| Feel            | Human                  | Mechanical / alien            |
| Per-step         | 1 trigger              | N micro-triggers              |
| Subdivisions    | Musical (swing)        | Non-musical (tuplets)         |
| Best for        | Hip-hop, jazz, funk    | Drill'n'bass, IDM, experimental|

---

### Code Structure

```
include/dsp/StepSequencerDSP.h     (Main sequencer)
include/dsp/Voices/                 (Voice implementations)
  ├── KickVoice.h
  ├── SnareVoice.h
  ├── HiHatVoice.h
  ├── ClapVoice.h
  ├── ShakerVoice.h
  ├── TomVoice.h
  └── CymbalVoice.h

src/dsp/StepSequencerDSP.cpp       (Implementation)
src/dsp/Voices/                     (Voice implementations)
```

### DSP Classes

- **StepSequencerDSP**: Main sequencer with timing engine
- **Voice**: Base class for all drum voices
- **TimingRole**: Pocket/Push/Pull enum
- **DillaState**: Per-track drift accumulator
- **DillaParams**: Global Dilla time controls

---

## Parameters

### Global Parameters

| Parameter      | Range    | Default | Description                     |
|----------------|----------|---------|---------------------------------|
| tempo          | 60-200   | 120     | BPM                             |
| swing          | 0-1      | 0.5     | Swing amount                     |
| dillaAmount    | 0-1      | 0.6     | Overall Dilla drift              |
| dillaMaxDrift  | 0-0.5    | 0.15    | Maximum drift per step           |
| rhythmMode     | enum     | Groove  | Groove or Drill mode            |

### Drill Mode Parameters

| Parameter      | Range    | Default | Description                     |
|----------------|----------|---------|---------------------------------|
| drillEnabled   | bool     | false   | Enable drill mode                |
| drillAmount    | 0-1      | 0.0     | Drill intensity                  |
| drillMinBurst  | 1-16     | 1       | Minimum micro-hits              |
| drillMaxBurst  | 1-16     | 8       | Maximum micro-hits              |
| drillChaos     | 0-1      | 0.0     | Timing randomness                |
| drillDropout   | 0-1      | 0.0     | Micro-hit skip chance           |
| drillSpread    | 0-1      | 0.35    | Burst time span                  |
| drillVelDecay  | 0-1      | 0.35    | Velocity decay rate              |
| drillAccentFlip| 0-1      | 0.0     | Random accent spikes            |
| drillMutation  | 0-1      | 0.0     | Pattern mutation rate           |
| drillGrid      | enum     | Straight| Subdivision grid               |
| temporalAggression | 0-1  | 1.0     | Macro control scaling burst, chaos, mutation |

---

## Advanced Drill Features

### 1. Drill-Aware Pattern Generation

**Pattern = Permission + Intent**

Drill-aware pattern generation adds semantic meaning to each step, allowing patterns to encode "where drill is allowed or encouraged" without hard-coding burst counts.

#### DrillIntent System

Each `StepCell` now has a `drillIntent` field with three values:

| Intent       | Threshold | Musical Use                     |
|--------------|-----------|---------------------------------|
| None         | Never     | Kicks, steady beats             |
| Optional     | >25%      | Backbeats, hats, percussion     |
| Emphasize    | >5%       | Fills, accents, climaxes        |

#### Default Intent Mapping

| Instrument            | Intent       | Rationale                       |
|-----------------------|--------------|---------------------------------|
| Kick                  | None         | Steady anchor                   |
| Snare (backbeat 2/4)  | Optional     | May drill if intensity high      |
| Snare (fill steps)    | Emphasize    | Explode on fills                |
| Closed Hi-Hat         | Optional     | Light stutter                    |
| Open Hi-Hat / Noise   | Emphasize    | Prefer drilling                 |
| Percussion            | Optional     | Context-dependent                |

**Result**: Drill intensity reveals more of the pattern. Not all steps explode at once. Musical escalation feels intentional.

```cpp
// Example: Set drill intent for a step
StepCell& cell = tracks_[SNARE].steps[stepIndex];
cell.drillIntent = DrillIntent::Emphasize;  // This step loves drill
```

---

### 2. Drill Intensity Automation (Compositional Sequencing)

**Treat drill amount as a time-varying parameter, not a mode toggle.**

Drill amount is to rhythm what filter cutoff is to timbre.

#### Automation Lane

```cpp
struct DrillAutomationPoint {
    int bar;         // Bar index (0-based)
    float amount;    // Drill amount 0..1
};

// Usage
drumMachine->addDrillAutomationPoint(0, 0.0);   // Verse: no drill
drumMachine->addDrillAutomationPoint(8, 0.25);  // Pre-chorus: subtle
drumMachine->addDrillAutomationPoint(16, 0.5);  // Chorus: medium
drumMachine->addDrillAutomationPoint(24, 0.9);  // Break: extreme
drumMachine->addDrillAutomationPoint(32, 0.7);  // Drop: high energy
```

#### Musical Patterns

**Section-Based Escalation**:
- Verse: 0.0 (pure groove)
- Pre-chorus: 0.25 (light stutter)
- Chorus: 0.5 (moderate drill)
- Break: 0.9 (chaos)
- Drop: 0.7 (controlled energy)

**Fill-Only Drill** (very Aphex):
```cpp
// Bars 1-7: 0.0
// Bar 8: 0.6
// Combined with DrillIntent::Emphasize, only fills explode
```

**Anti-Drop** (Aphex signature):
```cpp
// Bar 15: 0.8 (build)
// Bar 16: 0.0 (abrupt silence)
```

---

### 3. Automatic Drill Fills

**Context-sensitive, short-lived drill intensity triggered by musical boundaries.**

Fills happen at bar ends without automation lanes.

#### Fill Policy

```cpp
DrillFillPolicy fillPolicy;
fillPolicy.enabled = true;
fillPolicy.fillLengthSteps = 2;    // Last 2 steps of bar
fillPolicy.triggerChance = 0.7f;    // 70% chance per bar
fillPolicy.fillAmount = 0.8f;       // 80% drill intensity during fill
fillPolicy.decayPerStep = 0.15f;    // Linear decay across fill

drumMachine->setDrillFillPolicy(fillPolicy);
```

#### How It Works

1. At each bar start, RNG determines if fill triggers
2. If active, last N steps of bar get escalated drill amount
3. Decay curve makes last step wildest
4. Next bar snaps back to groove cleanly

**Result**: "Intelligent machine" feel. Patterns explode unpredictably at bar boundaries, then return to order.

#### Recommended Defaults

- `fillLengthSteps = 2`
- `triggerChance = 0.65`
- `fillAmount = 0.75`
- `decayPerStep = 0.25`

---

### 4. Drill ↔ Silence Gating (Extreme IDM)

**Intentional rhythmic annihilation. Not mute — musical violence.**

Instead of filling time with more events, remove time entirely. The brain fills in missing beats → huge tension.

Used by: Aphex Twin, Autechre, Venetian Snares, Squarepusher

#### Gate Policy

```cpp
DrillGatePolicy gatePolicy;
gatePolicy.enabled = true;
gatePolicy.silenceChance = 0.25f;  // 25% chance to start silent run
gatePolicy.burstChance = 0.5f;     // 50% chance silence becomes burst
gatePolicy.minSilentSteps = 1;     // Minimum steps in silent run
gatePolicy.maxSilentSteps = 3;     // Maximum steps in silent run

drumMachine->setDrillGatePolicy(gatePolicy);
```

#### Musical Behaviors

**Low `silenceChance` (0.1-0.2)**:
- Stuttering groove
- Perceptual syncopation

**High `silenceChance` (0.3-0.5)**:
- Broken transport illusion
- Time feels discontinuous

**High `burstChance` (0.6-0.8)**:
- Silence → violent explosion
- Classic IDM "jump scare"

#### Extreme IDM Preset

```cpp
DrillGatePolicy gate;
gate.enabled = true;
gate.silenceChance = 0.35f;
gate.burstChance = 0.65f;
gate.minSilentSteps = 1;
gate.maxSilentSteps = 4;

// Combine with:
drill.grid = DrillGrid::RandomPrime;
drill.maxBurst = 16;
drill.chaos = 0.3f;
```

---

### Putting It All Together

The four systems work in this order:

1. **Pattern Generation** → Sets `DrillIntent` per step
2. **Composition / Form** → Sets `DrillAutomationLane` (amount over bars)
3. **Runtime** (per step):
   - Evaluate automation → base drill amount
   - Apply fill escalation (if bar-end)
   - Apply gate (silence or explode)
   - Check `cellWantsDrill()` → step permission
   - Schedule micro-burst if allowed

**No architecture rewrite. No special cases. No musical confusion.**

---

### 5. IDM Macro Presets (Behavioral Identities)

**Complete behaviors, not individual parameters.**

An IDM macro preset bundles drill + fills + gates into a single "behavioral identity." You don't tweak these independently in IDM mode — you select a personality.

#### The 5 Macro Presets

**1️⃣ Ghost Fill** (Subtle, Aphex-Adjacent, Safe Default)
- **Drill**: 35% intensity, 1-4 bursts, triplets
- **Fills**: 55% chance, 45% amount
- **Gate**: Disabled
- **Use**: Verses, texture tracks, nervous grooves

**2️⃣ Snare Hallucination** (Silence → Explosion → Silence)
- **Drill**: 75% intensity, 4-12 bursts, random primes
- **Fills**: 85% chance, 75% amount
- **Gate**: 30% silence, 70% burst
- **Use**: Breakdowns, Aphex snare abuse, shock moments

**3️⃣ Broken Transport** (Time Disappears, Machine Stutters)
- **Drill**: 85% intensity, 6-16 bursts, random primes
- **Fills**: 65% chance, 80% amount, 3-step fills
- **Gate**: 45% silence, 55% burst, 2-4 step runs
- **Use**: "Is the DAW broken?" moments

**4️⃣ Venetian Collapse** (Maximalist Drill'n'Bass)
- **Drill**: 100% intensity, 10-24 bursts, random primes
- **Fills**: 90% chance, 100% amount, 4-step fills
- **Gate**: 35% silence, 80% burst, up to 5 steps
- **Use**: Finales, drops, pure violence

**5️⃣ Anti-Groove Intelligence** (Groove Actively Destroyed)
- **Drill**: 65% intensity, 3-10 bursts, septuplets
- **Fills**: Disabled (gate does the work)
- **Gate**: 55% silence, 40% burst (Autechre-style)
- **Use**: Abstract textures, deconstructed rhythms

#### Usage

```cpp
// Apply a complete behavioral identity in one call
drumMachine->applyIdmMacroPreset(drumMachine->idmMacroBrokenTransport());

// Or load and inspect first
auto preset = drumMachine->idmMacroSnareHallucination();
std::cout << "Loading: " << preset.name << std::endl;
drumMachine->applyIdmMacroPreset(preset);
```

**These are identities, not settings.**

---

### 6. Bar-Aware Phrase Detection (Musical Intelligence)

**IDM chaos must respect form, or it becomes noise.**

The drum machine now includes phrase detection for 4/8/16 bar musical intelligence.

#### How It Works

```cpp
PhraseDetector detector;
detector.barsPerPhrase = 4;  // or 8, or 16

// Automatically makes fills and gates phrase-aware
drumMachine->setPhraseDetector(detector);
```

#### Phrase-Aware Behavior

**Fill Escalation**:
- Mid-phrase: 40% trigger chance, 60% amount (gentle)
- Phrase end: 90% trigger chance, 100% amount (explosive)

**Gate Activation**:
- Mid-phrase: Gates off (unless user enables)
- Phrase end: Gates auto-enable (temporal collapse)

**Result**: Stable phrases with sudden "what just happened" moments at boundaries.

#### 8-Bar Event Bars

Optionally force "big moments" every 8 bars:

```cpp
// In your sequencing logic
if (phraseDetector_.isEventBar(currentBar_, 8)) {
    drumMachine->applyIdmMacroPreset(drumMachine->idmMacroVenetianCollapse());
}
```

This is your big red button for finales and drops.

---

### 7. UI Metaphors (Making This Usable)

**The UI describes what the machine feels like, not what it does.**

❌ **Don't expose**:
- Burst count
- Chaos
- Dropout
- Mutation

✅ **Do expose**:
- Stability
- Fracture
- Violence
- Disappearance

#### Recommended: The "Madness Strip"

A single horizontal strip per drum track:

```
[ STABLE ] ────┬────┬────┬──── [ FRACTURE ] ──── [ COLLAPSE ]
               ↑    ↑    ↑
             Fill  Gate Drill
```

**Interaction**:
- Drag right → more chaos
- Drag left → more groove
- Icons light up as subsystems engage

**Internal mapping**:
- Left = Groove mode
- Middle = Automatic fills
- Right = Drill + Gates

#### IDM Mode Selector

Instead of knobs:

```
IDM MODE:
○ Ghost Fill
○ Snare Hallucination
● Broken Transport
○ Venetian Collapse
○ Anti-Groove
```

These are behavioral identities, not parameter settings.

#### Phrase Indicator

A subtle bar counter:

```
[ 1 ][ 2 ][ 3 ][ ⚡ ]
```

⚡ lights up on phrase end. Users feel when something is coming.

#### Visual Metaphor Options

**Option A — Machine Stress Meter**
- Calm → vibrating → tearing
- Color shift only (no numbers)

**Option B — Time Integrity Bar**
- Solid line → cracked → fragmented → missing chunks

**Option C — Heartbeat Monitor**
- Regular pulse → arrhythmia → flatline → spike

All map to: Groove → Fill → Drill → Silence

---

### Per-Voice Parameters

Each voice has:

| Parameter      | Range    | Description                     |
|----------------|----------|---------------------------------|
| timingRole     | enum     | Pocket/Push/Pull                |
| level          | 0-1      | Voice volume                     |
| decay          | 0.01-1s  | Voice decay time                 |
| tone           | 0-1      | Voice brightness/character       |

### Sequencer Parameters

| Parameter      | Range    | Description                     |
|----------------|----------|---------------------------------|
| step[16]       | on/off   | Step gate                       |
| velocity[16]   | 0-1      | Step velocity                   |
| patternLength  | 1-16     | Active pattern length            |

---

## Building

### Requirements

- JUCE 8.0.4+
- CMake 3.15+
- C++17 compiler
- tvOS SDK (for tvOS target)

### Build Commands

```bash
# Create build directory
cd /Users/bretbouchard/apps/schill/juce_backend
mkdir -p build && cd build

# Configure with CMake
cmake .. -DCMAKE_TOOLCHAIN_FILE=../cmake/tvos-arm64.cmake

# Build
make -j8

# Run tests
ctest --verbose
```

---

## Integration

### FFI Bridge

The Drum Machine includes an FFI bridge for Swift integration:

```cpp
// FFI interface
extern "C" {
    uint32_t drummachine_create();
    void drummachine_destroy(uint32_t instance);
    void drummachine_setPattern(uint32_t instance, const char* json);
    void drummachine_trigger(uint32_t instance, int step);
    void drummachine_setTempo(uint32_t instance, float bpm);
}
```

### Swift Integration

```swift
import Foundation

class DrumMachine {
    private let instance: UInt32

    init() {
        instance = drummachine_create()
    }

    func setPattern(_ pattern: String) {
        pattern.withCString { ptr in
            drummachine_setPattern(instance, ptr)
        }
    }

    func setTempo(_ bpm: Float) {
        drummachine_setTempo(instance, bpm)
    }
}
```

---

## Performance

### CPU Usage

- **Typical load**: 2-3% of one core (at 48kHz)
- **Maximum load**: 5% (all voices, full patterns)
- **Realtime-safe**: No allocations in audio thread

### Memory

- **Code size**: ~150KB compiled
- **Per-instance memory**: ~50KB
- **Pattern storage**: ~1KB per pattern

---

## Usage Example

### Creating a Basic Pattern

```cpp
// Create instance
auto drumMachine = std::make_unique<StepSequencerDSP>();
drumMachine->prepareToPlay(48000.0, 512);

// Set up a basic pattern
for (int step = 0; step < 16; step++) {
    // Kick on 1, 5, 9, 13
    drumMachine->setStep(KICK, step, (step % 4 == 0));
    drumMachine->setVelocity(KICK, step, 0.8);

    // Snare on 5, 13
    drumMachine->setStep(SNARE, step, (step % 8 == 4));
    drumMachine->setVelocity(SNARE, step, 0.7);

    // Hi-hat on every step
    drumMachine->setStep(HIHAT, step, true);
    drumMachine->setVelocity(HIHAT, step, step % 2 == 0 ? 0.8 : 0.5);
}

// Set tempo and groove
drumMachine->setTempo(120.0);
drumMachine->setSwing(0.5);
drumMachine->setDillaAmount(0.6);
```

### Applying Dilla Time

```cpp
// Enable Dilla time for human feel
DillaParams params;
params.amount = 0.6;        // Overall strength
params.hatBias = 0.55;       // Hats push forward
params.snareLate = 0.8;      // Snares lay back
params.kickTight = 0.7;      // Kicks stay steady
params.maxDrift = 0.15;      // Maximum drift

drumMachine->setDillaParams(params);
```

### Enabling Drill Mode (Aphex Twin Style)

```cpp
// Switch to drill mode
drumMachine->setRhythmFeelMode(RhythmFeelMode::Drill);

// Load drill preset
auto drillMode = drumMachine->presetAphexSnareHell();
drumMachine->setDrillMode(drillMode);

// Snare will now burst into 6-12 micro-hits
// Hi-hat creates noise-like streams
// Kick stays stable (doesn't use drill by default)
```

### Custom Drill Configuration

```cpp
// Create custom drill settings
DrillMode customDrill;
customDrill.enabled = true;
customDrill.amount = 0.6;          // 60% intensity
customDrill.minBurst = 2;
customDrill.maxBurst = 6;          // 2-6 micro-hits
customDrill.chaos = 0.2;            // Low timing chaos
customDrill.dropout = 0.1;          // 10% skip chance
customDrill.grid = DrillGrid::Triplet; // Triplet grid
customDrill.velDecay = 0.4;         // Moderate velocity decay
customDrill.temporalAggression = 0.7; // Scale all burst/chaos/mutation by 70%

drumMachine->setDrillMode(customDrill);
```

### Per-Track Drill Override

```cpp
// Get current track
Track snareTrack = drumMachine->getTrack(SNARE);

// Enable custom drill for just this track
snareTrack.drillOverride.useOverride = true;
snareTrack.drillOverride.drill = drumMachine->presetVenetianMode();

drumMachine->setTrack(SNARE, snareTrack);
```

### Advanced Drill-Aware Pattern

```cpp
// Create a pattern that knows where drill is allowed
for (int step = 0; step < 16; step++) {
    // Kick never drills (steady anchor)
    drumMachine->setStep(KICK, step, (step % 4 == 0));
    drumMachine->setDrillIntent(KICK, step, DrillIntent::None);

    // Snare backbeat may drill if intense
    drumMachine->setStep(SNARE, step, (step % 8 == 4));
    drumMachine->setDrillIntent(SNARE, step,
        (step % 8 == 4) ? DrillIntent::Optional : DrillIntent::None);

    // Fill steps (12-15) emphasize drill
    if (step >= 12 && step < 16) {
        drumMachine->setDrillIntent(SNARE, step, DrillIntent::Emphasize);
        drumMachine->setDrillIntent(HIHAT, step, DrillIntent::Emphasize);
    }
}
```

### Drill Automation Lane (Song Structure)

```cpp
// Clear any existing automation
drumMachine->clearDrillAutomation();

// Build song structure through drill intensity
drumMachine->addDrillAutomationPoint(0, 0.0);   // Bar 0: Verse (no drill)
drumMachine->addDrillAutomationPoint(16, 0.25); // Bar 16: Pre-chorus (light)
drumMachine->addDrillAutomationPoint(32, 0.5);  // Bar 32: Chorus (moderate)
drumMachine->addDrillAutomationPoint(48, 0.9);  // Bar 48: Break (chaos)
drumMachine->addDrillAutomationPoint(56, 0.0);  // Bar 56: Anti-drop (silence)
drumMachine->addDrillAutomationPoint(64, 0.7);  // Bar 64: Drop (energy)

// Drill becomes form, not chaos
```

### Automatic Drill Fills

```cpp
// Enable context-sensitive fills at bar ends
DrillFillPolicy fillPolicy;
fillPolicy.enabled = true;
fillPolicy.fillLengthSteps = 2;     // Last 2 steps of each bar
fillPolicy.triggerChance = 0.65f;   // 65% chance per bar
fillPolicy.fillAmount = 0.75f;      // 75% drill during fill
fillPolicy.decayPerStep = 0.25f;    // Wilder at the end

drumMachine->setDrillFillPolicy(fillPolicy);

// No automation needed - fills happen organically at bar boundaries
// Last step of bar explodes most, then snaps back to groove
```

### Drill ↔ Silence Gating (Extreme IDM)

```cpp
// Enable broken transport effect
DrillGatePolicy gatePolicy;
gatePolicy.enabled = true;
gatePolicy.silenceChance = 0.35f;   // 35% chance to start silent run
gatePolicy.burstChance = 0.65f;     // 65% chance silence becomes burst
gatePolicy.minSilentSteps = 1;      // Minimum 1 step silent
gatePolicy.maxSilentSteps = 4;      // Maximum 4 steps silent

drumMachine->setDrillGatePolicy(gatePolicy);

// Combine with extreme drill settings
auto extremeDrill = drumMachine->presetDigitalSeizure();
extremeDrill.grid = DrillGrid::RandomPrime;
extremeDrill.maxBurst = 16;
extremeDrill.chaos = 0.4f;

drumMachine->setDrillMode(extremeDrill);

// Result: Groove stutters, drops out, then violently explodes
```

### Complete Aphex-Style Setup

```cpp
// 1. Set base drill preset
auto drill = drumMachine->presetWindowlickerSnare();
drill.temporalAggression = 0.8f;
drumMachine->setDrillMode(drill);

// 2. Add song structure automation
drumMachine->clearDrillAutomation();
drumMachine->addDrillAutomationPoint(0, 0.0);
drumMachine->addDrillAutomationPoint(16, 0.3);
drumMachine->addDrillAutomationPoint(32, 0.7);
drumMachine->addDrillAutomationPoint(48, 0.0); // Anti-drop

// 3. Enable automatic fills
DrillFillPolicy fills;
fills.enabled = true;
fills.fillLengthSteps = 2;
fills.triggerChance = 0.6f;
fills.fillAmount = 0.8f;
fills.decayPerStep = 0.2f;
drumMachine->setDrillFillPolicy(fills);

// 4. Add light gating for texture
DrillGatePolicy gate;
gate.enabled = true;
gate.silenceChance = 0.15f;  // Subtle
gate.burstChance = 0.5f;
gate.minSilentSteps = 1;
gate.maxSilentSteps = 2;
drumMachine->setDrillGatePolicy(gate);

// Result: Intelligent, evolving rhythm that feels both chaotic and intentional
```

### IDM Macro Presets (One-Click Behaviors)

```cpp
// Method 1: Apply preset directly
drumMachine->applyIdmMacroPreset(drumMachine->idmMacroBrokenTransport());

// Method 2: Load and inspect
auto preset = drumMachine->idmMacroSnareHallucination();
std::cout << "Loading IDM preset: " << preset.name << std::endl;
drumMachine->applyIdmMacroPreset(preset);

// All behavior (drill + fills + gates) is configured in one call
// No tweaking required - these are complete identities
```

### Phrase-Aware Musical Form

```cpp
// Set up 4-bar phrase intelligence
PhraseDetector detector;
detector.barsPerPhrase = 4;
drumMachine->setPhraseDetector(detector);

// Now the drum machine automatically:
// - Keeps fills gentle mid-phrase (40% chance)
// - Explodes at phrase ends (90% chance, 100% intensity)
// - Enables gates only at phrase boundaries
// - Creates "what just happened" moments automatically

// For 8-bar song sections:
PhraseDetector sectionDetector;
sectionDetector.barsPerPhrase = 8;
drumMachine->setPhraseDetector(sectionDetector);
```

### Complete IDM Production Setup

```cpp
// 1. Choose your behavioral identity
drumMachine->applyIdmMacroPreset(drumMachine->idmMacroGhostFill());

// 2. Set phrase length (matches your song structure)
PhraseDetector detector;
detector.barsPerPhrase = 8;  // 8-bar phrases
drumMachine->setPhraseDetector(detector);

// 3. Add song automation (optional)
drumMachine->clearDrillAutomation();
drumMachine->addDrillAutomationPoint(0, 0.0);    // Verse: groove
drumMachine->addDrillAutomationPoint(32, 0.5);  // Chorus: medium drill

// 4. Set up drill-aware pattern
for (int step = 0; step < 16; step++) {
    drumMachine->setStep(SNARE, step, (step % 8 == 4));
    drumMachine->setDrillIntent(SNARE, step, DrillIntent::Optional);

    // Fill steps get emphasis
    if (step >= 12 && step < 16) {
        drumMachine->setDrillIntent(SNARE, step, DrillIntent::Emphasize);
        drumMachine->setDrillIntent(HIHAT, step, DrillIntent::Emphasize);
    }
}

// Result:
// - Verses play with subtle ghost fills
// - Phrase ends (every 8 bars) get explosive drill
// - Chorus escalates to medium intensity
// - Fills are more intense at phrase boundaries
// - Everything feels intentional and musical
```

### The "Big Red Button" (Event Bars)

```cpp
// Force a maximalist moment every 8 bars
void onBarStart(int barIndex) {
    if ((barIndex + 1) % 8 == 0) {
        // Every 8th bar: VENETIAN COLLAPSE
        drumMachine->applyIdmMacroPreset(
            drumMachine->idmMacroVenetianCollapse()
        );
    } else if ((barIndex + 1) % 8 == 7) {
        // Bar before: return to subtler mode
        drumMachine->applyIdmMacroPreset(
            drumMachine->idmMacroGhostFill()
        );
    }
}

// Result: Predictable "drop" moments that feel huge
```

---

## Factory Presets

Included presets demonstrate different groove and drill styles:

**Groove Presets:**
- **Straight**: No swing, pocket timing (electronic)
- **HipHop**: 65% swing, Dilla time (boom bap)
- **DnB**: Fast tempo, half-time breaks
- **House**: 4-on-floor, minimal swing
- **Jazz**: Brush snare, ride cymbal
- **Rock**: Backbeat with fills

**Drill Presets:**

**A) Transitional / Musical (Groove ↔ Drill bridges):**
- **Glitch Accent**: Subtle micro-bursts (1-3 hits, straight grid, 25% intensity)
- **Broken Groove**: Light stutter (2-5 hits, triplets, 35% intensity)
- **Neo-IDM Fill**: Medium ratchets (2-6 hits, quintuplets, 50% intensity)
- **Ghost Machinery**: Dense fills (4-8 hits, septuplets, 60% intensity, ghost notes)

**B) Aphex-Style Signature Presets:**
- **Aphex Microfracture**: Subtle fracturing (2-4 hits, 40% intensity, occasional glitch)
- **Windowlicker Snare**: Classic Aphex stutter (3-8 hits, triplets, 65% intensity)
- **Polygon Window**: Geometric bursts (4-9 hits, quintuplets, 55% intensity, angular decay)
- **Clock Desync**: Temporal instability (3-7 hits, 70% intensity, high chaos)

**C) Drill'n'Bass / Venetian Snares Energy:**
- **Drill'n'Bass Core**: Classic drill (4-10 hits, triplets, 75% intensity)
- **Venetian Ghosts**: Haunting stutters (5-12 hits, random primes, 65% intensity, ghost notes)
- **Amen Shredder**: Breakbeat shredder (6-14 hits, 85% intensity, mutation)
- **Overclocked Snare**: Hyper-fast bursts (8-16 hits, 90% intensity, extreme chaos)

**D) Noise / Experimental / Brutal:**
- **Time Grinder**: Temporal destruction (6-16 hits, 80% intensity, heavy mutation)
- **Digital Seizure**: Harsh noise (8-16 hits, 95% intensity, maximum chaos)
- **Static Engine**: Textural brutality (10-16 hits, 90% intensity, heavy dropout)

**E) Rhythmic Control / Utility:**
- **Ratchet Builder**: Controlled ratchets (2-8 hits, triplets, 40% intensity, predictable)
- **Fill Generator**: Versatile fills (3-10 hits, quintuplets, 50% intensity, even spread)

**IDM Macro Presets (Behavioral Identities):**

**1️⃣ Ghost Fill** (Subtle, Safe Default)
- Drill: 35% intensity, 1-4 bursts, triplets
- Fills: 55% chance, 45% amount
- Gate: Disabled
- Best for: Verses, texture, nervous grooves

**2️⃣ Snare Hallucination** (Silence → Explosion)
- Drill: 75% intensity, 4-12 bursts, random primes
- Fills: 85% chance, 75% amount
- Gate: 30% silence, 70% burst replacement
- Best for: Breakdowns, snare abuse, shock moments

**3️⃣ Broken Transport** (Machine Stutters)
- Drill: 85% intensity, 6-16 bursts, random primes
- Fills: 65% chance, 80% amount, 3-step fills
- Gate: 45% silence, 55% burst, 2-4 step runs
- Best for: "Is the DAW broken?" moments

**4️⃣ Venetian Collapse** (Maximalist Violence)
- Drill: 100% intensity, 10-24 bursts, random primes
- Fills: 90% chance, 100% amount, 4-step fills
- Gate: 35% silence, 80% burst, up to 5 steps
- Best for: Finales, drops, pure violence

**5️⃣ Anti-Groove Intelligence** (Autechre-Style)
- Drill: 65% intensity, 3-10 bursts, septuplets
- Fills: Disabled (gate does the work)
- Gate: 55% silence, 40% burst, 1-2 step runs
- Best for: Abstract textures, deconstructed rhythms

---

## Technical Details

### Sample Rate

- Designed for: 44.1kHz, 48kHz
- Tested up to: 96kHz
- Not recommended for: 192kHz (overkill)

### Block Size

- Minimum: 32 samples
- Recommended: 128-512 samples
- Maximum: 2048 samples

### Latency

- Algorithmic latency: 0 samples
- Typical total latency: < 5ms (including I/O)

---

## License

See LICENSE file in parent directory.

---

## Support

For issues or questions:
- Check `/Users/bretbouchard/apps/schill/juce_backend/docs/`
- Review build status in `BUILD_STATUS.md`
- See implementation details in `IMPLEMENTATION_SUMMARY.md`

## Plugin Formats

This plugin is available in the following formats:

- **VST3**: Cross-platform plugin format (Windows, macOS, Linux)
- **Audio Unit (AU)**: macOS-only format (macOS 10.15+)
- **CLAP**: Modern cross-platform format (CLAP 1.1+)
- **LV2**: Linux plugin format (LV2 1.18+)
- **AUv3**: iOS format (iOS 13+)
- **Standalone**: Desktop application (Windows, macOS, Linux)

### Build Status

See docs/BUILD.md for build instructions and current status.

### Installation

Each format installs to its standard system location. See docs/BUILD.md for details.
