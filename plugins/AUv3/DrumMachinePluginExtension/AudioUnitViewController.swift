//
//  AudioUnitViewController.swift
//  DrumMachinePluginExtension
//
//  UI view controller for AUv3 extension
//

import UIKit
import AudioToolbox

@objc(AudioUnitViewController)
public class AudioUnitViewController: UIViewController {

    var audioUnit: AUAudioUnit?

    private let scrollView = UIScrollView()
    private let contentView = UIView()

    // Drum Pad Grid (4x4)
    private var drumPads: [UIButton] = []
    private let drumGrid = UIStackView()

    // Step Sequencer (16 steps x 4 tracks for now)
    private var stepButtons: [UIButton] = []
    private let stepSequencerView = UIView()

    // Controls
    private let tempoSlider = UISlider()
    private let swingSlider = UISlider()
    private let volumeSlider = UISlider()
    private let playButton = UIButton(type: .system)
    private let stopButton = UIButton(type: .system)
    private let clearButton = UIButton(type: .system)
    private let randomButton = UIButton(type: .system)

    public override func viewDidLoad() {
        super.viewDidLoad()
        setupUI()
    }

    private func setupUI() {
        view.backgroundColor = .systemBackground

        // Setup scroll view
        view.addSubview(scrollView)
        scrollView.translatesAutoresizingMaskIntoConstraints = false
        NSLayoutConstraint.activate([
            scrollView.topAnchor.constraint(equalTo: view.topAnchor),
            scrollView.leadingAnchor.constraint(equalTo: view.leadingAnchor),
            scrollView.trailingAnchor.constraint(equalTo: view.trailingAnchor),
            scrollView.bottomAnchor.constraint(equalTo: view.bottomAnchor)
        ])

        scrollView.addSubview(contentView)
        contentView.translatesAutoresizingMaskIntoConstraints = false
        NSLayoutConstraint.activate([
            contentView.topAnchor.constraint(equalTo: scrollView.topAnchor),
            contentView.leadingAnchor.constraint(equalTo: scrollView.leadingAnchor),
            contentView.trailingAnchor.constraint(equalTo: scrollView.trailingAnchor),
            contentView.bottomAnchor.constraint(equalTo: scrollView.bottomAnchor),
            contentView.widthAnchor.constraint(equalTo: scrollView.widthAnchor)
        ])

        // Setup title
        let titleLabel = UILabel()
        titleLabel.text = "Drum Machine"
        titleLabel.font = .boldSystemFont(ofSize: 24)
        titleLabel.textAlignment = .center
        contentView.addSubview(titleLabel)
        titleLabel.translatesAutoresizingMaskIntoConstraints = false
        NSLayoutConstraint.activate([
            titleLabel.topAnchor.constraint(equalTo: contentView.topAnchor, constant: 20),
            titleLabel.leadingAnchor.constraint(equalTo: contentView.leadingAnchor, constant: 20),
            titleLabel.trailingAnchor.constraint(equalTo: contentView.trailingAnchor, constant: -20)
        ])

        // Setup drum grid (4x4 pads)
        setupDrumGrid()

        // Setup step sequencer
        setupStepSequencer()

        // Setup controls
        setupControls()
    }

    private func setupDrumGrid() {
        contentView.addSubview(drumGrid)
        drumGrid.translatesAutoresizingMaskIntoConstraints = false
        drumGrid.axis = .vertical
        drumGrid.distribution = .fillEqually
        drumGrid.spacing = 8

        // Create 4 rows of 4 pads
        for row in 0..<4 {
            let rowStack = UIStackView()
            rowStack.axis = .horizontal
            rowStack.distribution = .fillEqually
            rowStack.spacing = 8

            for col in 0..<4 {
                let padIndex = row * 4 + col
                let pad = UIButton(type: .system)
                pad.backgroundColor = .systemGray5
                pad.layer.cornerRadius = 8
                pad.setTitle("\(padIndex + 1)", for: .normal)
                pad.tag = padIndex
                pad.addTarget(self, action: #selector(drumPadTapped(_:)), for: .touchDown)
                drumPads.append(pad)
                rowStack.addArrangedSubview(pad)
            }

            drumGrid.addArrangedSubview(rowStack)
        }

        NSLayoutConstraint.activate([
            drumGrid.topAnchor.constraint(equalTo: contentView.topAnchor, constant: 80),
            drumGrid.leadingAnchor.constraint(equalTo: contentView.leadingAnchor, constant: 20),
            drumGrid.trailingAnchor.constraint(equalTo: contentView.trailingAnchor, constant: -20),
            drumGrid.heightAnchor.constraint(equalToConstant: 300)
        ])
    }

    private func setupStepSequencer() {
        contentView.addSubview(stepSequencerView)
        stepSequencerView.translatesAutoresizingMaskIntoConstraints = false
        stepSequencerView.backgroundColor = .systemGray6
        stepSequencerView.layer.cornerRadius = 8

        // Create step buttons (16 steps x 4 tracks)
        for track in 0..<4 {
            for step in 0..<16 {
                let stepButton = UIButton(type: .system)
                stepButton.backgroundColor = .systemGray4
                stepButton.layer.cornerRadius = 4
                stepButton.tag = track * 16 + step
                stepButton.addTarget(self, action: #selector(stepButtonTapped(_:)), for: .touchUpInside)
                stepButtons.append(stepButton)
                stepSequencerView.addSubview(stepButton)
            }
        }

        // Layout step buttons
        var constraints: [NSLayoutConstraint] = []
        let buttonSize: CGFloat = 30
        let spacing: CGFloat = 4

        for (index, button) in stepButtons.enumerated() {
            let track = index / 16
            let step = index % 16

            button.translatesAutoresizingMaskIntoConstraints = false
            constraints.append(button.leadingAnchor.constraint(equalTo: stepSequencerView.leadingAnchor,
                                                               constant: CGFloat(step) * (buttonSize + spacing) + 10))
            constraints.append(button.topAnchor.constraint(equalTo: stepSequencerView.topAnchor,
                                                         constant: CGFloat(track) * (buttonSize + spacing) + 10))
            constraints.append(button.widthAnchor.constraint(equalToConstant: buttonSize))
            constraints.append(button.heightAnchor.constraint(equalToConstant: buttonSize))
        }

        NSLayoutConstraint.activate(constraints)
        NSLayoutConstraint.activate([
            stepSequencerView.topAnchor.constraint(equalTo: drumGrid.bottomAnchor, constant: 20),
            stepSequencerView.leadingAnchor.constraint(equalTo: contentView.leadingAnchor, constant: 20),
            stepSequencerView.trailingAnchor.constraint(equalTo: contentView.trailingAnchor, constant: -20),
            stepSequencerView.heightAnchor.constraint(equalToConstant: 150)
        ])
    }

    private func setupControls() {
        let controlsStack = UIStackView()
        controlsStack.axis = .vertical
        controlsStack.spacing = 16
        contentView.addSubview(controlsStack)
        controlsStack.translatesAutoresizingMaskIntoConstraints = false

        // Tempo slider
        let tempoLabel = createLabeledSlider("Tempo", slider: tempoSlider, range: 60...200, defaultValue: 120)
        controlsStack.addArrangedSubview(tempoLabel)

        // Swing slider
        let swingLabel = createLabeledSlider("Swing", slider: swingSlider, range: 0...100, defaultValue: 0)
        controlsStack.addArrangedSubview(swingLabel)

        // Volume slider
        let volumeLabel = createLabeledSlider("Volume", slider: volumeSlider, range: 0...100, defaultValue: 80)
        controlsStack.addArrangedSubview(volumeLabel)

        // Transport buttons
        let transportStack = UIStackView()
        transportStack.axis = .horizontal
        transportStack.distribution = .fillEqually
        transportStack.spacing = 16

        playButton.setTitle("Play", for: .normal)
        playButton.addTarget(self, action: #selector(playTapped), for: .touchUpInside)
        transportStack.addArrangedSubview(playButton)

        stopButton.setTitle("Stop", for: .normal)
        stopButton.addTarget(self, action: #selector(stopTapped), for: .touchUpInside)
        transportStack.addArrangedSubview(stopButton)

        controlsStack.addArrangedSubview(transportStack)

        // Pattern buttons
        let patternStack = UIStackView()
        patternStack.axis = .horizontal
        patternStack.distribution = .fillEqually
        patternStack.spacing = 16

        clearButton.setTitle("Clear", for: .normal)
        clearButton.addTarget(self, action: #selector(clearTapped), for: .touchUpInside)
        patternStack.addArrangedSubview(clearButton)

        randomButton.setTitle("Random", for: .normal)
        randomButton.addTarget(self, action: #selector(randomTapped), for: .touchUpInside)
        patternStack.addArrangedSubview(randomButton)

        controlsStack.addArrangedSubview(patternStack)

        NSLayoutConstraint.activate([
            controlsStack.topAnchor.constraint(equalTo: stepSequencerView.bottomAnchor, constant: 20),
            controlsStack.leadingAnchor.constraint(equalTo: contentView.leadingAnchor, constant: 20),
            controlsStack.trailingAnchor.constraint(equalTo: contentView.trailingAnchor, constant: -20),
            controlsStack.bottomAnchor.constraint(equalTo: contentView.bottomAnchor, constant: -20)
        ])
    }

    private func createLabeledSlider(_ label: String, slider: UISlider, range: ClosedRange<Float>, defaultValue: Float) -> UIView {
        let container = UIView()
        let labelView = UILabel()
        labelView.text = label
        labelView.font = .systemFont(ofSize: 14)

        slider.minimumValue = range.lowerBound
        slider.maximumValue = range.upperBound
        slider.value = defaultValue
        slider.addTarget(self, action: #selector(sliderChanged(_:)), for: .valueChanged)

        let stack = UIStackView(arrangedSubviews: [labelView, slider])
        stack.axis = .vertical
        stack.spacing = 4
        container.addSubview(stack)
        stack.translatesAutoresizingMaskIntoConstraints = false
        NSLayoutConstraint.activate([
            stack.topAnchor.constraint(equalTo: container.topAnchor),
            stack.leadingAnchor.constraint(equalTo: container.leadingAnchor),
            stack.trailingAnchor.constraint(equalTo: container.trailingAnchor),
            stack.bottomAnchor.constraint(equalTo: container.bottomAnchor)
        ])

        return container
    }

    @objc private func drumPadTapped(_ sender: UIButton) {
        let padIndex = sender.tag

        // Visual feedback
        UIView.animate(withDuration: 0.1, animations: {
            sender.backgroundColor = .systemBlue
        }) { _ in
            UIView.animate(withDuration: 0.1) {
                sender.backgroundColor = .systemGray5
            }
        }

        // Trigger drum sound (would send MIDI to DSP)
        // This would require accessing the audio unit's DSP
    }

    @objc private func stepButtonTapped(_ sender: UIButton) {
        let isActive = sender.backgroundColor == .systemBlue

        UIView.animate(withDuration: 0.1) {
            sender.backgroundColor = isActive ? .systemGray4 : .systemBlue
        }

        // Update step sequencer state
        // This would require accessing the audio unit's DSP
    }

    @objc private func sliderChanged(_ sender: UISlider) {
        guard let audioUnit = audioUnit,
              let parameterTree = audioUnit.parameterTree else { return }

        if sender == tempoSlider {
            let param = parameterTree.allParameters.first { $0.identifier == "tempo" }
            param?.value = sender.value
        } else if sender == swingSlider {
            let param = parameterTree.allParameters.first { $0.identifier == "swing" }
            param?.value = sender.value / 100.0
        } else if sender == volumeSlider {
            let param = parameterTree.allParameters.first { $0.identifier == "masterVolume" }
            param?.value = sender.value / 100.0
        }
    }

    @objc private func playTapped() {
        // Start playback
        playButton.backgroundColor = .systemGreen
        stopButton.backgroundColor = .systemBackground
    }

    @objc private func stopTapped() {
        // Stop playback
        stopButton.backgroundColor = .systemRed
        playButton.backgroundColor = .systemBackground
    }

    @objc private func clearTapped() {
        // Clear pattern
        for button in stepButtons {
            button.backgroundColor = .systemGray4
        }
    }

    @objc private func randomTapped() {
        // Randomize pattern
        for button in stepButtons {
            let active = Bool.random()
            button.backgroundColor = active ? .systemBlue : .systemGray4
        }
    }
}
