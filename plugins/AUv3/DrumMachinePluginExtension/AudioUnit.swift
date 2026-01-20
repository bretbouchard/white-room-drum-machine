//
//  AudioUnit.swift
//  DrumMachinePluginExtension
//
//  Main AUv3 audio unit implementation
//

import AudioToolbox

@objc(AudioUnit)
public class AudioUnit: AUAudioUnit {

    var dsp: DrumMachineDSPWrapper?
    var parameterTree: AUParameterTree!

    // Drum Machine Parameters
    private let globalParameters: [AUParameterIdentifier: (name: String, range: ClosedRange<Float>, unit: AUUnitParameterUnit, defaultValue: Float)] = [
        "tempo": ("Tempo", 60.0...200.0, .bpm, 120.0),
        "swing": ("Swing", 0.0...1.0, .generic, 0.0),
        "masterVolume": ("Master Volume", 0.0...1.0, .linearGain, 0.8),
        "patternLength": ("Pattern Length", 1.0...64.0, .generic, 16.0),
    ]

    private let timingParameters: [AUParameterIdentifier: (name: String, range: ClosedRange<Float>, unit: AUUnitParameterUnit, defaultValue: Float)] = [
        "pocketOffset": ("Pocket Offset", -0.1...0.1, .generic, 0.0),
        "pushOffset": ("Push Offset", -0.1...0.0, .generic, -0.04),
        "pullOffset": ("Pull Offset", 0.0...0.1, .generic, 0.06),
    ]

    private let dillaParameters: [AUParameterIdentifier: (name: String, range: ClosedRange<Float>, unit: AUUnitParameterUnit, defaultValue: Float)] = [
        "dillaAmount": ("Dilla Amount", 0.0...1.0, .generic, 0.6),
        "dillaHatBias": ("Dilla Hat Bias", 0.0...1.0, .generic, 0.55),
        "dillaSnareLate": ("Dilla Snare Late", 0.0...1.0, .generic, 0.8),
        "dillaKickTight": ("Dilla Kick Tight", 0.0...1.0, .generic, 0.7),
        "dillaMaxDrift": ("Dilla Max Drift", 0.0...0.3, .generic, 0.15),
    ]

    private let stereoParameters: [AUParameterIdentifier: (name: String, range: ClosedRange<Float>, unit: AUUnitParameterUnit, defaultValue: Float)] = [
        "stereoWidth": ("Stereo Width", 0.0...1.0, .generic, 0.5),
        "roomWidth": ("Room Width", 0.0...1.0, .generic, 0.3),
        "effectsWidth": ("Effects Width", 0.0...1.0, .generic, 0.7),
    ]

    public override init(componentDescription: AudioComponentDescription,
                         options: AudioComponentInstantiationOptions) throws {
        try super.init(componentDescription: componentDescription, options: options)

        // Initialize DSP wrapper
        dsp = DrumMachineDSPWrapper()

        // Create parameter tree
        parameterTree = AUParameterTree()

        // Register global parameters
        for (identifier, info) in globalParameters {
            let parameter = AUParameter(
                identifier: identifier,
                name: info.name,
                address: AUParameterAddress(identifier.hashValue),
                range: info.range,
                unit: info.unit,
                flags: [.flag_IsReadable, .flag_IsWritable]
            )
            parameter.value = info.defaultValue
            parameterTree.registerParameter(parameter)
        }

        // Register timing parameters
        for (identifier, info) in timingParameters {
            let parameter = AUParameter(
                identifier: identifier,
                name: info.name,
                address: AUParameterAddress(identifier.hashValue),
                range: info.range,
                unit: info.unit,
                flags: [.flag_IsReadable, .flag_IsWritable]
            )
            parameter.value = info.defaultValue
            parameterTree.registerParameter(parameter)
        }

        // Register Dilla parameters
        for (identifier, info) in dillaParameters {
            let parameter = AUParameter(
                identifier: identifier,
                name: info.name,
                address: AUParameterAddress(identifier.hashValue),
                range: info.range,
                unit: info.unit,
                flags: [.flag_IsReadable, .flag_IsWritable]
            )
            parameter.value = info.defaultValue
            parameterTree.registerParameter(parameter)
        }

        // Register stereo parameters
        for (identifier, info) in stereoParameters {
            let parameter = AUParameter(
                identifier: identifier,
                name: info.name,
                address: AUParameterAddress(identifier.hashValue),
                range: info.range,
                unit: info.unit,
                flags: [.flag_IsReadable, .flag_IsWritable]
            )
            parameter.value = info.defaultValue
            parameterTree.registerParameter(parameter)
        }

        // Register per-track volume parameters (16 tracks)
        for track in 0..<16 {
            let identifier = "trackVolume_\(track)"
            let parameter = AUParameter(
                identifier: identifier,
                name: "Track \(track) Volume",
                address: AUParameterAddress(DrumMachineDSP.ParameterAddress.TrackVolume0.rawValue + UInt(track)),
                range: 0.0...1.0,
                unit: .linearGain,
                flags: [.flag_IsReadable, .flag_IsWritable]
            )
            parameter.value = 0.8
            parameterTree.registerParameter(parameter)
        }

        // Register voice parameters (Kick)
        let kickParameters = [
            ("kickPitch", "Kick Pitch", 0.0...1.0, 0.5),
            ("kickDecay", "Kick Decay", 0.0...1.0, 0.5),
            ("kickClick", "Kick Click", 0.0...1.0, 0.3)
        ]

        for (identifier, name, range, defaultValue) in kickParameters {
            let parameter = AUParameter(
                identifier: identifier,
                name: name,
                address: AUParameterAddress(identifier.hashValue),
                range: range,
                unit: .generic,
                flags: [.flag_IsReadable, .flag_IsWritable]
            )
            parameter.value = defaultValue
            parameterTree.registerParameter(parameter)
        }

        // Register voice parameters (Snare)
        let snareParameters = [
            ("snareTone", "Snare Tone", 0.0...1.0, 0.7),
            ("snareDecay", "Snare Decay", 0.0...1.0, 0.5),
            ("snareSnap", "Snare Snap", 0.0...1.0, 0.5)
        ]

        for (identifier, name, range, defaultValue) in snareParameters {
            let parameter = AUParameter(
                identifier: identifier,
                name: name,
                address: AUParameterAddress(identifier.hashValue),
                range: range,
                unit: .generic,
                flags: [.flag_IsReadable, .flag_IsWritable]
            )
            parameter.value = defaultValue
            parameterTree.registerParameter(parameter)
        }

        // Default parameter values
        parameterTree.implementorValueProvider = { [weak self] address in
            guard let self = self else { return 0.0 }
            return self.dsp?.getParameter(forAddress: address) ?? 0.0
        }

        parameterTree.implementorStringFromValueProvider = { parameter, value in
            return String(format: "%.2f", value)
        }
    }

    public override func allocateRenderResources() throws {
        try super.allocateRenderResources()

        // Initialize DSP with current format
        if let dsp = dsp {
            let format = self.outputBusses[0].format
            dsp.initialize(withSampleRate: format.sampleRate,
                          maximumFramesToRender: Int32(self.maximumFramesToRender))
        }
    }

    public override func deallocateRenderResources() {
        super.deallocateRenderResources()
    }

    public override var internalRenderBlock: AUInternalRenderBlock {
        return { [weak self] timestamp, frameCount, outputBusNumber, outputBufferList, inputBusNumber, events in
            guard let self = self else {
                return kAudioUnitErr_InvalidProperty
            }

            // Process events (MIDI, parameters)
            for event in events {
                self.handleEvent(event)
            }

            // Render audio
            self.dsp?.process(frameCount: frameCount,
                            outputBufferList: outputBufferList,
                            timestamp: timestamp)

            return noErr
        }
    }

    private func handleEvent(_ event: AURenderEvent) {
        switch event.head.eventType {
        case .MIDI:
            if let midiEvent = event.MIDI {
                self.handleMIDI(midiEvent)
            }
        case .parameter:
            if let parameterEvent = event.parameter {
                self.handleParameter(parameterEvent)
            }
        default:
            break
        }
    }

    private func handleMIDI(_ event: AUMIDIEvent) {
        var message = [UInt8](repeating: 0, count: Int(event.length))
        message.withMutableBufferPointer { buffer in
            if let baseAddress = buffer.baseAddress {
                event.getData(&baseAddress.pointee)
                self.dsp?.handleMIDIEvent(message, messageSize: UInt8(event.length))
            }
        }
    }

    private func handleParameter(_ event: AUParameterEvent) {
        dsp?.setParameter(event.parameterAddress, value: event.value)
    }
}
