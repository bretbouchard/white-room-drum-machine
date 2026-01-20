# White Room Drum Machine AUv3 iOS Plugin

## Project Structure

```
ios-auv3/
├── DrumMachinePluginApp/        # AUv3 host container app
│   ├── AppDelegate.swift
│   ├── ViewController.swift
│   └── Info.plist
├── DrumMachinePluginExtension/  # AUv3 extension
│   ├── AudioUnit.swift
│   ├── AudioUnitViewController.swift
│   ├── ParameterBridge.swift
│   └── Info.plist
└── SharedDSP/                   # C++ static library
    ├── DrumMachineDSP.h
    ├── DrumMachineDSP.cpp
    └── CMakeLists.txt
```

## Features

- **Platform**: iOS (AUv3 Instrument Extension)
- **DSP**: Drum Machine with 16-track step sequencer
- **Voices**: 16 synthesized drum sounds (kick, snare, hihat, clap, etc.)
- **Controls**: 16-pad drum grid, step sequencer, tempo, swing, volume
- **Presets**: JSON-based pattern and kit management
- **UI**: UIKit interface optimized for touch

## Building

### Prerequisites

1. Xcode 14.0+
2. iOS 15.0+ SDK
3. CMake 3.15+

### Create Xcode Project

⚠️ **IMPORTANT**: The Xcode project must be created manually using Xcode's project template. Here's how:

1. **Open Xcode** → File → New → Project
2. **Choose**: iOS → App
3. **Product Name**: DrumMachinePlugin
4. **Team**: Select your development team
5. **Language**: Swift
6. **Location**: `juce_backend/instruments/drummachine/ios-auv3/`
7. **Add AUv3 Extension Target**:
   - File → New → Target
   - Choose: Audio Unit Extension
   - Product Name: DrumMachinePluginExtension
   - Language: Swift

### Configure Project

1. **Add SharedDSP Static Library**:
   - File → New → Target
   - Choose: Static Library
   - Product Name: DrumMachineDSP
   - Language: C++
   - Add all files from `SharedDSP/` directory

2. **Link DSP to Extension**:
   - Select DrumMachinePluginExtension target
   - Build Phases → Link Binary With Libraries
   - Add DrumMachineDSP.lib

3. **Set Capabilities**:
   - DrumMachinePluginExtension: Inter-App Audio
   - DrumMachinePluginApp: None (container only)

4. **Update Bundle Identifiers**:
   - App: `com.whiteroom.DrumMachinePlugin`
   - Extension: `com.whiteroom.DrumMachinePlugin.Extension`

### Build

```bash
# Make build script executable
chmod +x build.sh

# Build for iOS Simulator and Device
./build.sh
```

Or build in Xcode:
- Open `DrumMachinePlugin.xcodeproj`
- Select destination (iOS Simulator or Device)
- Product → Build (⌘B)

## Deployment Target

- **iOS**: 15.0+
- **Architectures**: arm64 (device), arm64-sim (simulator)

## DSP Architecture

### Drum Machine Features

- **16 Tracks**: Each with independent volume, pan, and drum type
- **16-Step Sequencer**: Pattern-based programming with velocity control
- **16 Synthesized Voices**:
  - Kick, Snare, HiHat Closed, HiHat Open
  - Clap, Tom Low, Tom Mid, Tom High
  - Crash, Ride, Cowbell, Shaker
  - Tambourine, Percussion, Special
- **Advanced Features**:
  - Swing/shuffle control
  - Timing roles (Pocket/Push/Pull)
  - Dilla-style micro-timing
  - Drill mode (Aphex Twin-style micro-bursts)
  - Pattern automation

### Parameter Mapping

Parameters are mapped between iOS AUv3 and C++ DSP:
- Global: Tempo, Swing, Master Volume, Pattern Length
- Timing: Pocket/Push/Pull offsets, Dilla parameters
- Stereo: Width controls
- Per-track: 16 track volumes
- Voice parameters: Pitch, decay, tone per drum voice

## Notes

- DSP compiled as static C++ library
- No network operations (iOS extension sandbox)
- No background agents or LangGraph
- Pure audio/MIDI processing
- MIDI note mapping: C2 (36) = Track 0, D2 (38) = Track 1, etc.

## Testing

### In GarageBand:
1. Install app on device/simulator
2. Open GarageBand → Song → Track → Instrument
3. Plugin → Drum Machine
4. Test pads, sequencer, and controls

### In AUM (Audio Unit Manager):
1. Install AUM from App Store
2. Add new track → Plugin → Drum Machine
3. Test MIDI input and audio output

## Architecture

The Drum Machine AUv3 plugin consists of three main components:

1. **Host App**: Minimal iOS app that hosts the AUv3 extension
2. **AUv3 Extension**: The actual audio plugin that processes MIDI/Audio
3. **Shared DSP**: C++ library containing the drum machine implementation

### DSP Integration

The C++ DSP code from the drummachine instrument is compiled as a static library and linked into both the host app and the AUv3 extension. This ensures:

- Single source of truth for DSP algorithms
- Efficient performance (native C++)
- Code reuse between iOS and desktop versions

### Parameter Bridge

The `ParameterBridge.swift` file provides Swift-to-C++ interoperability:
- Wraps C++ DSP class with Objective-C-compatible interface
- Handles memory management for DSP instance
- Converts Swift types to C++ types
- Provides clean API for AUv3 extension

---

**Status**: ✅ Created 2025-01-17
**Location**: juce_backend/instruments/drummachine/ios-auv3/
