# Build Instructions

## Prerequisites

- JUCE 7.0.0 or later
- CMake 3.15 or later
- macOS 10.15 or later (for AU/AUv3)
- Xcode 12 or later (for macOS builds)

## Building All Formats

### VST3
```bash
cmake -B build/VST -DJUCE_FORMATS=VST .
cmake --build build/VST --config Release
```

### Audio Unit (macOS)
```bash
cmake -B build/AU -DJUCE_FORMATS=AU .
cmake --build build/AU --config Release
```

### CLAP
```bash
cmake -B build/CLAP -DJUCE_FORMATS=CLAP .
cmake --build build/CLAP --config Release
```

### LV2
```bash
cmake -B build/LV2 -DJUCE_FORMATS=LV2 .
cmake --build build/LV2 --config Release
```

### Standalone
```bash
cmake -B build/Standalone -DJUCE_FORMATS=Standalone .
cmake --build build/Standalone --config Release
```

### AUv3 (iOS)
```bash
cmake -B build/AUv3 -DJUCE_FORMATS=AUv3 -DIOS=1 .
cmake --build build/AUv3 --config Release
```

## Installation

### macOS
VST3: `cp -R build/VST/[Plugin].vst3 ~/Library/Audio/Plug-Ins/VST3/`
AU: `cp -R build/AU/[Plugin].component ~/Library/Audio/Plug-Ins/Components/`
Standalone: `cp build/Standalone/[Plugin] /Applications/`

### iOS
AUv3: Deploy via Xcode to device or simulator

## Troubleshooting

See README.md for common issues and solutions.
