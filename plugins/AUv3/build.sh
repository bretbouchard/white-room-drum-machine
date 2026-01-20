#!/bin/bash

# Build script for Drum Machine AUv3 iOS Plugin
# Builds for iOS Simulator (arm64) and iOS Device (arm64)

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_NAME="DrumMachinePlugin"
BUILD_DIR="${SCRIPT_DIR}/build"
PROJECT_FILE="${SCRIPT_DIR}/${PROJECT_NAME}.xcodeproj"

echo "🔨 Building Drum Machine AUv3 Plugin..."
echo "📁 Script directory: ${SCRIPT_DIR}"

# Check if Xcode project exists
if [ ! -d "${PROJECT_FILE}" ]; then
    echo "❌ Error: Xcode project not found at ${PROJECT_FILE}"
    echo "⚠️  Please create the Xcode project first (see README.md)"
    exit 1
fi

# Clean build directory
echo "🧹 Cleaning build directory..."
rm -rf "${BUILD_DIR}"
mkdir -p "${BUILD_DIR}"

# Build for iOS Simulator (arm64)
echo "📱 Building for iOS Simulator (arm64)..."
xcodebuild -project "${PROJECT_FILE}" \
           -scheme "${PROJECT_NAME}" \
           -configuration Release \
           -sdk iphonesimulator \
           -arch arm64 \
           ONLY_ACTIVE_ARCH=NO \
           -derivedDataPath "${BUILD_DIR}/simulator" \
           build || {
    echo "❌ Build failed for iOS Simulator"
    exit 1
}

# Build for iOS Device (arm64)
echo "📱 Building for iOS Device (arm64)..."
xcodebuild -project "${PROJECT_FILE}" \
           -scheme "${PROJECT_NAME}" \
           -configuration Release \
           -sdk iphoneos \
           -arch arm64 \
           ONLY_ACTIVE_ARCH=NO \
           -derivedDataPath "${BUILD_DIR}/device" \
           build || {
    echo "❌ Build failed for iOS Device"
    exit 1
}

echo "✅ Build complete!"
echo "📦 iOS Simulator build: ${BUILD_DIR}/simulator"
echo "📦 iOS Device build: ${BUILD_DIR}/device"
echo ""
echo "🎯 To install on device:"
echo "   1. Archive and distribute via Xcode Organizer"
echo "   2. Or deploy directly to connected device"
echo ""
echo "🎯 To test in GarageBand/AUM:"
echo "   1. Install app on device/simulator"
echo "   2. Open GarageBand or AUv3 host"
echo "   3. Create new track -> Plugin -> Drum Machine"
