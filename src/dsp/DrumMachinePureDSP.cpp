/*
  ==============================================================================

    DrumMachinePureDSP.cpp
    Created: December 30, 2025
    Author: Bret Bouchard

    Pure DSP implementation of Drum Machine
    Synthesized drum voices with step sequencing

  ==============================================================================
*/

#include "dsp/DrumMachinePureDSP.h"
#include "../../../../include/dsp/InstrumentFactory.h"
#include "../../../../include/dsp/LookupTables.h"
#include "../../../../include/dsp/DSPLogging.h"
#include <cstring>
#include <cstdlib>
#include <cmath>

namespace DSP {

//==============================================================================
// Kick Voice Implementation - Enhanced
//==============================================================================

void KickVoice::prepare(double sampleRate)
{
    sampleRate = sampleRate;
    reset();
}

void KickVoice::reset()
{
    phase = 0.0f;
    frequency = 150.0f;
    pitchEnvelope = 0.0f;
    amplitude = 0.0f;
    transientPhase = 0.0f;
    pitchSmoothing = 0.0f;
    amplitudeSmoothing = 0.0f;
}

void KickVoice::trigger(float velocity)
{
    // Enhanced amplitude envelope with faster attack
    amplitude = velocity * 3.2f;  // Boosted 4x for normalization (was 0.8)
    decay = 0.996f - (0.996f - 0.992f) * (1.0f - velocity) * 0.5f;

    // Enhanced pitch envelope with exponential decay (more realistic beater impact)
    pitchEnvelope = 1.0f;
    pitchAmount = frequency * 3.5f;  // Increased pitch sweep range
    pitchDecay = 0.985f;  // Faster initial pitch drop

    // Enhanced transient with sharper attack
    transientPhase = 1.0f;
    transientAmount = 0.45f * velocity;  // More click presence

    // Initialize smoothing
    pitchSmoothing = 0.0f;
    amplitudeSmoothing = 0.0f;
}

float KickVoice::processSample()
{
    // Enhanced pitch envelope with two-stage decay (fast then slow)
    float currentFreq = frequency + pitchEnvelope * pitchAmount;

    // Two-stage pitch decay for realistic drum beater
    if (pitchEnvelope > 0.3f)
        pitchEnvelope *= pitchDecay;  // Fast initial drop
    else
        pitchEnvelope *= 0.992f;  // Slower tail decay

    // Apply parameter smoothing (prevent zipper noise)
    pitchSmoothing = pitchSmoothing * 0.95f + currentFreq * 0.05f;

    // Generate sine wave with sub-octave content for body
    phase += static_cast<float>(pitchSmoothing / sampleRate);
    if (phase > 1.0f) phase -= 1.0f;

    float tone = SchillingerEcosystem::DSP::fastSineLookup(phase * 2.0f * M_PI);
    float subOctave = SchillingerEcosystem::DSP::fastSineLookup(phase * M_PI) * 0.3f;  // Add sub-bass
    tone = tone * 0.7f + subOctave;  // Mix for fuller sound

    // Enhanced transient with band-limited click
    float transient = 0.0f;
    if (transientPhase > 0.0f)
    {
        // Sharper transient curve
        float transientCurve = transientPhase * transientPhase;  // Quadratic decay
        transient = std::sin(transientCurve * M_PI * 0.5f) * transientAmount;
        transientPhase -= 0.08f;  // Slightly longer transient
        if (transientPhase < 0.0f) transientPhase = 0.0f;
    }

    // Smoothed amplitude envelope
    float targetAmplitude = amplitude;
    amplitudeSmoothing = amplitudeSmoothing * 0.9f + targetAmplitude * 0.1f;

    amplitude *= decay;
    if (amplitude < 0.0001f) amplitude = 0.0f;

    return (tone + transient) * amplitudeSmoothing;
}

void KickVoice::setPitch(float pitch)
{
    float targetFreq = 50.0f + pitch * 200.0f;
    // Smooth frequency changes
    frequency = frequency * 0.8f + targetFreq * 0.2f;
}

void KickVoice::setDecay(float decay)
{
    pitchDecay = 0.985f + decay * 0.012f;  // Expanded range
}

void KickVoice::setClick(float click)
{
    // Smooth click parameter changes
    transientAmount = transientAmount * 0.9f + click * 0.1f;
}

//==============================================================================
// Snare Voice Implementation - Enhanced
//==============================================================================

void SnareVoice::prepare(double sampleRate)
{
    sampleRate = sampleRate;
    reset();
}

void SnareVoice::reset()
{
    tonePhase = 0.0f;
    toneAmplitude = 0.0f;
    noiseAmplitude = 0.0f;
    filterState = 0.0f;
    snapAmplitude = 0.0f;
    rattlePhase = 0.0f;
    filterSmoothing = 0.0f;
    toneSmoothing = 0.0f;
}

void SnareVoice::trigger(float velocity)
{
    // Enhanced tone with richer harmonics
    toneAmplitude = 2.8f * velocity;  // Boosted 4x for normalization (was 0.7)
    toneDecay = 0.992f - (0.992f - 0.988f) * (1.0f - velocity) * 0.5f;

    // Enhanced noise with more body
    noiseAmplitude = 3.4f * velocity;  // Boosted 4x for normalization (was 0.85)
    noiseDecay = 0.996f - (0.996f - 0.992f) * (1.0f - velocity) * 0.5f;

    // Enhanced snap with sharper attack
    snapAmplitude = 2.4f * velocity;  // Boosted 4x for normalization (was 0.6)
    snapDecay = 0.92f;

    // Initialize snare rattle (snares buzzing against bottom head)
    rattlePhase = 1.0f;

    filterState = 0.0f;
    filterSmoothing = filterResonance;
    toneSmoothing = 0.0f;
}

float SnareVoice::processSample()
{
    // Generate enhanced tone with multiple harmonics (triangle + sawtooth mix)
    tonePhase += static_cast<float>(toneFreq / sampleRate);
    if (tonePhase > 1.0f) tonePhase -= 1.0f;

    float triangle = (tonePhase < 0.5f) ? (tonePhase * 4.0f - 1.0f) : (3.0f - tonePhase * 4.0f);
    float square = (tonePhase < 0.5f) ? 0.7f : -0.7f;  // Softer square
    float tone = triangle * 0.6f + square * 0.2f;  // Mix for richer tone

    toneAmplitude *= toneDecay;

    // Generate noise (deterministic LCG with per-instance seed)
    noiseSeed = noiseSeed * 1103515245 + 12345;
    float noise = static_cast<float>((noiseSeed & 0x7fffffff)) / static_cast<float>(0x7fffffff) * 2.0f - 1.0f;

    // Enhanced snare rattle (high-frequency buzz)
    float rattle = 0.0f;
    if (rattlePhase > 0.0f)
    {
        noiseSeed = noiseSeed * 1103515245 + 12345;
        float rattleNoise = static_cast<float>((noiseSeed & 0x7fffffff)) / static_cast<float>(0x7fffffff) * 2.0f - 1.0f;
        rattle = rattleNoise * rattlePhase * 0.3f;
        rattlePhase *= 0.994f;  // Fast decay for rattle
        if (rattlePhase < 0.01f) rattlePhase = 0.0f;
    }

    // Enhanced filter with resonance (bandpass for snare body)
    float targetCoeff = 1.0f - filterResonance;
    filterSmoothing = filterSmoothing * 0.98f + targetCoeff * 0.02f;  // Smooth filter changes

    // Two-pole filter for better frequency response
    float filterInput = noise + rattle;
    filterState = filterState * filterSmoothing + filterInput * (1.0f - filterSmoothing);

    // Add high-frequency content for snare wires
    float highFreq = (filterInput - filterState) * 0.4f;

    noiseAmplitude *= noiseDecay;

    // Enhanced snap with more impact
    float snap = std::sin(snapAmplitude * 12.0f) * snapAmplitude * 1.2f;
    snapAmplitude *= snapDecay;

    // Smooth tone amplitude
    toneSmoothing = toneSmoothing * 0.9f + toneAmplitude * 0.1f;

    // Mix components with better balance
    float output = tone * toneSmoothing + filterState * noiseAmplitude + highFreq * noiseAmplitude * 0.5f + snap;

    return output;
}

void SnareVoice::setTone(float tone)
{
    // Smooth filter parameter changes to prevent zipper noise
    filterResonance = filterResonance * 0.9f + tone * 0.1f;
}

void SnareVoice::setDecay(float decay)
{
    noiseDecay = 0.992f + decay * 0.008f;  // Expanded range for longer snares
}

void SnareVoice::setSnap(float snap)
{
    // Smooth snap parameter changes
    snapAmplitude = snapAmplitude * 0.95f + snap * 0.05f;
}

//==============================================================================
// Hi-Hat Voice Implementation - Enhanced with Improved Metallic Cymbals
//==============================================================================

void HiHatVoice::prepare(double sampleRate)
{
    sampleRate = sampleRate;
    reset();
}

void HiHatVoice::reset()
{
    noisePhase = 0.0f;
    amplitude = 0.0f;
    filterState = 0.0f;
    metalPhase = 0.0f;
    metalPhase2 = 0.0f;
    metalPhase3 = 0.0f;
    filterSmoothing = 0.0f;
    amplitudeSmoothing = 0.0f;
}

void HiHatVoice::trigger(float velocity)
{
    amplitude = velocity * 2.8f;  // Boosted 4x for normalization (was 0.7)
    decay = 0.97f - (0.97f - 0.92f) * (1.0f - velocity) * 0.5f;
    filterState = 0.0f;
    metalAmount = 0.15f;  // Increased metallic content

    // Initialize multiple metallic oscillators at inharmonic frequencies
    metalPhase = 0.0f;
    metalPhase2 = 0.0f;
    metalPhase3 = 0.0f;

    filterSmoothing = filterCoeff;
    amplitudeSmoothing = 0.0f;
}

float HiHatVoice::processSample()
{
    // Generate high-frequency noise (deterministic LCG with per-instance seed)
    noiseSeed = noiseSeed * 1103515245 + 12345;
    float noise = static_cast<float>((noiseSeed & 0x7fffffff)) / static_cast<float>(0x7fffffff) * 2.0f - 1.0f;

    // Enhanced high-pass filter with better frequency response
    float targetCoeff = filterCoeff;
    filterSmoothing = filterSmoothing * 0.98f + targetCoeff * 0.02f;

    float highpass = noise - filterState;
    filterState = noise * filterSmoothing;

    // Enhanced metallic overtones with multiple FM oscillators
    // Primary metallic tone
    float metal1 = SchillingerEcosystem::DSP::fastSineLookup(metalPhase * 2.0f * M_PI) * metalAmount;
    metalPhase += 0.7f;  // Very high frequency
    if (metalPhase > 1.0f) metalPhase -= 1.0f;

    // Secondary metallic overtone (slightly detuned)
    float metal2 = SchillingerEcosystem::DSP::fastSineLookup(metalPhase2 * 2.0f * M_PI) * metalAmount * 0.6f;
    metalPhase2 += 0.53f;  // Inharmonic ratio
    if (metalPhase2 > 1.0f) metalPhase2 -= 1.0f;

    // Tertiary metallic overtone (higher frequency)
    float metal3 = SchillingerEcosystem::DSP::fastSineLookup(metalPhase3 * 2.0f * M_PI) * metalAmount * 0.4f;
    metalPhase3 += 1.1f;  // Even higher frequency
    if (metalPhase3 > 1.0f) metalPhase3 -= 1.0f;

    // Mix metallic components for rich cymbal sound
    float metal = metal1 + metal2 + metal3;

    // Add frequency modulation for shimming effect
    float fmMod = SchillingerEcosystem::DSP::fastSineLookup(metalPhase * 4.0f * M_PI) * 0.1f;
    metal += metal * fmMod;

    // Apply amplitude smoothing (prevent zipper noise)
    float targetAmplitude = amplitude;
    amplitudeSmoothing = amplitudeSmoothing * 0.9f + targetAmplitude * 0.1f;

    amplitude *= decay;
    if (amplitude < 0.0001f) amplitude = 0.0f;

    // Mix high-pass noise and metallic content
    float output = (highpass * 0.6f + metal * 0.4f) * amplitudeSmoothing;

    return output * 0.6f;  // Slightly lower overall level
}

void HiHatVoice::setTone(float tone)
{
    // Smooth filter parameter changes
    filterCoeff = filterCoeff * 0.95f + tone * 0.05f;
}

void HiHatVoice::setDecay(float decay)
{
    this->decay = 0.92f + decay * 0.08f;  // Expanded range for longer decays
}

void HiHatVoice::setMetallic(float metallic)
{
    // Smooth metallic parameter changes
    metalAmount = metalAmount * 0.9f + metallic * 0.1f;
}

//==============================================================================
// Clap Voice Implementation - Enhanced
//==============================================================================

void ClapVoice::prepare(double sampleRate)
{
    sampleRate = sampleRate;
    reset();
}

void ClapVoice::reset()
{
    amplitude = 0.0f;
    decay = 0.97f;
    currentImpulse = 0;
    impulseCounter = 0;
    filterState = 0.0f;
    filterSmoothing = 0.0f;
    amplitudeSmoothing = 0.0f;
}

void ClapVoice::trigger(float velocity)
{
    amplitude = velocity * 3.2f;  // Boosted 4x for normalization (was 0.8)
    decay = 0.975f - (0.975f - 0.945f) * (1.0f - velocity) * 0.5f;
    currentImpulse = 0;
    impulseCounter = 0;
    filterState = 0.0f;
    filterSmoothing = filterCoeff;
    amplitudeSmoothing = 0.0f;
}

float ClapVoice::processSample()
{
    float output = 0.0f;

    // Generate multiple noise bursts with natural timing
    if (currentImpulse < numImpulses)
    {
        if (impulseCounter <= 0)
        {
            // Trigger new impulse with slight randomization for natural feel
            impulseCounter = impulseSpacing + (currentImpulse % 2) * 100;
            currentImpulse++;
        }
        else
        {
            impulseCounter--;
        }
    }

    // Generate noise (deterministic LCG with per-instance seed)
    noiseSeed = noiseSeed * 1103515245 + 12345;
    float noise = static_cast<float>((noiseSeed & 0x7fffffff)) / static_cast<float>(0x7fffffff) * 2.0f - 1.0f;

    // Enhanced filter with smoothing
    float targetCoeff = filterCoeff;
    filterSmoothing = filterSmoothing * 0.98f + targetCoeff * 0.02f;
    filterState = filterState * filterSmoothing + noise * (1.0f - filterSmoothing);

    // Apply amplitude smoothing
    float targetAmplitude = amplitude;
    amplitudeSmoothing = amplitudeSmoothing * 0.9f + targetAmplitude * 0.1f;

    amplitude *= decay;
    if (amplitude < 0.0001f) amplitude = 0.0f;

    return filterState * amplitudeSmoothing;
}

void ClapVoice::setTone(float tone)
{
    // Smooth filter parameter changes
    filterCoeff = filterCoeff * 0.95f + tone * 0.05f;
}

void ClapVoice::setDecay(float decay)
{
    this->decay = 0.945f + decay * 0.055f;  // Expanded range
}

void ClapVoice::setNumImpulses(int num)
{
    numImpulses = std::max(1, std::min(8, num));  // Clamp to reasonable range
}

//==============================================================================
// Percussion Voice Implementation - Enhanced (for Toms, Cowbell, etc.)
//==============================================================================

void PercVoice::prepare(double sampleRate)
{
    sampleRate = sampleRate;
    reset();
}

void PercVoice::reset()
{
    phase = 0.0f;
    phase2 = 0.0f;  // Second oscillator for richer sound
    frequency = 200.0f;
    amplitude = 0.0f;
    toneMix = 0.7f;
    noiseAmplitude = 0.0f;
    pitchSmoothing = 0.0f;
    amplitudeSmoothing = 0.0f;
}

void PercVoice::trigger(float velocity)
{
    amplitude = velocity * 3.0f;  // Boosted 4x for normalization (was 0.75)
    decay = 0.992f;  // Slightly longer decay
    noiseAmplitude = velocity * 1.0f;  // Boosted 4x for normalization (was 0.25)
    pitchSmoothing = frequency;
    amplitudeSmoothing = 0.0f;
}

float PercVoice::processSample()
{
    // Apply pitch smoothing (prevent zipper noise)
    pitchSmoothing = pitchSmoothing * 0.98f + frequency * 0.02f;

    // Generate primary tone (sine wave)
    phase += static_cast<float>(pitchSmoothing / sampleRate);
    if (phase > 1.0f) phase -= 1.0f;
    float tone = SchillingerEcosystem::DSP::fastSineLookup(phase * 2.0f * M_PI);

    // Generate secondary tone at slight detune for body resonance
    phase2 += static_cast<float>(pitchSmoothing * 1.5f / sampleRate);  // Fifth above
    if (phase2 > 1.0f) phase2 -= 1.0f;
    float tone2 = SchillingerEcosystem::DSP::fastSineLookup(phase2 * 2.0f * M_PI) * 0.2f;

    // Mix tones for fuller sound
    tone = tone * 0.8f + tone2;

    // Generate noise (deterministic LCG with per-instance seed)
    noiseSeed = noiseSeed * 1103515245 + 12345;
    float noise = static_cast<float>((noiseSeed & 0x7fffffff)) / static_cast<float>(0x7fffffff) * 2.0f - 1.0f;

    // Apply amplitude smoothing
    float targetAmplitude = amplitude;
    amplitudeSmoothing = amplitudeSmoothing * 0.9f + targetAmplitude * 0.1f;

    amplitude *= decay;
    if (amplitude < 0.0001f) amplitude = 0.0f;
    noiseAmplitude *= decay;

    // Mix tone and noise with better balance
    return (tone * toneMix + noise * (1.0f - toneMix)) * amplitudeSmoothing;
}

void PercVoice::setPitch(float pitch)
{
    float targetFreq = 100.0f + pitch * 400.0f;
    // Smooth frequency changes
    frequency = frequency * 0.8f + targetFreq * 0.2f;
}

void PercVoice::setDecay(float decay)
{
    this->decay = 0.992f + decay * 0.007f;  // Expanded range
}

void PercVoice::setTone(float tone)
{
    // Smooth tone mix changes
    toneMix = toneMix * 0.95f + tone * 0.05f;
}

//==============================================================================
// Cymbal Voice Implementation - Enhanced with More Metallic Decay
//==============================================================================

void CymbalVoice::prepare(double sampleRate)
{
    sampleRate = sampleRate;
    reset();
}

void CymbalVoice::reset()
{
    for (int i = 0; i < numOscillators; ++i)
    {
        phases[i] = 0.0f;
        frequencies[i] = 0.0f;
        amplitudes[i] = 0.0f;
    }
    masterAmplitude = 0.0f;
    decay = 0.999f;
    fmDepth = 0.0f;
    fmPhase = 0.0f;
    fmPhase2 = 0.0f;  // Second FM oscillator for richer metallic sound
    amplitudeSmoothing = 0.0f;
}

void CymbalVoice::trigger(float velocity)
{
    // Enhanced inharmonic frequencies based on real cymbal spectra
    frequencies[0] = 500.0f;   // Fundamental
    frequencies[1] = 723.0f;   // Minor third
    frequencies[2] = 1150.0f;  // Fifth above
    frequencies[3] = 1370.0f;  // Seventh
    frequencies[4] = 1850.0f;  // High overtone
    frequencies[5] = 2430.0f;  // Very high overtone

    // Set amplitudes with spectral balance
    amplitudes[0] = velocity * 0.25f;  // Strong fundamental
    amplitudes[1] = velocity * 0.20f;
    amplitudes[2] = velocity * 0.18f;
    amplitudes[3] = velocity * 0.15f;
    amplitudes[4] = velocity * 0.12f;
    amplitudes[5] = velocity * 0.10f;  // Weaker high overtones

    masterAmplitude = velocity * 2.4f;  // Boosted 4x for normalization (was 0.6)
    decay = 0.9992f - (0.9992f - 0.9985f) * (1.0f - velocity) * 0.5f;  // Longer decay
    fmDepth = 0.15f;  // Increased FM for more metallic sound

    amplitudeSmoothing = 0.0f;
}

float CymbalVoice::processSample()
{
    float output = 0.0f;

    // Enhanced FM modulation with two oscillators
    float fmMod = SchillingerEcosystem::DSP::fastSineLookup(fmPhase * 2.0f * M_PI) * fmDepth;
    fmPhase += 0.08f;  // Slower FM for smooth modulation
    if (fmPhase > 1.0f) fmPhase -= 1.0f;

    float fmMod2 = SchillingerEcosystem::DSP::fastSineLookup(fmPhase2 * 2.0f * M_PI) * fmDepth * 0.5f;
    fmPhase2 += 0.13f;  // Different FM rate for complexity
    if (fmPhase2 > 1.0f) fmPhase2 -= 1.0f;

    float combinedFm = fmMod + fmMod2;

    for (int i = 0; i < numOscillators; ++i)
    {
        // Apply FM modulation with depth variation per partial
        float partialFmDepth = fmDepth * (1.0f + i * 0.1f);  // Higher partials get more FM
        float modFreq = frequencies[i] * (1.0f + combinedFm * partialFmDepth);

        phases[i] += static_cast<float>(modFreq / sampleRate);
        if (phases[i] > 1.0f) phases[i] -= 1.0f;

        output += SchillingerEcosystem::DSP::fastSineLookup(phases[i] * 2.0f * M_PI) * amplitudes[i];
    }

    // Apply amplitude smoothing
    float targetAmplitude = masterAmplitude;
    amplitudeSmoothing = amplitudeSmoothing * 0.95f + targetAmplitude * 0.05f;

    // Apply envelope
    masterAmplitude *= decay;
    if (masterAmplitude < 0.0001f) masterAmplitude = 0.0f;

    return output * amplitudeSmoothing * 0.25f;  // Slightly higher output level
}

void CymbalVoice::setTone(float tone)
{
    // Brightness control: scale higher partials
    float brightnessMultiplier = 0.5f + tone * 1.0f;  // 0.5 to 1.5
    for (int i = 3; i < numOscillators; ++i)
    {
        amplitudes[i] *= brightnessMultiplier;
    }
}

void CymbalVoice::setDecay(float decay)
{
    this->decay = 0.9985f + decay * 0.0012f;  // Expanded range for longer decays
}

void CymbalVoice::setMetallic(float metallic)
{
    // Smooth FM depth changes
    fmDepth = fmDepth * 0.9f + metallic * 0.1f;
}

//==============================================================================
// Step Sequencer Implementation
//==============================================================================

StepSequencer::StepSequencer()
{
    // Initialize tracks with default drum types
    tracks_[0].type = Track::DrumType::Kick;
    tracks_[0].timingRole = TimingRole::Pocket;  // Kick = steady
    tracks_[1].type = Track::DrumType::Snare;
    tracks_[1].timingRole = TimingRole::Pull;   // Snare = late
    tracks_[2].type = Track::DrumType::HiHatClosed;
    tracks_[2].timingRole = TimingRole::Push;   // Hi-hat = early
    tracks_[3].type = Track::DrumType::HiHatOpen;
    tracks_[3].timingRole = TimingRole::Push;
    tracks_[4].type = Track::DrumType::Clap;
    tracks_[4].timingRole = TimingRole::Pull;   // Clap = late
    tracks_[5].type = Track::DrumType::TomLow;
    tracks_[5].timingRole = TimingRole::Pocket;
    tracks_[6].type = Track::DrumType::TomMid;
    tracks_[6].timingRole = TimingRole::Pocket;
    tracks_[7].type = Track::DrumType::TomHigh;
    tracks_[7].timingRole = TimingRole::Pocket;
    tracks_[8].type = Track::DrumType::Crash;
    tracks_[8].timingRole = TimingRole::Pocket;
    tracks_[9].type = Track::DrumType::Ride;
    tracks_[9].timingRole = TimingRole::Pocket;
    tracks_[10].type = Track::DrumType::Cowbell;
    tracks_[10].timingRole = TimingRole::Pocket;
    tracks_[11].type = Track::DrumType::Shaker;
    tracks_[11].timingRole = TimingRole::Push;   // Shaker = early
    tracks_[12].type = Track::DrumType::Tambourine;
    tracks_[12].timingRole = TimingRole::Push;
    tracks_[13].type = Track::DrumType::Percussion;
    tracks_[13].timingRole = TimingRole::Pocket;
    tracks_[14].type = Track::DrumType::Percussion;
    tracks_[14].timingRole = TimingRole::Pocket;
    tracks_[15].type = Track::DrumType::Special;
    tracks_[15].timingRole = TimingRole::Pocket;

    // Initialize Dilla drift states
    for (auto& state : dillaStates_)
    {
        state.drift = 0.0f;
    }
}

void StepSequencer::prepare(double sampleRate, int samplesPerBlock)
{
    sampleRate_ = sampleRate;
    setTempo(tempo_);

    // Reset micro-hit safety counter at start of each audio block
    microHitsThisBlock_ = 0;

    // Prepare all drum voices
    kick_.prepare(sampleRate);
    snare_.prepare(sampleRate);
    hihatClosed_.prepare(sampleRate);
    hihatOpen_.prepare(sampleRate);
    clap_.prepare(sampleRate);
    tomLow_.prepare(sampleRate);
    tomMid_.prepare(sampleRate);
    tomHigh_.prepare(sampleRate);
    crash_.prepare(sampleRate);
    ride_.prepare(sampleRate);
    cowbell_.prepare(sampleRate);
    shaker_.prepare(sampleRate);
    tambourine_.prepare(sampleRate);
    percussion_.prepare(sampleRate);
    special_.prepare(sampleRate);
}

void StepSequencer::reset()
{
    position_ = 0.0;
    currentStep_ = 0;
    microHitsThisBlock_ = 0;  // Reset micro-hit safety counter

    kick_.reset();
    snare_.reset();
    hihatClosed_.reset();
    hihatOpen_.reset();
    clap_.reset();
    tomLow_.reset();
    tomMid_.reset();
    tomHigh_.reset();
    crash_.reset();
    ride_.reset();
    cowbell_.reset();
    shaker_.reset();
    tambourine_.reset();
    percussion_.reset();
    special_.reset();
}

void StepSequencer::setTempo(float bpm)
{
    tempo_ = bpm;
    float beatsPerSecond = bpm / 60.0f;
    samplesPerBeat_ = sampleRate_ / beatsPerSecond;
    samplesPerStep_ = samplesPerBeat_ / 4.0f;  // 16th notes
}

void StepSequencer::setSwing(float swingAmount)
{
    swingAmount_ = swingAmount;
}

void StepSequencer::setPatternLength(int length)
{
    patternLength_ = std::max(1, std::min(16, length));
}

bool StepSequencer::isTrackTriggered(int trackIndex, int stepIndex) const
{
    if (trackIndex < 0 || trackIndex >= static_cast<int>(tracks_.size())) return false;
    if (stepIndex < 0 || stepIndex >= 16) return false;

    return tracks_[trackIndex].steps[stepIndex].active;
}

void StepSequencer::triggerTrack(int trackIndex, int stepIndex, float velocity)
{
    if (trackIndex < 0 || trackIndex >= static_cast<int>(tracks_.size())) return;
    if (stepIndex < 0 || stepIndex >= 16) return;

    auto& step = tracks_[trackIndex].steps[stepIndex];

    // Check probability
    if (step.probability < 1.0f)
    {
        probSeed = probSeed * 1103515245 + 12345;
        float randVal = static_cast<float>((probSeed & 0x7fffffff)) / static_cast<float>(0x7fffffff);
        if (randVal > step.probability) return;
    }

    // Trigger appropriate drum voice
    Track::DrumType type = tracks_[trackIndex].type;

    // Apply flam
    if (step.hasFlam)
    {
        // Trigger twice with slight offset
        processDrumVoice(type, velocity * 0.7f);
    }

    // Apply roll
    if (step.isRoll && step.rollNotes > 1)
    {
        // Trigger multiple times within the step
        for (int i = 0; i < step.rollNotes; ++i)
        {
            processDrumVoice(type, velocity);
        }
    }
    else
    {
        processDrumVoice(type, velocity);
    }
}

void StepSequencer::triggerAllTracks(int stepIndex)
{
    // ========================================================================
    // PHASE 0: Phrase-Aware Intelligence (Musical Form)
    // ========================================================================

    // Create phrase-aware copies of policies (don't modify originals)
    DrillFillPolicy phraseAwareFill = drillFillPolicy_;
    DrillGatePolicy phraseAwareGate = drillGatePolicy_;

    // Phrase-aware fill escalation
    if (phraseDetector_.isPhraseEnd(currentBar_))
    {
        // Phrase boundaries: more intense fills
        phraseAwareFill.triggerChance = std::max(phraseAwareFill.triggerChance, 0.9f);
        phraseAwareFill.fillAmount = std::max(phraseAwareFill.fillAmount, 1.0f);
    }
    else
    {
        // Mid-phrase: gentler fills
        phraseAwareFill.triggerChance = std::min(phraseAwareFill.triggerChance, 0.4f);
        phraseAwareFill.fillAmount = std::min(phraseAwareFill.fillAmount, 0.6f);
    }

    // Phrase-aware gate activation (temporal collapse at boundaries)
    if (phraseDetector_.isPhraseEnd(currentBar_))
    {
        phraseAwareGate.enabled = true;  // Enable gate at phrase ends
    }
    else if (!drillGatePolicy_.enabled)
    {
        phraseAwareGate.enabled = false;  // Keep gates off mid-phrase unless user enabled
    }

    // ========================================================================
    // PHASE 1: Calculate global drill amount (composition + automation + fill)
    // ========================================================================

    // Start with base drill amount from drill mode
    float effectiveDrillAmount = drillMode_.amount;

    // Apply automation (compositional sequencing)
    if (!drillAutomation_.points.empty())
    {
        const float automatedAmount = drillAutomation_.evaluateAt(currentBar_);
        effectiveDrillAmount = automatedAmount; // Automation overrides base
    }

    // Apply automatic fill escalation (context-sensitive, phrase-aware)
    if (drillFillState_.active && isFillStep(stepIndex, getStepsPerBar(), phraseAwareFill))
    {
        // Calculate fill step index (0 = first fill step)
        const int fillStepIndex = stepIndex - (getStepsPerBar() - phraseAwareFill.fillLengthSteps);

        // Linear decay across fill (last step is most wild)
        const float decay = 1.0f - (fillStepIndex * phraseAwareFill.decayPerStep);
        const float fillAmount = phraseAwareFill.fillAmount * std::max(0.0f, decay);

        // Fill increases drill amount (takes maximum)
        effectiveDrillAmount = std::max(effectiveDrillAmount, fillAmount);
    }

    // ========================================================================
    // PHASE 2: Check global gate (applies to all tracks, phrase-aware)
    // ========================================================================

    const bool globallyGated = shouldGateStep(phraseAwareGate);

    // ========================================================================
    // PHASE 3: Process each track
    // ========================================================================

    for (int i = 0; i < static_cast<int>(tracks_.size()); ++i)
    {
        if (!tracks_[i].steps[stepIndex].active)
            continue;

        Track::DrumType type = tracks_[i].type;
        const StepCell& cell = tracks_[i].steps[stepIndex];

        // ====================================================================
        // Gate Logic (per-track)
        // ====================================================================

        if (globallyGated)
        {
            // Either complete silence or replace with extreme burst
            if (drillRng_.next01() >= drillGatePolicy_.burstChance)
            {
                // Silence: skip this step entirely
                continue;
            }
            else
            {
                // Replace silence with extreme drill burst
                effectiveDrillAmount = 1.0f;
            }
        }

        // ====================================================================
        // Drill-Aware Pattern Resolution
        // ====================================================================

        // Check if this specific cell wants drill (semantic intent)
        const bool cellWantsIt = cellWantsDrill(cell, drillMode_, effectiveDrillAmount);

        // Check if track type supports drill (fallback to old logic)
        const bool trackWantsIt = trackWantsDrill(type);

        // Final decision: need both track AND cell to agree
        const bool useDrillForTrack = cellWantsIt && trackWantsIt &&
                                      (rhythmFeelMode_ == RhythmFeelMode::Drill ||
                                       cell.useDrill ||
                                       effectiveDrillAmount > 0.0f);

        // ====================================================================
        // Execute Timing + Trigger
        // ====================================================================

        if (useDrillForTrack)
        {
            // DRILL MODE: Apply micro-burst scheduling
            // Note: drill mode bypasses groove timing layers for burst hits
            double stepStartSeconds = 0.0; // Relative to current step
            double stepDurationSeconds = samplesPerStep_ / sampleRate_;

            // Pass effective drill amount (from automation/fill/gate)
            scheduleMicroBurst(i, cell, stepStartSeconds, stepDurationSeconds, effectiveDrillAmount);
        }
        else
        {
            // GROOVE MODE: Apply timing layers (swing + role + Dilla)
            applyTimingLayers(i, stepIndex);
            triggerTrack(i, stepIndex, cell.velocity / 127.0f);
        }
    }
}

void StepSequencer::advance(int numSamples)
{
    position_ += numSamples;

    // Check if we've advanced past the current step
    while (position_ >= samplesPerStep_)
    {
        position_ -= samplesPerStep_;
        advanceStep();
    }
}

void StepSequencer::advanceStep()
{
    currentStep_ = (currentStep_ + 1) % patternLength_;

    // Update bar index for automation
    updateBarIndex();

    // Check if we're at the start of a new bar (step 0)
    if (currentStep_ == 0)
    {
        // Apply phrase-aware intelligence to fill policy for the new bar
        DrillFillPolicy phraseAwareFill = drillFillPolicy_;

        // Phrase boundaries get more aggressive fill triggering
        if (phraseDetector_.isPhraseEnd(currentBar_))
        {
            phraseAwareFill.triggerChance = std::max(phraseAwareFill.triggerChance, 0.9f);
        }
        else
        {
            phraseAwareFill.triggerChance = std::min(phraseAwareFill.triggerChance, 0.4f);
        }

        // Update fill state for the new bar (phrase-aware)
        updateFillState(phraseAwareFill);
    }

    // Trigger all tracks at current step
    triggerAllTracks(currentStep_);
}

//==============================================================================
// Timing System Implementation - Enhanced with Better Swing and Dilla Drift
//==============================================================================

float StepSequencer::getSwingOffset(int stepIndex) const
{
    // Enhanced swing with better groove feel
    // Apply swing to odd-numbered steps (1, 3, 5, etc.) with curve
    if (stepIndex % 2 == 1)
    {
        // Apply eased swing curve for more natural feel
        // Swing amount 0-1 maps to 0-50% of step duration
        float swingFraction = swingAmount_ * 0.5f;

        // Apply slight curve to swing for more musical feel
        // At low swing: linear, at high swing: eased
        if (swingAmount_ > 0.5f)
        {
            // Ease out curve for heavy swing
            float t = (swingAmount_ - 0.5f) * 2.0f;  // 0-1
            swingFraction = 0.25f + t * t * 0.25f;  // 25% to 50%
        }

        return swingFraction;
    }
    return 0.0f;
}

void StepSequencer::updateDillaDrift(int trackIndex, TimingRole role)
{
    DillaState& state = dillaStates_[trackIndex];
    const DillaParams& p = dillaParams_;

    float instability = 0.0f;
    float bias = 0.0f;
    float correctionStrength = 0.01f;  // Subtle pull toward center

    // Enhanced instability and bias based on timing role
    switch (role)
    {
        case TimingRole::Pocket:
            // Kick and toms: tight but with micro-variation
            instability = 0.015f * p.amount * (1.0f - p.kickTight);  // Less if kick tight
            bias = 0.0f;
            correctionStrength = 0.02f;  // Stronger correction for pocket
            break;

        case TimingRole::Push:
        {
            // Hi-hats, shaker, tambourine: push forward (early)
            // Interpolate between pull and push based on hatBias
            float pushAmount = p.hatBias;  // 0 = pull, 1 = push
            instability = 0.07f * p.amount;
            bias = -pushAmount * 0.08f + (1.0f - pushAmount) * 0.02f;  // More push early
            correctionStrength = 0.005f;  // Weaker correction for push
            break;
        }

        case TimingRole::Pull:
        {
            // Snares, claps: lay back (late)
            instability = 0.05f * p.amount;
            bias = +p.snareLate * 0.10f;  // Positive = late
            correctionStrength = 0.008f;  // Medium correction for pull
            break;
        }
    }

    // Enhanced random walk with deterministic PRNG
    probSeed = probSeed * 1103515245 + 12345;
    float randomVal = static_cast<float>((probSeed & 0x7fffffff)) / static_cast<float>(0x7fffffff);

    // Perlin-like smoothed noise for more natural drift
    float delta = (randomVal - 0.5f) * instability;

    // Add bias (directional tendency)
    delta += bias * 0.5f;

    // Apply delta with smoothing for more organic feel
    state.drift = state.drift * 0.98f + delta * 0.02f;

    // Add subtle correction toward center (prevents excessive drift)
    state.drift -= state.drift * correctionStrength;

    // Clamp drift with soft limiting
    float maxDrift = p.maxDrift;
    if (std::abs(state.drift) > maxDrift)
    {
        // Soft clamp: tanh curve
        state.drift = std::tanh(state.drift / maxDrift) * maxDrift;
    }
}

void StepSequencer::applyTimingLayers(int trackIndex, int stepIndex)
{
    if (trackIndex < 0 || trackIndex >= static_cast<int>(tracks_.size())) return;
    if (stepIndex < 0 || stepIndex >= 16) return;

    StepCell& cell = tracks_[trackIndex].steps[stepIndex];
    TimingRole role = tracks_[trackIndex].timingRole;

    // Clear existing offset
    cell.timingOffset = 0.0f;

    // 1. Apply enhanced swing
    cell.timingOffset += getSwingOffset(stepIndex);

    // 2. Apply role timing (Pocket/Push/Pull)
    float roleOffset = 0.0f;
    switch (role)
    {
        case TimingRole::Pocket:
            roleOffset = roleTimingParams_.pocketOffset;
            break;
        case TimingRole::Push:
        {
            // Slightly exaggerate push on offbeats for more groove
            float pushMultiplier = (stepIndex % 2 == 1) ? 1.2f : 1.0f;
            roleOffset = roleTimingParams_.pushOffset * pushMultiplier;
            break;
        }
        case TimingRole::Pull:
        {
            // Slightly exaggerate pull on backbeats (2, 4) for more feel
            float pullMultiplier = (stepIndex % 4 == 2) ? 1.15f : 1.0f;
            roleOffset = roleTimingParams_.pullOffset * pullMultiplier;
            break;
        }
    }
    cell.timingOffset += roleOffset;

    // 3. Update enhanced Dilla drift
    updateDillaDrift(trackIndex, role);

    // 4. Apply Dilla drift
    cell.timingOffset += dillaStates_[trackIndex].drift;

    // 5. Sub-sample accurate timing: convert fraction to samples
    // timingOffset is in fraction of step, multiply by samplesPerStep_
    // This maintains sub-sample precision for scheduling
}

void StepSequencer::processTrack(int trackIndex, float* output, int numSamples)
{
    if (trackIndex < 0 || trackIndex >= static_cast<int>(tracks_.size())) return;

    // Clear output
    std::fill(output, output + numSamples, 0.0f);

    // Process the drum voice for this track
    Track::DrumType type = tracks_[trackIndex].type;

    for (int i = 0; i < numSamples; ++i)
    {
        output[i] = processDrumVoice(type, 0.0f);  // 0 velocity = process existing envelope
    }
}

void StepSequencer::setTrack(int index, const Track& track)
{
    if (index >= 0 && index < static_cast<int>(tracks_.size()))
    {
        tracks_[index] = track;
    }
}

Track StepSequencer::getTrack(int index) const
{
    if (index >= 0 && index < static_cast<int>(tracks_.size()))
    {
        return tracks_[index];
    }
    return Track{};
}

bool StepSequencer::hasActiveVoices() const
{
    // Check if any drum voice is currently playing
    if (kick_.isActive()) return true;
    if (snare_.isActive()) return true;
    if (hihatClosed_.isActive()) return true;
    if (hihatOpen_.isActive()) return true;
    if (clap_.isActive()) return true;
    if (tomLow_.isActive()) return true;
    if (tomMid_.isActive()) return true;
    if (tomHigh_.isActive()) return true;
    if (crash_.isActive()) return true;
    if (ride_.isActive()) return true;
    if (cowbell_.isActive()) return true;
    if (shaker_.isActive()) return true;
    if (tambourine_.isActive()) return true;
    if (percussion_.isActive()) return true;
    if (special_.isActive()) return true;

    return false;
}

float StepSequencer::processDrumVoice(Track::DrumType type, float velocity)
{
    switch (type)
    {
        case Track::DrumType::Kick:
            if (velocity > 0.0f) kick_.trigger(velocity);
            return kick_.processSample();

        case Track::DrumType::Snare:
            if (velocity > 0.0f) snare_.trigger(velocity);
            return snare_.processSample();

        case Track::DrumType::HiHatClosed:
            if (velocity > 0.0f) hihatClosed_.trigger(velocity);
            return hihatClosed_.processSample();

        case Track::DrumType::HiHatOpen:
            if (velocity > 0.0f) hihatOpen_.trigger(velocity);
            return hihatOpen_.processSample();

        case Track::DrumType::Clap:
            if (velocity > 0.0f) clap_.trigger(velocity);
            return clap_.processSample();

        case Track::DrumType::TomLow:
            if (velocity > 0.0f) tomLow_.trigger(velocity);
            return tomLow_.processSample();

        case Track::DrumType::TomMid:
            if (velocity > 0.0f) tomMid_.trigger(velocity);
            return tomMid_.processSample();

        case Track::DrumType::TomHigh:
            if (velocity > 0.0f) tomHigh_.trigger(velocity);
            return tomHigh_.processSample();

        case Track::DrumType::Crash:
            if (velocity > 0.0f) crash_.trigger(velocity);
            return crash_.processSample();

        case Track::DrumType::Ride:
            if (velocity > 0.0f) ride_.trigger(velocity);
            return ride_.processSample();

        case Track::DrumType::Cowbell:
            if (velocity > 0.0f) cowbell_.trigger(velocity);
            return cowbell_.processSample();

        case Track::DrumType::Shaker:
            if (velocity > 0.0f) shaker_.trigger(velocity);
            return shaker_.processSample();

        case Track::DrumType::Tambourine:
            if (velocity > 0.0f) tambourine_.trigger(velocity);
            return tambourine_.processSample();

        case Track::DrumType::Percussion:
            if (velocity > 0.0f) percussion_.trigger(velocity);
            return percussion_.processSample();

        case Track::DrumType::Special:
            if (velocity > 0.0f) special_.trigger(velocity);
            return special_.processSample();

        default:
            return 0.0f;
    }
}

//==============================================================================
// Main Drum Machine Implementation
//==============================================================================

DrumMachinePureDSP::DrumMachinePureDSP()
{
    // Deterministic PRNG - don't seed srand()
}

DrumMachinePureDSP::~DrumMachinePureDSP()
{
}

bool DrumMachinePureDSP::prepare(double sampleRate, int blockSize)
{
    sampleRate_ = sampleRate;
    blockSize_ = blockSize;

    sequencer_.prepare(sampleRate, blockSize);
    sequencer_.setTempo(params_.tempo);
    sequencer_.setPatternLength(static_cast<int>(params_.patternLength));

    // Initialize timing parameters
    RoleTimingParams roleParams;
    roleParams.pocketOffset = params_.pocketOffset;
    roleParams.pushOffset = params_.pushOffset;
    roleParams.pullOffset = params_.pullOffset;
    sequencer_.setRoleTimingParams(roleParams);

    DillaParams dillaParams;
    dillaParams.amount = params_.dillaAmount;
    dillaParams.hatBias = params_.dillaHatBias;
    dillaParams.snareLate = params_.dillaSnareLate;
    dillaParams.kickTight = params_.dillaKickTight;
    dillaParams.maxDrift = params_.dillaMaxDrift;
    sequencer_.setDillaParams(dillaParams);

    return true;
}

void DrumMachinePureDSP::reset()
{
    sequencer_.reset();
}

void DrumMachinePureDSP::process(float** outputs, int numChannels, int numSamples)
{
    // Clear output buffers
    for (int ch = 0; ch < numChannels; ++ch)
    {
        std::fill(outputs[ch], outputs[ch] + numSamples, 0.0f);
    }

    // Create temp buffer for mixing
    float* tempBuffer = new float[numSamples];
    std::fill(tempBuffer, tempBuffer + numSamples, 0.0f);

    // Process each track
    for (int track = 0; track < 16; ++track)
    {
        sequencer_.processTrack(track, tempBuffer, numSamples);

        // Apply track volume and pan
        float volume = params_.trackVolumes[track];
        float pan = 0.5f;  // Default center (could be per-track)

        for (int i = 0; i < numSamples; ++i)
        {
            float sample = tempBuffer[i] * volume * params_.masterVolume;

            // Apply pan
            outputs[0][i] += sample * std::sqrt(1.0f - pan);
            if (numChannels > 1)
            {
                outputs[1][i] += sample * std::sqrt(pan);
            }
        }
    }

    // Advance sequencer
    sequencer_.advance(numSamples);

    delete[] tempBuffer;
}

void DrumMachinePureDSP::handleEvent(const ScheduledEvent& event)
{
    switch (event.type)
    {
        case ScheduledEvent::NOTE_ON:
            // For drum machine, note on can trigger specific drums
            // Map MIDI notes to tracks
            {
                int track = event.data.note.midiNote % 16;
                float velocity = event.data.note.velocity;  // Already normalized (0-1)
                sequencer_.triggerTrack(track, sequencer_.getCurrentStep(), velocity);
            }
            break;

        default:
            break;
    }
}

float DrumMachinePureDSP::getParameter(const char* paramId) const
{
    if (std::strcmp(paramId, "tempo") == 0)
        return params_.tempo;
    if (std::strcmp(paramId, "swing") == 0)
        return params_.swing;
    if (std::strcmp(paramId, "master_volume") == 0)
        return params_.masterVolume;
    if (std::strcmp(paramId, "pattern_length") == 0)
        return params_.patternLength;

    // Role timing parameters
    if (std::strcmp(paramId, "pocket_offset") == 0)
        return params_.pocketOffset;
    if (std::strcmp(paramId, "push_offset") == 0)
        return params_.pushOffset;
    if (std::strcmp(paramId, "pull_offset") == 0)
        return params_.pullOffset;

    // Dilla parameters
    if (std::strcmp(paramId, "dilla_amount") == 0)
        return params_.dillaAmount;
    if (std::strcmp(paramId, "dilla_hat_bias") == 0)
        return params_.dillaHatBias;
    if (std::strcmp(paramId, "dilla_snare_late") == 0)
        return params_.dillaSnareLate;
    if (std::strcmp(paramId, "dilla_kick_tight") == 0)
        return params_.dillaKickTight;
    if (std::strcmp(paramId, "dilla_max_drift") == 0)
        return params_.dillaMaxDrift;

    // Track volumes
    for (int i = 0; i < 16; ++i)
    {
        char name[32];
        std::snprintf(name, sizeof(name), "track_%d_volume", i);
        if (std::strcmp(paramId, name) == 0)
            return params_.trackVolumes[i];
    }

    return 0.0f;
}

void DrumMachinePureDSP::setParameter(const char* paramId, float value)
{
    // Get old value for logging (before change)
    float oldValue = getParameter(paramId);

    if (std::strcmp(paramId, "tempo") == 0)
    {
        params_.tempo = value;
        sequencer_.setTempo(value);
    }
    else if (std::strcmp(paramId, "swing") == 0)
    {
        params_.swing = value;
        sequencer_.setSwing(value);
    }
    else if (std::strcmp(paramId, "master_volume") == 0)
    {
        params_.masterVolume = value;
    }
    else if (std::strcmp(paramId, "pattern_length") == 0)
    {
        params_.patternLength = value;
        sequencer_.setPatternLength(static_cast<int>(value));
    }
    // Role timing parameters
    else if (std::strcmp(paramId, "pocket_offset") == 0)
    {
        params_.pocketOffset = value;
        RoleTimingParams params = sequencer_.getRoleTimingParams();
        params.pocketOffset = value;
        sequencer_.setRoleTimingParams(params);
    }
    else if (std::strcmp(paramId, "push_offset") == 0)
    {
        params_.pushOffset = value;
        RoleTimingParams params = sequencer_.getRoleTimingParams();
        params.pushOffset = value;
        sequencer_.setRoleTimingParams(params);
    }
    else if (std::strcmp(paramId, "pull_offset") == 0)
    {
        params_.pullOffset = value;
        RoleTimingParams params = sequencer_.getRoleTimingParams();
        params.pullOffset = value;
        sequencer_.setRoleTimingParams(params);
    }
    // Dilla parameters
    else if (std::strcmp(paramId, "dilla_amount") == 0)
    {
        params_.dillaAmount = value;
        DillaParams params = sequencer_.getDillaParams();
        params.amount = value;
        sequencer_.setDillaParams(params);
    }
    else if (std::strcmp(paramId, "dilla_hat_bias") == 0)
    {
        params_.dillaHatBias = value;
        DillaParams params = sequencer_.getDillaParams();
        params.hatBias = value;
        sequencer_.setDillaParams(params);
    }
    else if (std::strcmp(paramId, "dilla_snare_late") == 0)
    {
        params_.dillaSnareLate = value;
        DillaParams params = sequencer_.getDillaParams();
        params.snareLate = value;
        sequencer_.setDillaParams(params);
    }
    else if (std::strcmp(paramId, "dilla_kick_tight") == 0)
    {
        params_.dillaKickTight = value;
        DillaParams params = sequencer_.getDillaParams();
        params.kickTight = value;
        sequencer_.setDillaParams(params);
    }
    else if (std::strcmp(paramId, "dilla_max_drift") == 0)
    {
        params_.dillaMaxDrift = value;
        DillaParams params = sequencer_.getDillaParams();
        params.maxDrift = value;
        sequencer_.setDillaParams(params);
    }
    else
    {
        // Track volumes
        for (int i = 0; i < 16; ++i)
        {
            char name[32];
            std::snprintf(name, sizeof(name), "track_%d_volume", i);
            if (std::strcmp(paramId, name) == 0)
            {
                params_.trackVolumes[i] = value;
                break;
            }
        }
    }

    // Log parameter change (shared telemetry infrastructure)
    LOG_PARAMETER_CHANGE("DrumMachine", paramId, oldValue, value);
}

//==============================================================================
// Base Class Preset Interface
//==============================================================================

bool DrumMachinePureDSP::savePreset(char* jsonBuffer, int jsonBufferSize) const
{
    return savePresetEx(jsonBuffer, jsonBufferSize, PRESET_ALL);
}

bool DrumMachinePureDSP::loadPreset(const char* jsonData)
{
    return loadPresetEx(jsonData, PRESET_ALL);
}

//==============================================================================
// Enhanced Preset System with Section Support
//==============================================================================

bool DrumMachinePureDSP::savePresetEx(char* jsonBuffer, int jsonBufferSize, int sections) const
{
    int offset = 0;

    // Write opening brace
    std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "{\n");
    offset = 2;

    // Always write metadata and global parameters
    writeJsonString("version", "1.0.0", jsonBuffer, offset, jsonBufferSize);
    writeJsonString("name", "Drum Machine Preset", jsonBuffer, offset, jsonBufferSize);
    writeJsonString("author", "Schill Instruments", jsonBuffer, offset, jsonBufferSize);
    writeJsonString("category", "Uncategorized", jsonBuffer, offset, jsonBufferSize);
    writeJsonString("creationDate", "2025-01-07", jsonBuffer, offset, jsonBufferSize);

    // Global parameters (always saved)
    std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "  \"parameters\": {\n");
    offset += std::strlen(jsonBuffer + offset);

    writeJsonParameter("tempo", params_.tempo, jsonBuffer, offset, jsonBufferSize);
    writeJsonParameter("swing", params_.swing, jsonBuffer, offset, jsonBufferSize);
    writeJsonParameter("master_volume", params_.masterVolume, jsonBuffer, offset, jsonBufferSize);
    writeJsonParameter("pattern_length", params_.patternLength, jsonBuffer, offset, jsonBufferSize);
    writeJsonParameter("pocket_offset", params_.pocketOffset, jsonBuffer, offset, jsonBufferSize);
    writeJsonParameter("push_offset", params_.pushOffset, jsonBuffer, offset, jsonBufferSize);
    writeJsonParameter("pull_offset", params_.pullOffset, jsonBuffer, offset, jsonBufferSize);
    writeJsonParameter("dilla_amount", params_.dillaAmount, jsonBuffer, offset, jsonBufferSize);
    writeJsonParameter("dilla_hat_bias", params_.dillaHatBias, jsonBuffer, offset, jsonBufferSize);
    writeJsonParameter("dilla_snare_late", params_.dillaSnareLate, jsonBuffer, offset, jsonBufferSize);
    writeJsonParameter("dilla_kick_tight", params_.dillaKickTight, jsonBuffer, offset, jsonBufferSize);
    writeJsonParameter("dilla_max_drift", params_.dillaMaxDrift, jsonBuffer, offset, jsonBufferSize);

    // Remove trailing comma
    if (offset > 2 && jsonBuffer[offset - 1] == ',') offset--;
    std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "\n  },\n");
    offset += std::strlen(jsonBuffer + offset);

    // Pattern section (rhythms)
    if (sections & PRESET_PATTERN)
    {
        std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "  \"pattern\": {\n");
        offset += std::strlen(jsonBuffer + offset);

        std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "    \"tracks\": [\n");
        offset += std::strlen(jsonBuffer + offset);

        for (int trackIdx = 0; trackIdx < 16; ++trackIdx)
        {
            const Track& track = sequencer_.getTrack(trackIdx);

            std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "      {\n");
            offset += std::strlen(jsonBuffer + offset);

            writeJsonParameter("index", trackIdx, jsonBuffer, offset, jsonBufferSize);

            // Write drum type as string
            const char* drumType = "Kick";
            switch (track.type)
            {
                case Track::DrumType::Kick: drumType = "Kick"; break;
                case Track::DrumType::Snare: drumType = "Snare"; break;
                case Track::DrumType::HiHatClosed: drumType = "HiHatClosed"; break;
                case Track::DrumType::HiHatOpen: drumType = "HiHatOpen"; break;
                case Track::DrumType::Clap: drumType = "Clap"; break;
                case Track::DrumType::TomLow: drumType = "TomLow"; break;
                case Track::DrumType::TomMid: drumType = "TomMid"; break;
                case Track::DrumType::TomHigh: drumType = "TomHigh"; break;
                case Track::DrumType::Crash: drumType = "Crash"; break;
                case Track::DrumType::Ride: drumType = "Ride"; break;
                case Track::DrumType::Cowbell: drumType = "Cowbell"; break;
                case Track::DrumType::Shaker: drumType = "Shaker"; break;
                case Track::DrumType::Tambourine: drumType = "Tambourine"; break;
                case Track::DrumType::Percussion: drumType = "Percussion"; break;
                case Track::DrumType::Special: drumType = "Special"; break;
            }
            writeJsonString("type", drumType, jsonBuffer, offset, jsonBufferSize);

            // Write timing role
            const char* timingRole = "Pocket";
            switch (track.timingRole)
            {
                case TimingRole::Pocket: timingRole = "Pocket"; break;
                case TimingRole::Push: timingRole = "Push"; break;
                case TimingRole::Pull: timingRole = "Pull"; break;
            }
            writeJsonString("timing_role", timingRole, jsonBuffer, offset, jsonBufferSize);

            writeJsonParameter("volume", track.volume, jsonBuffer, offset, jsonBufferSize);
            writeJsonParameter("pan", track.pan, jsonBuffer, offset, jsonBufferSize);
            writeJsonParameter("pitch", track.pitch, jsonBuffer, offset, jsonBufferSize);

            // Write steps
            std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "        \"steps\": [");
            offset += std::strlen(jsonBuffer + offset);

            for (int stepIdx = 0; stepIdx < 16; ++stepIdx)
            {
                const StepCell& step = track.steps[stepIdx];

                std::snprintf(jsonBuffer + offset, jsonBufferSize - offset,
                    "{\"active\":%s,\"velocity\":%d,\"probability\":%.3f,\"flam\":%s,\"roll\":%s,\"roll_notes\":%d}",
                    step.active ? "true" : "false",
                    step.velocity,
                    step.probability,
                    step.hasFlam ? "true" : "false",
                    step.isRoll ? "true" : "false",
                    step.rollNotes);

                offset += std::strlen(jsonBuffer + offset);

                if (stepIdx < 15)
                {
                    std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, ",");
                    offset++;
                }
            }

            std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "]\n");
            offset += std::strlen(jsonBuffer + offset);

            // Remove trailing comma before closing track
            if (offset > 2 && jsonBuffer[offset - 2] == ',') offset -= 2;

            std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "      }");
            offset += std::strlen(jsonBuffer + offset);

            if (trackIdx < 15)
            {
                std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, ",\n");
                offset++;
            }
            else
            {
                std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "\n");
                offset++;
            }
        }

        std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "    ]\n");
        offset += std::strlen(jsonBuffer + offset);

        std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "  },\n");
        offset += std::strlen(jsonBuffer + offset);
    }

    // Kit section (drum sounds)
    if (sections & PRESET_KIT)
    {
        std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "  \"kit\": {\n");
        offset += std::strlen(jsonBuffer + offset);

        std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "    \"voices\": {\n");
        offset += std::strlen(jsonBuffer + offset);

        // Kick
        std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "      \"kick\": {\n");
        offset += std::strlen(jsonBuffer + offset);
        writeJsonParameter("pitch", voiceParams_.kickPitch, jsonBuffer, offset, jsonBufferSize);
        writeJsonParameter("decay", voiceParams_.kickDecay, jsonBuffer, offset, jsonBufferSize);
        writeJsonParameter("click", voiceParams_.kickClick, jsonBuffer, offset, jsonBufferSize);
        if (jsonBuffer[offset - 1] == ',') offset--;
        std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "\n      },\n");
        offset += std::strlen(jsonBuffer + offset);

        // Snare
        std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "      \"snare\": {\n");
        offset += std::strlen(jsonBuffer + offset);
        writeJsonParameter("tone", voiceParams_.snareTone, jsonBuffer, offset, jsonBufferSize);
        writeJsonParameter("decay", voiceParams_.snareDecay, jsonBuffer, offset, jsonBufferSize);
        writeJsonParameter("snap", voiceParams_.snareSnap, jsonBuffer, offset, jsonBufferSize);
        if (jsonBuffer[offset - 1] == ',') offset--;
        std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "\n      },\n");
        offset += std::strlen(jsonBuffer + offset);

        // HiHat Closed
        std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "      \"hihat_closed\": {\n");
        offset += std::strlen(jsonBuffer + offset);
        writeJsonParameter("tone", voiceParams_.hihatClosedTone, jsonBuffer, offset, jsonBufferSize);
        writeJsonParameter("decay", voiceParams_.hihatClosedDecay, jsonBuffer, offset, jsonBufferSize);
        writeJsonParameter("metallic", voiceParams_.hihatClosedMetallic, jsonBuffer, offset, jsonBufferSize);
        if (jsonBuffer[offset - 1] == ',') offset--;
        std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "\n      },\n");
        offset += std::strlen(jsonBuffer + offset);

        // HiHat Open
        std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "      \"hihat_open\": {\n");
        offset += std::strlen(jsonBuffer + offset);
        writeJsonParameter("tone", voiceParams_.hihatOpenTone, jsonBuffer, offset, jsonBufferSize);
        writeJsonParameter("decay", voiceParams_.hihatOpenDecay, jsonBuffer, offset, jsonBufferSize);
        writeJsonParameter("metallic", voiceParams_.hihatOpenMetallic, jsonBuffer, offset, jsonBufferSize);
        if (jsonBuffer[offset - 1] == ',') offset--;
        std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "\n      },\n");
        offset += std::strlen(jsonBuffer + offset);

        // Clap
        std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "      \"clap\": {\n");
        offset += std::strlen(jsonBuffer + offset);
        writeJsonParameter("tone", voiceParams_.clapTone, jsonBuffer, offset, jsonBufferSize);
        writeJsonParameter("decay", voiceParams_.clapDecay, jsonBuffer, offset, jsonBufferSize);
        writeJsonParameter("num_impulses", voiceParams_.clapNumImpulses, jsonBuffer, offset, jsonBufferSize);
        if (jsonBuffer[offset - 1] == ',') offset--;
        std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "\n      },\n");
        offset += std::strlen(jsonBuffer + offset);

        // Tom Low
        std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "      \"tom_low\": {\n");
        offset += std::strlen(jsonBuffer + offset);
        writeJsonParameter("pitch", voiceParams_.tomLowPitch, jsonBuffer, offset, jsonBufferSize);
        writeJsonParameter("decay", voiceParams_.tomLowDecay, jsonBuffer, offset, jsonBufferSize);
        writeJsonParameter("tone", voiceParams_.tomLowTone, jsonBuffer, offset, jsonBufferSize);
        if (jsonBuffer[offset - 1] == ',') offset--;
        std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "\n      },\n");
        offset += std::strlen(jsonBuffer + offset);

        // Tom Mid
        std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "      \"tom_mid\": {\n");
        offset += std::strlen(jsonBuffer + offset);
        writeJsonParameter("pitch", voiceParams_.tomMidPitch, jsonBuffer, offset, jsonBufferSize);
        writeJsonParameter("decay", voiceParams_.tomMidDecay, jsonBuffer, offset, jsonBufferSize);
        writeJsonParameter("tone", voiceParams_.tomMidTone, jsonBuffer, offset, jsonBufferSize);
        if (jsonBuffer[offset - 1] == ',') offset--;
        std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "\n      },\n");
        offset += std::strlen(jsonBuffer + offset);

        // Tom High
        std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "      \"tom_high\": {\n");
        offset += std::strlen(jsonBuffer + offset);
        writeJsonParameter("pitch", voiceParams_.tomHighPitch, jsonBuffer, offset, jsonBufferSize);
        writeJsonParameter("decay", voiceParams_.tomHighDecay, jsonBuffer, offset, jsonBufferSize);
        writeJsonParameter("tone", voiceParams_.tomHighTone, jsonBuffer, offset, jsonBufferSize);
        if (jsonBuffer[offset - 1] == ',') offset--;
        std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "\n      },\n");
        offset += std::strlen(jsonBuffer + offset);

        // Crash
        std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "      \"crash\": {\n");
        offset += std::strlen(jsonBuffer + offset);
        writeJsonParameter("tone", voiceParams_.crashTone, jsonBuffer, offset, jsonBufferSize);
        writeJsonParameter("decay", voiceParams_.crashDecay, jsonBuffer, offset, jsonBufferSize);
        writeJsonParameter("metallic", voiceParams_.crashMetallic, jsonBuffer, offset, jsonBufferSize);
        if (jsonBuffer[offset - 1] == ',') offset--;
        std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "\n      },\n");
        offset += std::strlen(jsonBuffer + offset);

        // Ride
        std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "      \"ride\": {\n");
        offset += std::strlen(jsonBuffer + offset);
        writeJsonParameter("tone", voiceParams_.rideTone, jsonBuffer, offset, jsonBufferSize);
        writeJsonParameter("decay", voiceParams_.rideDecay, jsonBuffer, offset, jsonBufferSize);
        writeJsonParameter("metallic", voiceParams_.rideMetallic, jsonBuffer, offset, jsonBufferSize);
        if (jsonBuffer[offset - 1] == ',') offset--;
        std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "\n      },\n");
        offset += std::strlen(jsonBuffer + offset);

        // Cowbell
        std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "      \"cowbell\": {\n");
        offset += std::strlen(jsonBuffer + offset);
        writeJsonParameter("pitch", voiceParams_.cowbellPitch, jsonBuffer, offset, jsonBufferSize);
        writeJsonParameter("decay", voiceParams_.cowbellDecay, jsonBuffer, offset, jsonBufferSize);
        writeJsonParameter("tone", voiceParams_.cowbellTone, jsonBuffer, offset, jsonBufferSize);
        if (jsonBuffer[offset - 1] == ',') offset--;
        std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "\n      },\n");
        offset += std::strlen(jsonBuffer + offset);

        // Shaker
        std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "      \"shaker\": {\n");
        offset += std::strlen(jsonBuffer + offset);
        writeJsonParameter("tone", voiceParams_.shakerTone, jsonBuffer, offset, jsonBufferSize);
        writeJsonParameter("decay", voiceParams_.shakerDecay, jsonBuffer, offset, jsonBufferSize);
        writeJsonParameter("metallic", voiceParams_.shakerMetallic, jsonBuffer, offset, jsonBufferSize);
        if (jsonBuffer[offset - 1] == ',') offset--;
        std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "\n      },\n");
        offset += std::strlen(jsonBuffer + offset);

        // Tambourine
        std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "      \"tambourine\": {\n");
        offset += std::strlen(jsonBuffer + offset);
        writeJsonParameter("tone", voiceParams_.tambourineTone, jsonBuffer, offset, jsonBufferSize);
        writeJsonParameter("decay", voiceParams_.tambourineDecay, jsonBuffer, offset, jsonBufferSize);
        writeJsonParameter("metallic", voiceParams_.tambourineMetallic, jsonBuffer, offset, jsonBufferSize);
        if (jsonBuffer[offset - 1] == ',') offset--;
        std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "\n      },\n");
        offset += std::strlen(jsonBuffer + offset);

        // Percussion
        std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "      \"percussion\": {\n");
        offset += std::strlen(jsonBuffer + offset);
        writeJsonParameter("pitch", voiceParams_.percussionPitch, jsonBuffer, offset, jsonBufferSize);
        writeJsonParameter("decay", voiceParams_.percussionDecay, jsonBuffer, offset, jsonBufferSize);
        writeJsonParameter("tone", voiceParams_.percussionTone, jsonBuffer, offset, jsonBufferSize);
        if (jsonBuffer[offset - 1] == ',') offset--;
        std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "\n      },\n");
        offset += std::strlen(jsonBuffer + offset);

        // Special
        std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "      \"special\": {\n");
        offset += std::strlen(jsonBuffer + offset);
        writeJsonParameter("tone", voiceParams_.specialTone, jsonBuffer, offset, jsonBufferSize);
        writeJsonParameter("decay", voiceParams_.specialDecay, jsonBuffer, offset, jsonBufferSize);
        writeJsonParameter("snap", voiceParams_.specialSnap, jsonBuffer, offset, jsonBufferSize);
        if (jsonBuffer[offset - 1] == ',') offset--;
        std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "\n      }\n");
        offset += std::strlen(jsonBuffer + offset);

        std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "    }\n");
        offset += std::strlen(jsonBuffer + offset);

        std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "  }\n");
        offset += std::strlen(jsonBuffer + offset);
    }

    // Closing brace
    std::snprintf(jsonBuffer + offset, jsonBufferSize - offset, "}");

    return true;
}

bool DrumMachinePureDSP::loadPresetEx(const char* jsonData, int sections)
{
    (void)sections;  // Ignore sections parameter for now
    double value;

    if (parseJsonParameter(jsonData, "tempo", value))
    {
        params_.tempo = static_cast<float>(value);
        sequencer_.setTempo(params_.tempo);
    }
    if (parseJsonParameter(jsonData, "swing", value))
    {
        params_.swing = static_cast<float>(value);
        sequencer_.setSwing(params_.swing);
    }
    if (parseJsonParameter(jsonData, "master_volume", value))
        params_.masterVolume = static_cast<float>(value);
    if (parseJsonParameter(jsonData, "pattern_length", value))
    {
        params_.patternLength = static_cast<float>(value);
        sequencer_.setPatternLength(static_cast<int>(params_.patternLength));
    }

    // Role timing parameters
    RoleTimingParams roleParams = sequencer_.getRoleTimingParams();
    if (parseJsonParameter(jsonData, "pocket_offset", value))
    {
        params_.pocketOffset = static_cast<float>(value);
        roleParams.pocketOffset = params_.pocketOffset;
    }
    if (parseJsonParameter(jsonData, "push_offset", value))
    {
        params_.pushOffset = static_cast<float>(value);
        roleParams.pushOffset = params_.pushOffset;
    }
    if (parseJsonParameter(jsonData, "pull_offset", value))
    {
        params_.pullOffset = static_cast<float>(value);
        roleParams.pullOffset = params_.pullOffset;
    }
    sequencer_.setRoleTimingParams(roleParams);

    // Dilla parameters
    DillaParams dillaParams = sequencer_.getDillaParams();
    if (parseJsonParameter(jsonData, "dilla_amount", value))
    {
        params_.dillaAmount = static_cast<float>(value);
        dillaParams.amount = params_.dillaAmount;
    }
    if (parseJsonParameter(jsonData, "dilla_hat_bias", value))
    {
        params_.dillaHatBias = static_cast<float>(value);
        dillaParams.hatBias = params_.dillaHatBias;
    }
    if (parseJsonParameter(jsonData, "dilla_snare_late", value))
    {
        params_.dillaSnareLate = static_cast<float>(value);
        dillaParams.snareLate = params_.dillaSnareLate;
    }
    if (parseJsonParameter(jsonData, "dilla_kick_tight", value))
    {
        params_.dillaKickTight = static_cast<float>(value);
        dillaParams.kickTight = params_.dillaKickTight;
    }
    if (parseJsonParameter(jsonData, "dilla_max_drift", value))
    {
        params_.dillaMaxDrift = static_cast<float>(value);
        dillaParams.maxDrift = params_.dillaMaxDrift;
    }
    sequencer_.setDillaParams(dillaParams);

    return true;
}

int DrumMachinePureDSP::getActiveVoiceCount() const
{
    // Check if any drum voices are actively playing
    return sequencer_.hasActiveVoices() ? 1 : 0;
}

bool DrumMachinePureDSP::writeJsonParameter(const char* name, double value,
                                              char* buffer, int& offset,
                                              int bufferSize) const
{
    int remaining = bufferSize - offset;
    if (remaining < 50) return false;

    int written = std::snprintf(buffer + offset, remaining,
                               "\"%s\":%.6f,",
                               name, value);

    if (written < 0 || written >= remaining) return false;

    offset += written;
    return true;
}

bool DrumMachinePureDSP::writeJsonString(const char* name, const char* value,
                                         char* buffer, int& offset,
                                         int bufferSize) const
{
    int remaining = bufferSize - offset;
    if (remaining < 50) return false;

    int written = std::snprintf(buffer + offset, remaining,
                               "\"%s\":\"%s\",",
                               name, value);

    if (written < 0 || written >= remaining) return false;

    offset += written;
    return true;
}

bool DrumMachinePureDSP::parseJsonParameter(const char* json, const char* param,
                                              double& value) const
{
    const char* search = json;
    char pattern[100];
    std::snprintf(pattern, sizeof(pattern), "\"%s\":", param);

    const char* found = std::strstr(search, pattern);
    if (!found) return false;

    found += std::strlen(pattern);
    value = std::atof(found);

    return true;
}

//==============================================================================
// Drill Mode Implementation (Aphex Twin / Drill'n'Bass)
//==============================================================================

bool StepSequencer::trackWantsDrill(Track::DrumType type) const
{
    // Default mapping: snares, hats, percussion use drill
    // Kick stays mostly stable
    switch (type)
    {
        case Track::DrumType::Snare:
        case Track::DrumType::HiHatClosed:
        case Track::DrumType::HiHatOpen:
        case Track::DrumType::Clap:
        case Track::DrumType::Shaker:
        case Track::DrumType::Tambourine:
        case Track::DrumType::Percussion:
            return true;
        default:
            return false;
    }
}

//==============================================================================
// Drill-Aware Pattern Generation
//==============================================================================

bool StepSequencer::cellWantsDrill(const StepCell& cell,
                                    const DrillMode& drill,
                                    float globalDrillAmount) const
{
    // Check if drill is globally enabled
    if (!drill.enabled || globalDrillAmount <= 0.001f)
        return false;

    // Check drill intent for this specific step
    switch (cell.drillIntent)
    {
        case DrillIntent::None:
            // Never drill - always use groove timing
            return false;

        case DrillIntent::Optional:
            // May drill if amount > 25% (musically sensible threshold)
            return globalDrillAmount > 0.25f;

        case DrillIntent::Emphasize:
            // Prefer drill here (fills, accents) - lower threshold
            return globalDrillAmount > 0.05f;
    }

    return false;
}

//==============================================================================
// Automatic Drill Fills
//==============================================================================

bool StepSequencer::isFillStep(int stepIndex,
                                int stepsPerBar,
                                const DrillFillPolicy& policy) const
{
    if (!policy.enabled)
        return false;

    // Fill happens at the end of the bar
    return stepIndex >= (stepsPerBar - policy.fillLengthSteps);
}

void StepSequencer::updateFillState(const DrillFillPolicy& policy)
{
    if (!policy.enabled)
    {
        drillFillState_.active = false;
        return;
    }

    // Determine if fill should trigger this bar
    drillFillState_.active = (drillRng_.next01() < policy.triggerChance);
}

//==============================================================================
// Drill ↔ Silence Gating
//==============================================================================

bool StepSequencer::shouldGateStep(const DrillGatePolicy& policy)
{
    if (!policy.enabled)
        return false;

    // Already in a silent run - continue it
    if (drillGateState_.silentStepsRemaining > 0)
    {
        drillGateState_.silentStepsRemaining--;
        return true;
    }

    // Possibly start a new silent run
    if (drillRng_.next01() < policy.silenceChance)
    {
        drillGateState_.silentStepsRemaining =
            drillRng_.rangeInt(policy.minSilentSteps,
                             policy.maxSilentSteps);
        return true;
    }

    return false;
}

//==============================================================================
// Bar Tracking for Automation
//==============================================================================

void StepSequencer::updateBarIndex()
{
    // Calculate bar index from current step
    // Assuming 16 steps per bar (4/4 time at 16th note resolution)
    currentBar_ = currentStep_ / getStepsPerBar();
}


int StepSequencer::chooseGridDivisor(DrillGrid grid)
{
    switch (grid)
    {
        case DrillGrid::Straight:      return 0; // special case (use burstCount)
        case DrillGrid::Triplet:       return 3;
        case DrillGrid::Quintuplet:    return 5;
        case DrillGrid::Septuplet:     return 7;
        case DrillGrid::RandomPrime: {
            // Weighted toward 5/7, occasional 11
            const float r = drillRng_.next01();
            if (r < 0.45f) return 5;
            if (r < 0.90f) return 7;
            return 11;
        }
    }
    return 0;
}

void StepSequencer::scheduleMicroBurst(int trackIndex, const StepCell& cell,
                                       double stepStartSeconds, double stepDurationSeconds,
                                       float effectiveDrillAmount)
{
    if (trackIndex < 0 || trackIndex >= static_cast<int>(tracks_.size())) return;

    Track& track = tracks_[trackIndex];

    // Get drill mode (per-track override or global)
    DrillMode drill = track.drillOverride.useOverride ? track.drillOverride.drill : drillMode_;

    // Use effective drill amount if provided (from automation/fill/gate)
    if (effectiveDrillAmount >= 0.0f)
    {
        drill.amount = std::max(0.0f, std::min(1.0f, effectiveDrillAmount));
    }

    // If drill disabled or amount ~0, schedule single hit
    const float amt = std::max(0.0f, std::min(1.0f, drill.amount));
    if (!drill.enabled || amt <= 0.0001f || drill.maxBurst <= 1)
    {
        // Single hit - use existing trigger mechanism
        // Convert timingOffset from fraction to sample delay
        int sampleDelay = static_cast<int>(cell.timingOffset * samplesPerStep_);
        if (sampleDelay >= 0 && sampleDelay < static_cast<int>(samplesPerStep_))
        {
            // Safety check for single hit
            if (microHitsThisBlock_ < kMaxMicroHitsPerBlock)
            {
                triggerTrack(trackIndex, 0, cell.velocity / 127.0f);
                microHitsThisBlock_++;
            }
        }
        return;
    }

    // Apply temporalAggression macro control (scales burst count, chaos, mutation, grid randomness)
    const float agg = std::max(0.0f, std::min(1.0f, drill.temporalAggression));

    // Determine burst count (mutating per hit if desired)
    int burstCount = std::max(1, drill.minBurst);
    {
        const int lo = std::max(1, drill.minBurst);
        const int hi = std::max(lo, drill.maxBurst);

        // Base burst count scaled by amt and temporalAggression
        const float effectiveAmt = amt * agg;
        const float scaled = static_cast<float>(lo) + effectiveAmt * static_cast<float>(hi - lo);
        burstCount = static_cast<int>(std::round(scaled));

        // Optional mutation (scaled by temporalAggression)
        const float scaledMutation = std::max(0.0f, std::min(1.0f, drill.mutationRate * agg));
        if (drillRng_.next01() < scaledMutation * effectiveAmt)
        {
            burstCount = drillRng_.rangeInt(lo, hi);
        }
        burstCount = std::max(1, burstCount);
    }

    // Get per-cell drill params or use defaults from drill mode
    const float cellChaos = cell.useDrill ? cell.burstChaos : drill.chaos;
    const float cellDropout = cell.useDrill ? cell.burstDropout : drill.dropout;
    const int cellBurstCount = cell.useDrill ? cell.burstCount : burstCount;

    // Compute how wide the burst spans inside this step
    const double span = stepDurationSeconds * std::max(0.0, std::min(1.0, static_cast<double>(drill.spread)));

    // Choose grid divisor
    const int gridDiv = chooseGridDivisor(drill.grid);

    // Effective "slots" for placement
    const int slots = (gridDiv == 0) ? cellBurstCount : gridDiv;

    // Chaos amount in seconds (scaled by amt and temporalAggression)
    const float scaledChaos = std::max(0.0f, std::min(1.0f, static_cast<float>(cellChaos) * agg));
    const double chaosSec = static_cast<double>(scaledChaos * amt) * (span * 0.35);

    // Velocity shaping
    const float baseVel = static_cast<float>(cell.velocity) / 127.0f;
    const float decay = std::max(0.0f, std::min(0.95f, static_cast<float>(drill.velDecay))) * amt;

    // Schedule micro-hits
    for (int i = 0; i < cellBurstCount; ++i)
    {
        // Safety check: prevent audio thread DOS
        if (microHitsThisBlock_ >= kMaxMicroHitsPerBlock)
        {
            // Drop remaining micro-hits if we've exceeded the limit
            break;
        }

        // Dropout: chance to skip this micro-hit (scaled by amt)
        if (drillRng_.next01() < std::max(0.0f, std::min(1.0f, static_cast<float>(cellDropout))) * amt)
            continue;

        // Map micro-hit i -> slot index (0..slots-1)
        float pos01 = 0.0f;
        if (cellBurstCount > 1)
        {
            pos01 = static_cast<float>(i) / static_cast<float>(cellBurstCount - 1);
        }

        int slotIndex = 0;
        if (slots > 1)
        {
            // distribute evenly across slots
            slotIndex = static_cast<int>(std::round(pos01 * static_cast<float>(slots - 1)));
            slotIndex = std::max(0, std::min(slots - 1, slotIndex));
        }

        // Base time within span
        const double slotPos01 = (slots <= 1) ? 0.0 : static_cast<double>(slotIndex) / static_cast<double>(slots - 1);
        double t = stepStartSeconds + slotPos01 * span;

        // Chaos perturbation: small time jitter within burst
        t += static_cast<double>(drillRng_.nextSigned()) * chaosSec;

        // Keep within the step window
        const double minT = stepStartSeconds;
        const double maxT = stepStartSeconds + stepDurationSeconds;
        t = std::max(minT, std::min(maxT, t));

        // Convert time to sample position within step
        double relativePos = t - stepStartSeconds;
        float timingOffsetFraction = static_cast<float>(relativePos / stepDurationSeconds);

        // Velocity decay (exponential-ish)
        float v = baseVel;
        if (cellBurstCount > 1)
        {
            // decay curve: v *= (1 - decay) ^ i
            const float d = std::max(0.0f, std::min(0.95f, decay));
            v *= std::pow(1.0f - d, static_cast<float>(i));
        }

        // Accent flip (optional): random spikes/dips
        if (drillRng_.next01() < std::max(0.0f, std::min(1.0f, drill.accentFlip)) * amt)
        {
            const float spike = 0.8f + 0.6f * drillRng_.next01(); // 0.8..1.4
            v = std::max(0.0f, std::min(1.0f, v * spike));
        }

        // Convert velocity back to MIDI range
        uint8_t midiVel = static_cast<uint8_t>(std::max(0.0f, std::min(1.0f, v)) * 127.0f);

        // Create temporary cell for this micro-hit
        StepCell microCell = cell;
        microCell.timingOffset = timingOffsetFraction;
        microCell.velocity = midiVel;
        microCell.useDrill = false; // Prevent infinite recursion

        // Trigger the micro-hit
        triggerTrack(trackIndex, 0, v);

        // Increment safety counter
        microHitsThisBlock_++;
    }
}

// Drill Preset Implementations

DrillMode StepSequencer::presetDrillLite()
{
    DrillMode d;
    d.enabled = true;
    d.amount = 0.4f;
    d.minBurst = 1;
    d.maxBurst = 4;
    d.spread = 0.35f;
    d.chaos = 0.12f;
    d.dropout = 0.05f;
    d.velDecay = 0.35f;
    d.accentFlip = 0.05f;
    d.mutationRate = 0.15f;
    d.grid = DrillGrid::Triplet;
    d.transitionBeats = 0.5f;
    return d;
}

DrillMode StepSequencer::presetAphexSnareHell()
{
    DrillMode d;
    d.enabled = true;
    d.amount = 0.75f;
    d.minBurst = 3;
    d.maxBurst = 12;
    d.spread = 0.55f;
    d.chaos = 0.25f;
    d.dropout = 0.12f;
    d.velDecay = 0.45f;
    d.accentFlip = 0.10f;
    d.mutationRate = 0.35f;
    d.grid = DrillGrid::RandomPrime;
    d.transitionBeats = 0.75f;
    return d;
}

DrillMode StepSequencer::presetVenetianMode()
{
    DrillMode d;
    d.enabled = true;
    d.amount = 0.95f;
    d.minBurst = 6;
    d.maxBurst = 16;
    d.spread = 0.75f;
    d.chaos = 0.35f;
    d.dropout = 0.18f;
    d.velDecay = 0.55f;
    d.accentFlip = 0.15f;
    d.mutationRate = 0.55f;
    d.grid = DrillGrid::RandomPrime;
    d.transitionBeats = 1.0f;
    return d;
}

//==============================================================================
// 17 Additional Drill Presets
//==============================================================================

// A) Transitional / Musical (Groove ↔ Drill bridges)

DrillMode StepSequencer::presetGlitchAccent()
{
    DrillMode d;
    d.enabled = true;
    d.amount = 0.25f;
    d.mutationRate = 0.10f;
    d.dropout = 0.02f;
    d.chaos = 0.08f;
    d.spread = 0.25f;
    d.velDecay = 0.30f;
    d.accentFlip = 0.03f;
    d.minBurst = 1;
    d.maxBurst = 3;
    d.grid = DrillGrid::Straight;
    d.transitionBeats = 0.5f;
    return d;
}

DrillMode StepSequencer::presetBrokenGroove()
{
    DrillMode d;
    d.enabled = true;
    d.amount = 0.35f;
    d.mutationRate = 0.20f;
    d.dropout = 0.05f;
    d.chaos = 0.12f;
    d.spread = 0.30f;
    d.velDecay = 0.35f;
    d.accentFlip = 0.05f;
    d.minBurst = 1;
    d.maxBurst = 4;
    d.grid = DrillGrid::Triplet;
    d.transitionBeats = 0.75f;
    return d;
}

DrillMode StepSequencer::presetNeoIDMFill()
{
    DrillMode d;
    d.enabled = true;
    d.amount = 0.45f;
    d.mutationRate = 0.25f;
    d.dropout = 0.05f;
    d.chaos = 0.18f;
    d.spread = 0.40f;
    d.velDecay = 0.40f;
    d.accentFlip = 0.06f;
    d.minBurst = 2;
    d.maxBurst = 5;
    d.grid = DrillGrid::Quintuplet;
    d.transitionBeats = 0.75f;
    return d;
}

DrillMode StepSequencer::presetGhostMachinery()
{
    DrillMode d;
    d.enabled = true;
    d.amount = 0.30f;
    d.mutationRate = 0.15f;
    d.dropout = 0.10f;
    d.chaos = 0.15f;
    d.spread = 0.25f;
    d.velDecay = 0.45f;
    d.accentFlip = 0.04f;
    d.minBurst = 1;
    d.maxBurst = 4;
    d.grid = DrillGrid::Straight;
    d.transitionBeats = 0.5f;
    return d;
}

// B) Aphex-Style Signature Presets

DrillMode StepSequencer::presetAphexMicrofracture()
{
    DrillMode d;
    d.enabled = true;
    d.amount = 0.55f;
    d.mutationRate = 0.30f;
    d.dropout = 0.10f;
    d.chaos = 0.22f;
    d.spread = 0.45f;
    d.velDecay = 0.45f;
    d.accentFlip = 0.08f;
    d.minBurst = 3;
    d.maxBurst = 8;
    d.grid = DrillGrid::Quintuplet;
    d.transitionBeats = 0.75f;
    return d;
}

DrillMode StepSequencer::presetWindowlickerSnare()
{
    DrillMode d;
    d.enabled = true;
    d.amount = 0.70f;
    d.mutationRate = 0.35f;
    d.dropout = 0.12f;
    d.chaos = 0.28f;
    d.spread = 0.55f;
    d.velDecay = 0.50f;
    d.accentFlip = 0.10f;
    d.minBurst = 4;
    d.maxBurst = 12;
    d.grid = DrillGrid::RandomPrime;
    d.transitionBeats = 1.0f;
    return d;
}

DrillMode StepSequencer::presetPolygonWindow()
{
    DrillMode d;
    d.enabled = true;
    d.amount = 0.65f;
    d.mutationRate = 0.25f;
    d.dropout = 0.05f;
    d.chaos = 0.15f;
    d.spread = 0.60f;
    d.velDecay = 0.30f;
    d.accentFlip = 0.04f;
    d.minBurst = 3;
    d.maxBurst = 7;
    d.grid = DrillGrid::Septuplet;
    d.transitionBeats = 0.75f;
    return d;
}

DrillMode StepSequencer::presetClockDesync()
{
    DrillMode d;
    d.enabled = true;
    d.amount = 0.60f;
    d.mutationRate = 0.40f;
    d.dropout = 0.08f;
    d.chaos = 0.35f;
    d.spread = 0.50f;
    d.velDecay = 0.55f;
    d.accentFlip = 0.12f;
    d.minBurst = 2;
    d.maxBurst = 10;
    d.grid = DrillGrid::RandomPrime;
    d.transitionBeats = 0.75f;
    return d;
}

// C) Drill'n'Bass / Venetian Snares Energy

DrillMode StepSequencer::presetDrillNBassCore()
{
    DrillMode d;
    d.enabled = true;
    d.amount = 0.75f;
    d.mutationRate = 0.30f;
    d.dropout = 0.10f;
    d.chaos = 0.25f;
    d.spread = 0.55f;
    d.velDecay = 0.45f;
    d.accentFlip = 0.08f;
    d.minBurst = 4;
    d.maxBurst = 10;
    d.grid = DrillGrid::Triplet;
    d.transitionBeats = 0.75f;
    return d;
}

DrillMode StepSequencer::presetVenetianGhosts()
{
    DrillMode d;
    d.enabled = true;
    d.amount = 0.80f;
    d.mutationRate = 0.45f;
    d.dropout = 0.18f;
    d.chaos = 0.30f;
    d.spread = 0.60f;
    d.velDecay = 0.50f;
    d.accentFlip = 0.12f;
    d.minBurst = 5;
    d.maxBurst = 14;
    d.grid = DrillGrid::RandomPrime;
    d.transitionBeats = 1.0f;
    return d;
}

DrillMode StepSequencer::presetAmenShredder()
{
    DrillMode d;
    d.enabled = true;
    d.amount = 0.85f;
    d.mutationRate = 0.50f;
    d.dropout = 0.15f;
    d.chaos = 0.35f;
    d.spread = 0.70f;
    d.velDecay = 0.55f;
    d.accentFlip = 0.15f;
    d.minBurst = 6;
    d.maxBurst = 16;
    d.grid = DrillGrid::RandomPrime;
    d.transitionBeats = 1.0f;
    return d;
}

DrillMode StepSequencer::presetOverclockedSnare()
{
    DrillMode d;
    d.enabled = true;
    d.amount = 0.90f;
    d.mutationRate = 0.35f;
    d.dropout = 0.05f;
    d.chaos = 0.20f;
    d.spread = 0.80f;
    d.velDecay = 0.40f;
    d.accentFlip = 0.05f;
    d.minBurst = 8;
    d.maxBurst = 16;
    d.grid = DrillGrid::Quintuplet;
    d.transitionBeats = 0.75f;
    return d;
}

// D) Noise / Experimental / Brutal

DrillMode StepSequencer::presetTimeGrinder()
{
    DrillMode d;
    d.enabled = true;
    d.amount = 0.95f;
    d.mutationRate = 0.55f;
    d.dropout = 0.20f;
    d.chaos = 0.40f;
    d.spread = 0.75f;
    d.velDecay = 0.65f;
    d.accentFlip = 0.18f;
    d.minBurst = 8;
    d.maxBurst = 20;
    d.grid = DrillGrid::RandomPrime;
    d.transitionBeats = 1.25f;
    return d;
}

DrillMode StepSequencer::presetDigitalSeizure()
{
    DrillMode d;
    d.enabled = true;
    d.amount = 1.00f;
    d.mutationRate = 0.65f;
    d.dropout = 0.25f;
    d.chaos = 0.45f;
    d.spread = 0.85f;
    d.velDecay = 0.70f;
    d.accentFlip = 0.20f;
    d.minBurst = 10;
    d.maxBurst = 24;
    d.grid = DrillGrid::RandomPrime;
    d.transitionBeats = 1.5f;
    return d;
}

DrillMode StepSequencer::presetStaticEngine()
{
    DrillMode d;
    d.enabled = true;
    d.amount = 0.85f;
    d.mutationRate = 0.40f;
    d.dropout = 0.35f;
    d.chaos = 0.30f;
    d.spread = 0.65f;
    d.velDecay = 0.50f;
    d.accentFlip = 0.10f;
    d.minBurst = 3;
    d.maxBurst = 12;
    d.grid = DrillGrid::Straight;
    d.transitionBeats = 0.75f;
    return d;
}

// E) Rhythmic Control / Utility

DrillMode StepSequencer::presetRatchetBuilder()
{
    DrillMode d;
    d.enabled = true;
    d.amount = 0.60f;
    d.mutationRate = 0.10f;
    d.dropout = 0.00f;
    d.chaos = 0.05f;
    d.spread = 0.50f;
    d.velDecay = 0.35f;
    d.accentFlip = 0.00f;
    d.minBurst = 2;
    d.maxBurst = 8;
    d.grid = DrillGrid::Straight;
    d.transitionBeats = 0.5f;
    return d;
}

DrillMode StepSequencer::presetFillGenerator()
{
    DrillMode d;
    d.enabled = true;
    d.amount = 0.50f;
    d.mutationRate = 0.60f;
    d.dropout = 0.05f;
    d.chaos = 0.20f;
    d.spread = 0.45f;
    d.velDecay = 0.40f;
    d.accentFlip = 0.08f;
    d.minBurst = 2;
    d.maxBurst = 10;
    d.grid = DrillGrid::Triplet;
    d.transitionBeats = 0.75f;
    return d;
}

//==============================================================================
// IDM Macro Presets (Behavioral Identities)
//==============================================================================

IdmMacroPreset StepSequencer::idmMacroGhostFill()
{
    // Subtle, Aphex-adjacent, safe default
    IdmMacroPreset p;
    p.name = "Ghost Fill";

    // Drill
    p.drill.enabled = true;
    p.drill.amount = 0.35f;
    p.drill.mutationRate = 0.15f;
    p.drill.dropout = 0.03f;
    p.drill.chaos = 0.10f;
    p.drill.spread = 0.30f;
    p.drill.velDecay = 0.35f;
    p.drill.accentFlip = 0.03f;
    p.drill.temporalAggression = 1.0f;
    p.drill.minBurst = 1;
    p.drill.maxBurst = 4;
    p.drill.grid = DrillGrid::Triplet;
    p.drill.transitionBeats = 0.5f;

    // Fill
    p.fill.enabled = true;
    p.fill.fillLengthSteps = 2;
    p.fill.triggerChance = 0.55f;
    p.fill.fillAmount = 0.45f;
    p.fill.decayPerStep = 0.25f;

    // Gate (disabled)
    p.gate.enabled = false;
    p.gate.silenceChance = 0.0f;
    p.gate.burstChance = 0.0f;
    p.gate.minSilentSteps = 1;
    p.gate.maxSilentSteps = 1;

    return p;
}

IdmMacroPreset StepSequencer::idmMacroSnareHallucination()
{
    // Silence → explosion → silence
    IdmMacroPreset p;
    p.name = "Snare Hallucination";

    // Drill
    p.drill.enabled = true;
    p.drill.amount = 0.75f;
    p.drill.mutationRate = 0.40f;
    p.drill.dropout = 0.10f;
    p.drill.chaos = 0.30f;
    p.drill.spread = 0.55f;
    p.drill.velDecay = 0.50f;
    p.drill.accentFlip = 0.10f;
    p.drill.temporalAggression = 0.75f;
    p.drill.minBurst = 4;
    p.drill.maxBurst = 12;
    p.drill.grid = DrillGrid::RandomPrime;
    p.drill.transitionBeats = 0.5f;

    // Fill
    p.fill.enabled = true;
    p.fill.fillLengthSteps = 2;
    p.fill.triggerChance = 0.85f;
    p.fill.fillAmount = 0.75f;
    p.fill.decayPerStep = 0.15f;

    // Gate
    p.gate.enabled = true;
    p.gate.silenceChance = 0.30f;
    p.gate.burstChance = 0.70f;
    p.gate.minSilentSteps = 1;
    p.gate.maxSilentSteps = 3;

    return p;
}

IdmMacroPreset StepSequencer::idmMacroBrokenTransport()
{
    // Time disappears, machine stutters
    IdmMacroPreset p;
    p.name = "Broken Transport";

    // Drill
    p.drill.enabled = true;
    p.drill.amount = 0.85f;
    p.drill.mutationRate = 0.45f;
    p.drill.dropout = 0.20f;
    p.drill.chaos = 0.35f;
    p.drill.spread = 0.65f;
    p.drill.velDecay = 0.55f;
    p.drill.accentFlip = 0.15f;
    p.drill.temporalAggression = 1.0f;
    p.drill.minBurst = 6;
    p.drill.maxBurst = 16;
    p.drill.grid = DrillGrid::RandomPrime;
    p.drill.transitionBeats = 0.5f;

    // Fill
    p.fill.enabled = true;
    p.fill.fillLengthSteps = 3;
    p.fill.triggerChance = 0.65f;
    p.fill.fillAmount = 0.80f;
    p.fill.decayPerStep = 0.20f;

    // Gate
    p.gate.enabled = true;
    p.gate.silenceChance = 0.45f;
    p.gate.burstChance = 0.55f;
    p.gate.minSilentSteps = 2;
    p.gate.maxSilentSteps = 4;

    return p;
}

IdmMacroPreset StepSequencer::idmMacroVenetianCollapse()
{
    // Maximalist drill'n'bass
    IdmMacroPreset p;
    p.name = "Venetian Collapse";

    // Drill
    p.drill.enabled = true;
    p.drill.amount = 1.00f;
    p.drill.mutationRate = 0.60f;
    p.drill.dropout = 0.15f;
    p.drill.chaos = 0.45f;
    p.drill.spread = 0.80f;
    p.drill.velDecay = 0.65f;
    p.drill.accentFlip = 0.20f;
    p.drill.temporalAggression = 1.25f;
    p.drill.minBurst = 10;
    p.drill.maxBurst = 24;
    p.drill.grid = DrillGrid::RandomPrime;
    p.drill.transitionBeats = 0.5f;

    // Fill
    p.fill.enabled = true;
    p.fill.fillLengthSteps = 4;
    p.fill.triggerChance = 0.90f;
    p.fill.fillAmount = 1.00f;
    p.fill.decayPerStep = 0.10f;

    // Gate
    p.gate.enabled = true;
    p.gate.silenceChance = 0.35f;
    p.gate.burstChance = 0.80f;
    p.gate.minSilentSteps = 1;
    p.gate.maxSilentSteps = 5;

    return p;
}

IdmMacroPreset StepSequencer::idmMacroAntiGroove()
{
    // Groove actively destroyed
    IdmMacroPreset p;
    p.name = "Anti-Groove Intelligence";

    // Drill
    p.drill.enabled = true;
    p.drill.amount = 0.65f;
    p.drill.mutationRate = 0.35f;
    p.drill.dropout = 0.25f;
    p.drill.chaos = 0.30f;
    p.drill.spread = 0.55f;
    p.drill.velDecay = 0.45f;
    p.drill.accentFlip = 0.12f;
    p.drill.temporalAggression = 0.75f;
    p.drill.minBurst = 3;
    p.drill.maxBurst = 10;
    p.drill.grid = DrillGrid::Septuplet;
    p.drill.transitionBeats = 0.5f;

    // Fill (disabled - let gate do the work)
    p.fill.enabled = false;
    p.fill.fillLengthSteps = 0;
    p.fill.triggerChance = 0.0f;
    p.fill.fillAmount = 0.0f;
    p.fill.decayPerStep = 0.0f;

    // Gate
    p.gate.enabled = true;
    p.gate.silenceChance = 0.55f;
    p.gate.burstChance = 0.40f;
    p.gate.minSilentSteps = 1;
    p.gate.maxSilentSteps = 2;

    return p;
}

//==============================================================================
// Factory Registration
//==============================================================================

// Register instrument factory - must be inside namespace DSP
DSP_REGISTER_INSTRUMENT(DrumMachinePureDSP, "DrumMachine")

} // namespace DSP
