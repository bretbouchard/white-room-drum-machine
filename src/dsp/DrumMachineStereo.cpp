/*
  ==============================================================================

    DrumMachineStereo.cpp
    Stereo processing implementation for Drum Machine
    Demonstrates per-drum stereo panning and room width

  ==============================================================================
*/

#include "dsp/DrumMachinePureDSP.h"
#include "../../../../include/dsp/StereoProcessor.h"

namespace DSP {

//==============================================================================
// DrumMachinePureDSP Stereo Processing
//==============================================================================

void DrumMachinePureDSP::processStereo(float** outputs, int numChannels, int numSamples)
{
    using namespace StereoProcessor;

    // Get stereo parameters
    float width = params_.stereoWidth;
    float roomWidth = params_.roomWidth;
    float effectsWidth = params_.effectsWidth;

    // Clear output buffers
    for (int ch = 0; ch < numChannels; ++ch)
    {
        for (int i = 0; i < numSamples; ++i)
        {
            outputs[ch][i] = 0.0f;
        }
    }

    // Process each track with stereo panning
    for (int track = 0; track < sequencer_.getNumTracks(); ++track)
    {
        Track currentTrack = sequencer_.getTrack(track);

        // Get track pan (-1.0 to 1.0, -1=left, 0=center, 1=right)
        float pan = currentTrack.pan;
        float trackVolume = params_.trackVolumes[track];

        // Calculate pan gains using constant power panning
        float panAngle = (pan + 1.0f) * 0.5f * M_PI * 0.5f;  // Map to 0-90 degrees
        float leftGain = std::cos(panAngle) * trackVolume;
        float rightGain = std::sin(panAngle) * trackVolume;

        // Process track
        std::vector<float> trackBuffer(numSamples);
        sequencer_.processTrack(track, trackBuffer.data(), numSamples);

        // Pan to stereo output
        for (int i = 0; i < numSamples; ++i)
        {
            if (numChannels >= 2)
            {
                outputs[0][i] += trackBuffer[i] * leftGain;
                outputs[1][i] += trackBuffer[i] * rightGain;
            }
            else if (numChannels == 1)
            {
                outputs[0][i] += trackBuffer[i] * trackVolume;
            }
        }
    }

    // Apply stereo width to overall mix
    if (numChannels >= 2 && width > 0.0f)
    {
        for (int i = 0; i < numSamples; ++i)
        {
            float left = outputs[0][i];
            float right = outputs[1][i];

            StereoWidth::processWidth(left, right, width);

            outputs[0][i] = left;
            outputs[1][i] = right;
        }
    }

    // Apply room width (reverb stereo enhancement)
    if (numChannels >= 2 && roomWidth > 0.0f)
    {
        // Room reverb would be processed here
        // For now, just demonstrate width on main outputs
        for (int i = 0; i < numSamples; ++i)
        {
            // Room effect would be summed here
            // outputs[0][i] += roomLeft[i];
            // outputs[1][i] += roomRight[i];
        }
    }

    // Apply master volume
    float masterVol = params_.masterVolume;
    for (int ch = 0; ch < numChannels; ++ch)
    {
        for (int i = 0; i < numSamples; ++i)
        {
            outputs[ch][i] *= masterVol;
        }
    }
}

//==============================================================================
// Implementation Examples: Advanced Drum Stereo Techniques
//==============================================================================

/*
 * Technique 1: Per-drum voice stereo positioning
 * Different drum types have default stereo positions:
 */

struct DrumStereoDefaults
{
    static float getPanForDrumType(Track::DrumType type)
    {
        switch (type)
        {
            case Track::DrumType::Kick:        return 0.0f;   // Center
            case Track::DrumType::Snare:       return 0.1f;   // Slightly right
            case Track::DrumType::HiHatClosed: return -0.3f;  // Left
            case Track::DrumType::HiHatOpen:   return -0.4f;  // Left
            case Track::DrumType::Clap:        return 0.2f;   // Right
            case Track::DrumType::TomLow:      return -0.5f;  // Far left
            case Track::DrumType::TomMid:      return -0.2f;  // Left
            case Track::DrumType::TomHigh:     return 0.3f;   // Right
            case Track::DrumType::Crash:       return -0.7f;  // Far left
            case Track::DrumType::Ride:        return 0.6f;   // Right
            case Track::DrumType::Cowbell:     return 0.4f;   // Right
            case Track::DrumType::Shaker:      return -0.6f;  // Far left
            case Track::DrumType::Tambourine:  return 0.5f;   // Right
            default:                           return 0.0f;   // Center
        }
    }
};

/*
 * Technique 2: Stereo room simulation
 * Apply different room sizes to left/right channels:
 */

void DrumMachinePureDSP::processStereoRoom(float** outputs, int numChannels, int numSamples)
{
    using namespace StereoProcessor;

    float roomWidth = params_.roomWidth;

    if (roomWidth <= 0.0f || numChannels < 2)
    {
        return;  // No room effect
    }

    // Simulate room reflections with different delays
    // Left channel: shorter room (closer wall)
    // Right channel: longer room (farther wall)

    float leftRoomSize = 0.3f + (1.0f - roomWidth) * 0.2f;  // 0.3-0.5
    float rightRoomSize = 0.4f + roomWidth * 0.3f;           // 0.4-0.7

    // Apply room simulation (simplified - would use actual reverb in production)
    for (int i = 0; i < numSamples; ++i)
    {
        float dryLeft = outputs[0][i];
        float dryRight = outputs[1][i];

        // Cross-mix for room effect
        float leftRoom = dryLeft * (1.0f - leftRoomSize) + dryRight * leftRoomSize * 0.3f;
        float rightRoom = dryRight * (1.0f - rightRoomSize) + dryLeft * rightRoomSize * 0.3f;

        outputs[0][i] = dryLeft * 0.7f + leftRoom * 0.3f;
        outputs[1][i] = dryRight * 0.7f + rightRoom * 0.3f;
    }
}

/*
 * Technique 3: Stereo effects returns
 * Process effects (delay, reverb) with separate stereo width:
 */

void DrumMachinePureDSP::processStereoEffects(float** outputs, int numChannels, int numSamples)
{
    using namespace StereoProcessor;

    float effectsWidth = params_.effectsWidth;

    if (effectsWidth <= 0.0f || numChannels < 2)
    {
        return;  // No effects width
    }

    // Process delay returns with stereo width
    // (In production, this would process actual delay buffers)

    for (int i = 0; i < numSamples; ++i)
    {
        float left = outputs[0][i];
        float right = outputs[1][i];

        // Apply effects width
        StereoWidth::processWidth(left, right, effectsWidth);

        // Mix with dry (simplified)
        outputs[0][i] = outputs[0][i] * 0.8f + left * 0.2f;
        outputs[1][i] = outputs[1][i] * 0.8f + right * 0.2f;
    }
}

/*
 * Technique 4: Drum kit stereo imaging presets
 */

struct DrumStereoPresets
{
    static void applyCompact(DrumMachinePureDSP& drum)
    {
        // Compact kit: all drums near center
        drum.params_.stereoWidth = 0.3f;
        drum.params_.roomWidth = 0.2f;
    }

    static void applyStandard(DrumMachinePureDSP& drum)
    {
        // Standard kit: balanced stereo
        drum.params_.stereoWidth = 0.5f;
        drum.params_.roomWidth = 0.4f;
    }

    static void applyWide(DrumMachinePureDSP& drum)
    {
        // Wide kit: maximum stereo spread
        drum.params_.stereoWidth = 0.8f;
        drum.params_.roomWidth = 0.6f;
    }

    static void applyRoom(DrumMachinePureDSP& drum)
    {
        // Room kit: emphasis on room sound
        drum.params_.stereoWidth = 0.6f;
        drum.params_.roomWidth = 0.8f;
        drum.params_.effectsWidth = 0.7f;
    }
};

} // namespace DSP
