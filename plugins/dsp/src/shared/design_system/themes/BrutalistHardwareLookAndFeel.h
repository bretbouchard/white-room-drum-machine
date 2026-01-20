/*
  ==============================================================================

    BrutalistHardwareLookAndFeel.h
    JUCE LookAndFeel implementation of White Room's Brutalist Hardware Theme

    Design Specs from: white_room/docs/design-system/THEME_GALLERY.md
    - Primary Accent: #FF6B35 (Industrial Orange)
    - Backgrounds: #1A1A1A → #E8E8E8 (Machine grays)
    - Corner Radius: 0px (sharp, brutalist)
    - Border Width: 2-3px (thick, structural)
    - Typography: Monospaced, technical

  ==============================================================================
*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

//==============================================================================
class BrutalistHardwareLookAndFeel  : public juce::LookAndFeel_V4
{
public:
    BrutalistHardwareLookAndFeel()
    {
        // Color Palette from White Room design system
        // Primary Accent: Industrial Orange #FF6B35
        setColour (juce::ResizableWindow::backgroundColourId, juce::Colour (0xFF1A1A1A));
        setColour (juce::Slider::thumbColourId, juce::Colour (0xFFFF6B35));
        setColour (juce::Slider::trackColourId, juce::Colour (0xFF404040));
        setColour (juce::Slider::backgroundColourId, juce::Colour (0xFF252525));
        setColour (juce::ComboBox::backgroundColourId, juce::Colour (0xFF252525));
        setColour (juce::ComboBox::textColourId, juce::Colour (0xFFE8E8E8));
        setColour (juce::ComboBox::outlineColourId, juce::Colour (0xFF404040));
        setColour (juce::Label::textColourId, juce::Colour (0xFFE8E8E8));
        setColour (juce::TextButton::buttonColourId, juce::Colour (0xFF404040));
        setColour (juce::TextButton::textColourOnId, juce::Colour (0xFFFFFFFF));
        setColour (juce::TextButton::textColourOffId, juce::Colour (0xFFE8E8E8));
    }

    //==============================================================================
    // SHARP CORNERS (0px radius - brutalist aesthetic)
    void drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           const juce::Slider::SliderStyle style, juce::Slider& slider) override
    {
        auto bounds = slider.getLocalBounds().toFloat();

        // Track background (machine gray)
        g.setColour (findColour (juce::Slider::backgroundColourId));
        g.fillRect (bounds);

        // Fill (industrial orange accent)
        auto trackBounds = bounds;
        if (slider.isHorizontal())
            trackBounds.setWidth (sliderPos - x);
        else
            trackBounds.setTop (sliderPos);

        g.setColour (findColour (juce::Slider::thumbColourId));
        g.fillRect (trackBounds);

        // Thumb (sharp corners, industrial feel)
        auto thumbBounds = bounds;
        if (slider.isHorizontal())
        {
            thumbBounds.setWidth (10.0f);
            thumbBounds.setCentre (bounds.getCentre().withX (sliderPos));
        }
        else
        {
            thumbBounds.setHeight (10.0f);
            thumbBounds.setCentre (bounds.getCentre().withY (sliderPos));
        }

        g.setColour (findColour (juce::Slider::thumbColourId));
        g.fillRect (thumbBounds);

        // Thick border (2-3px structural)
        g.setColour (juce::Colour (0xFF404040));
        g.drawRect (bounds, 2.0f);
    }

    //==============================================================================
    // COMBO BOX WITH SHARP CORNERS AND THICK BORDERS
    void drawComboBox (juce::Graphics& g, int width, int height, bool isButtonDown,
                       int buttonX, int buttonY, int buttonW, int buttonH,
                       juce::ComboBox& box) override
    {
        auto bounds = juce::Rectangle<float> (0, 0, width, height).reduced (2.0f);

        // Background (machine gray surface)
        g.setColour (findColour (juce::ComboBox::backgroundColourId)
                         .contrasting (isButtonDown ? 0.2f : 0.0f));
        g.fillRect (bounds);

        // Thick border (industrial feel)
        g.setColour (findColour (juce::ComboBox::outlineColourId));
        g.drawRect (bounds, 2.0f);

        // Text (technical, monospaced)
        g.setColour (findColour (juce::ComboBox::textColourId));
        g.setFont (juce::FontOptions { 14.0f });
        g.drawText (box.getTextWhenNothingSelected(),
                   bounds.reduced (10, 0),
                   juce::Justification::centredLeft, true);
    }

    //==============================================================================
    // BUTTON WITH SHARP CORNERS
    void drawButtonBackground (juce::Graphics& g, juce::Button& button,
                               const juce::Colour& backgroundColour,
                               bool shouldDrawButtonAsHighlighted,
                               bool shouldDrawButtonAsDown) override
    {
        auto bounds = button.getLocalBounds().toFloat().reduced (2.0f);

        // Background
        g.setColour (backgroundColour
                     .contrasting (shouldDrawButtonAsDown ? 0.3f :
                                   shouldDrawButtonAsHighlighted ? 0.1f : 0.0f));
        g.fillRect (bounds);

        // Thick border (industrial)
        g.setColour (juce::Colour (0xFF404040));
        g.drawRect (bounds, 2.0f);
    }

    //==============================================================================
    // LABEL WITH TECHNICAL TYPOGRAPHY
    void drawLabel (juce::Graphics& g, juce::Label& label) override
    {
        // Technical monospaced font
        g.setColour (findColour (juce::Label::textColourId));
        g.setFont (juce::FontOptions { juce::Font::getDefaultMonospacedFontName(), 14.0f });

        auto bounds = label.getLocalBounds().reduced (2, 0);
        g.drawText (label.getText(), bounds,
                   label.getJustificationType(),
                   true);
    }

    //==============================================================================
    // PANEL WITH VISIBLE GRID LINES (brutalist structure)
    void drawCornerResizer (juce::Graphics& g, int w, int h,
                            bool isMouseOver, bool isMouseDragging) override
    {
        // Visible structural element
        g.setColour (juce::Colour (0xFF404040));
        g.drawRect (0, 0, w, h, 2.0f);

        // Diagonal lines (industrial detail)
        g.drawLine (juce::Line<float> (w - 10.0f, 0.0f, w, 10.0f), 2.0f);
        g.drawLine (juce::Line<float> (w - 20.0f, 0.0f, w, 20.0f), 1.5f);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BrutalistHardwareLookAndFeel)
};
