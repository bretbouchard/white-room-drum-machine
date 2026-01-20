/*
  ==============================================================================

    RetroFuturisticTronLookAndFeel.h
    JUCE LookAndFeel implementation of White Room's Retro Futuristic Tron Theme

    Design Specs from: white_room/docs/design-system/THEME_GALLERY.md
    - Primary Accent: #00FFFF (Neon Cyan) with glow
    - Secondary Accent: #FF00FF (Electric Magenta)
    - Backgrounds: #050510 → #101025 (Deep space blacks)
    - Corner Radius: 0-4px (digital, precise)
    - Border Width: 1-2px neon (digital precision)
    - Typography: Futuristic, tech (Orbitron-style)

  ==============================================================================
*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

//==============================================================================
class RetroFuturisticTronLookAndFeel  : public juce::LookAndFeel_V4
{
public:
    RetroFuturisticTronLookAndFeel()
    {
        // Neon Cyberpunk Palette
        setColour (juce::ResizableWindow::backgroundColourId, juce::Colour (0xFF050510));
        setColour (juce::Slider::thumbColourId, juce::Colour (0xFF00FFFF)); // Neon Cyan
        setColour (juce::Slider::trackColourId, juce::Colour (0xFF101025));
        setColour (juce::Slider::backgroundColourId, juce::Colour (0xFF050510));
        setColour (juce::ComboBox::backgroundColourId, juce::Colour (0xFF151530));
        setColour (juce::ComboBox::textColourId, juce::Colour (0xFF00FFFF));
        setColour (juce::ComboBox::outlineColourId, juce::Colour (0xFF00FFFF));
        setColour (juce::Label::textColourId, juce::Colour (0xFF00FFFF));
        setColour (juce::TextButton::buttonColourId, juce::Colour (0xFF151530));
        setColour (juce::TextButton::textColourOnId, juce::Colour (0xFFFFFFFF));
        setColour (juce::TextButton::textColourOffId, juce::Colour (0xFF00FFFF));
    }

    void drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           const juce::Slider::SliderStyle style, juce::Slider& slider) override
    {
        auto bounds = slider.getLocalBounds().toFloat();

        // Track (deep space black)
        g.setColour (findColour (juce::Slider::backgroundColourId));
        g.fillRect (bounds);

        // Fill with glow effect (neon cyan)
        auto trackBounds = bounds;
        if (slider.isHorizontal())
            trackBounds.setWidth (sliderPos - x);
        else
            trackBounds.setTop (sliderPos);

        g.setColour (findColour (juce::Slider::thumbColourId).withAlpha (0.8f));
        g.fillRect (trackBounds);

        // Glow effect
        g.setColour (findColour (juce::Slider::thumbColourId).withAlpha (0.3f));
        g.fillRect (trackBounds.expanded (4.0f, 0.0f));

        // Thumb (digital precision)
        auto thumbBounds = bounds;
        if (slider.isHorizontal())
        {
            thumbBounds.setWidth (12.0f);
            thumbBounds.setCentre (bounds.getCentre().withX (sliderPos));
        }
        else
        {
            thumbBounds.setHeight (12.0f);
            thumbBounds.setCentre (bounds.getCentre().withY (sliderPos));
        }

        g.setColour (juce::Colour (0xFFFF00FF)); // Magenta thumb
        g.fillRect (thumbBounds);

        // Thin neon border
        g.setColour (juce::Colour (0xFF00FFFF).withAlpha (0.5f));
        g.drawRect (bounds, 1.0f);
    }

    void drawComboBox (juce::Graphics& g, int width, int height, bool isButtonDown,
                       int buttonX, int buttonY, int buttonW, int buttonH,
                       juce::ComboBox& box) override
    {
        auto bounds = juce::Rectangle<float> (0, 0, width, height).reduced (2.0f);

        // Background (cyber grid blue)
        g.setColour (findColour (juce::ComboBox::backgroundColourId));
        g.fillRect (bounds);

        // Neon border with glow
        g.setColour (findColour (juce::ComboBox::outlineColourId).withAlpha (0.5f));
        g.drawRect (bounds, 1.0f);
        g.setColour (findColour (juce::ComboBox::outlineColourId).withAlpha (0.2f));
        g.drawRect (bounds.expanded (3.0f), 1.0f);

        // Text (digital)
        g.setColour (findColour (juce::ComboBox::textColourId));
        g.setFont (juce::FontOptions { juce::Font::getDefaultMonospacedFontName(), 14.0f });
        g.drawText (box.getTextWhenNothingSelected(),
                   bounds.reduced (10, 0),
                   juce::Justification::centredLeft, true);
    }

    void drawLabel (juce::Graphics& g, juce::Label& label) override
    {
        g.setColour (findColour (juce::Label::textColourId));
        g.setFont (juce::FontOptions { juce::Font::getDefaultMonospacedFontName(), 14.0f });

        auto bounds = label.getLocalBounds().reduced (2, 0);
        g.drawText (label.getText(), bounds,
                   label.getJustificationType(),
                   true);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RetroFuturisticTronLookAndFeel)
};
