//
//  ParameterBridge.swift
//  DrumMachinePluginExtension
//
//  Swift wrapper for C++ DSP
//

import Foundation

@objc
public class DrumMachineDSPWrapper: NSObject {

    private var dspPtr: OpaquePointer?

    override public init() {
        super.init()
        dspPtr = DrumMachineDSP_Create()
    }

    deinit {
        if let ptr = dspPtr {
            DrumMachineDSP_Destroy(ptr)
        }
    }

    public func initialize(withSampleRate sampleRate: Double, maximumFramesToRender: Int32) {
        if let ptr = dspPtr {
            DrumMachineDSP_Initialize(ptr, sampleRate, maximumFramesToRender)
        }
    }

    public func process(frameCount: UInt32,
                       outputBufferList: UnsafeMutablePointer<AudioBufferList>,
                       timestamp: UnsafePointer<AUEventSampleTime>) {
        if let ptr = dspPtr {
            DrumMachineDSP_Process(ptr, frameCount, outputBufferList, timestamp)
        }
    }

    public func setParameter(_ address: AUParameterAddress, value: Float) {
        if let ptr = dspPtr {
            DrumMachineDSP_SetParameter(ptr, address, value)
        }
    }

    public func getParameter(forAddress address: AUParameterAddress) -> Float {
        guard let ptr = dspPtr else { return 0.0 }
        return DrumMachineDSP_GetParameter(ptr, address)
    }

    public func handleMIDIEvent(_ message: [UInt8], messageSize: UInt8) {
        if let ptr = dspPtr {
            message.withUnsafeBytes { bytes in
                if let baseAddress = bytes.baseAddress {
                    DrumMachineDSP_HandleMIDIEvent(ptr, baseAddress, messageSize)
                }
            }
        }
    }

    public func setStep(track: Int, step: Int, active: Bool, velocity: UInt8) {
        if let ptr = dspPtr {
            DrumMachineDSP_SetStep(ptr, Int32(track), Int32(step), active ? 1 : 0, velocity)
        }
    }

    public func getStep(track: Int, step: Int) -> Bool {
        guard let ptr = dspPtr else { return false }
        return DrumMachineDSP_GetStep(ptr, Int32(track), Int32(step)) != 0
    }

    public func getStepVelocity(track: Int, step: Int) -> UInt8 {
        guard let ptr = dspPtr else { return 0 }
        return UInt8(DrumMachineDSP_GetStepVelocity(ptr, Int32(track), Int32(step)))
    }

    public func setState(_ stateData: String) {
        if let ptr = dspPtr {
            stateData.withCString { cString in
                DrumMachineDSP_SetState(ptr, cString)
            }
        }
    }

    public func getState() -> String? {
        guard let ptr = dspPtr else { return nil }
        guard let cString = DrumMachineDSP_GetState(ptr) else { return nil }
        return String(cString: cString)
    }

    public func savePattern() -> String? {
        guard let ptr = dspPtr else { return nil }

        var buffer = [Int8](repeating: 0, count: 65536)
        let success = DrumMachineDSP_SavePattern(ptr, &buffer, Int32(buffer.count))

        if success > 0 {
            return String(cString: &buffer)
        }
        return nil
    }

    public func loadPattern(_ jsonData: String) -> Bool {
        guard let ptr = dspPtr else { return false }
        return jsonData.withCString { cString in
            DrumMachineDSP_LoadPattern(ptr, cString) != 0
        }
    }

    public func saveKit() -> String? {
        guard let ptr = dspPtr else { return nil }

        var buffer = [Int8](repeating: 0, count: 65536)
        let success = DrumMachineDSP_SaveKit(ptr, &buffer, Int32(buffer.count))

        if success > 0 {
            return String(cString: &buffer)
        }
        return nil
    }

    public func loadKit(_ jsonData: String) -> Bool {
        guard let ptr = dspPtr else { return false }
        return jsonData.withCString { cString in
            DrumMachineDSP_LoadKit(ptr, cString) != 0
        }
    }
}

// C function declarations for C++ interoperability
private func DrumMachineDSP_Create() -> OpaquePointer {
    let impl = DrumMachineDSP.__create()
    return Unmanaged.passRetained(impl as AnyObject).toOpaque()
}

private func DrumMachineDSP_Destroy(_ dsp: OpaquePointer) {
    let impl = Unmanaged<AnyObject>.fromOpaque(dsp).takeRetainedValue() as! DrumMachineDSP
    // Destructor called automatically
}

private func DrumMachineDSP_Initialize(_ dsp: OpaquePointer, _ sampleRate: Double, _ maximumFramesToRender: Int32) {
    let impl = Unmanaged<AnyObject>.fromOpaque(dsp).takeUnretainedValue() as! DrumMachineDSP
    impl.initialize(withSampleRate: sampleRate, maximumFramesToRender: maximumFramesToRender)
}

private func DrumMachineDSP_Process(_ dsp: OpaquePointer, _ frameCount: UInt32, _ outputBufferList: UnsafeMutablePointer<AudioBufferList>, _ timestamp: UnsafePointer<AUEventSampleTime>) {
    let impl = Unmanaged<AnyObject>.fromOpaque(dsp).takeUnretainedValue() as! DrumMachineDSP
    impl.process(frameCount: frameCount, outputBufferList: outputBufferList, timestamp: timestamp)
}

private func DrumMachineDSP_SetParameter(_ dsp: OpaquePointer, _ address: AUParameterAddress, _ value: Float) {
    let impl = Unmanaged<AnyObject>.fromOpaque(dsp).takeUnretainedValue() as! DrumMachineDSP
    impl.setParameter(address, value: value)
}

private func DrumMachineDSP_GetParameter(_ dsp: OpaquePointer, _ address: AUParameterAddress) -> Float {
    let impl = Unmanaged<AnyObject>.fromOpaque(dsp).takeUnretainedValue() as! DrumMachineDSP
    return impl.getParameter(address)
}

private func DrumMachineDSP_HandleMIDIEvent(_ dsp: OpaquePointer, _ message: UnsafeRawPointer, _ messageSize: UInt8) {
    let impl = Unmanaged<AnyObject>.fromOpaque(dsp).takeUnretainedValue() as! DrumMachineDSP
    let bytes = message.assumingMemoryBound(to: UInt8.self)
    impl.handleMIDIEvent(bytes, messageSize: messageSize)
}

private func DrumMachineDSP_SetStep(_ dsp: OpaquePointer, _ track: Int32, _ step: Int32, _ active: Int32, _ velocity: UInt8) {
    let impl = Unmanaged<AnyObject>.fromOpaque(dsp).takeUnretainedValue() as! DrumMachineDSP
    impl.setStep(track: Int(track), step: Int(step), active: active != 0, velocity: velocity)
}

private func DrumMachineDSP_GetStep(_ dsp: OpaquePointer, _ track: Int32, _ step: Int32) -> Int32 {
    let impl = Unmanaged<AnyObject>.fromOpaque(dsp).takeUnretainedValue() as! DrumMachineDSP
    return impl.getStep(track: Int(track), step: Int(step)) ? 1 : 0
}

private func DrumMachineDSP_GetStepVelocity(_ dsp: OpaquePointer, _ track: Int32, _ step: Int32) -> Int32 {
    let impl = Unmanaged<AnyObject>.fromOpaque(dsp).takeUnretainedValue() as! DrumMachineDSP
    return Int32(impl.getStepVelocity(track: Int(track), step: Int(step)))
}

private func DrumMachineDSP_SetState(_ dsp: OpaquePointer, _ stateData: UnsafePointer<Int8>) {
    let impl = Unmanaged<AnyObject>.fromOpaque(dsp).takeUnretainedValue() as! DrumMachineDSP
    impl.setState(String(cString: stateData))
}

private func DrumMachineDSP_GetState(_ dsp: OpaquePointer) -> UnsafePointer<Int8>? {
    let impl = Unmanaged<AnyObject>.fromOpaque(dsp).takeUnretainedValue() as! DrumMachineDSP
    return impl.getState()
}

private func DrumMachineDSP_SavePattern(_ dsp: OpaquePointer, _ jsonBuffer: UnsafeMutablePointer<Int8>, _ jsonBufferSize: Int32) -> Int32 {
    let impl = Unmanaged<AnyObject>.fromOpaque(dsp).takeUnretainedValue() as! DrumMachineDSP
    return impl.savePattern(jsonBuffer, jsonBufferSize: Int(jsonBufferSize)) ? 1 : 0
}

private func DrumMachineDSP_LoadPattern(_ dsp: OpaquePointer, _ jsonData: UnsafePointer<Int8>) -> Int32 {
    let impl = Unmanaged<AnyObject>.fromOpaque(dsp).takeUnretainedValue() as! DrumMachineDSP
    return impl.loadPattern(String(cString: jsonData)) ? 1 : 0
}

private func DrumMachineDSP_SaveKit(_ dsp: OpaquePointer, _ jsonBuffer: UnsafeMutablePointer<Int8>, _ jsonBufferSize: Int32) -> Int32 {
    let impl = Unmanaged<AnyObject>.fromOpaque(dsp).takeUnretainedValue() as! DrumMachineDSP
    return impl.saveKit(jsonBuffer, jsonBufferSize: Int(jsonBufferSize)) ? 1 : 0
}

private func DrumMachineDSP_LoadKit(_ dsp: OpaquePointer, _ jsonData: UnsafePointer<Int8>) -> Int32 {
    let impl = Unmanaged<AnyObject>.fromOpaque(dsp).takeUnretainedValue() as! DrumMachineDSP
    return impl.loadKit(String(cString: jsonData)) ? 1 : 0
}
