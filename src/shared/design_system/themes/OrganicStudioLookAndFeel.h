/*
  ==============================================================================

    OrganicStudioLookAndFeel.h
    JUCE LookAndFeel implementation of White Room's Organic Studio Theme

    Design Specs from: white_room/docs/design-system/THEME_GALLERY.md
    - Primary Accent: #D48A5E (Terracotta - earthy warmth)
    - Secondary Accent: #8B6914 (Olive - natural)
    - Backgrounds: #F5F0E8 → #D8D0C0 (Cream to beige)
    - Corner Radius: 4-16px (rounded, organic)
    - Border Width: 1px soft beige (subtle)
    - Typography: Warm, rounded, friendly

  ==============================================================================
*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

//==============================================================================
class OrganicStudioLookAndFeel  : public juce::LookAndFeel_V4
{
public:
    OrganicStudioLookAndFeel()
    {
        // Warm, Natural Palette
        setColour (juce::ResizableWindow::backgroundColourId, juce::Colour (0xFFF5F0E8));
        setColour (juce::Slider::thumbColourId, juce::Colour (0xFFD48A5E)); // Terracotta
        setColour (juce::Slider::trackColourId, juce::Colour (0xFFD8D0C0));
        setColour (juce::Slider::backgroundColourId, juce::Colour (0xFFE8E0D0));
        setColour (juce::ComboBox::backgroundColourId, juce::Colour (0xFFD8D0C0));
        setColour (juce::ComboBox::textColourId, juce::Colour (0xFF8B7355)); // Wood tone
        setColour (juce::ComboBox::outlineColourId, juce::Colour (0xFF8B7355));
        setColour (juce::Label::textColourId, juce::Colour (0xFF8B7355));
        setColour (juce::TextButton::buttonColourId, juce::Colour (0xFF8B7355));
        setColour (juce::TextButton::textColourOnId, juce::Colour (0xFFFFFFFF));
        setColour (juce::TextButton::textColourOffId, juce::Colour (0xFFF5F0E8));
    }

    void drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           const juce::Slider::SliderStyle style, juce::Slider& slider) override
    {
        auto bounds = slider.getLocalBounds().toFloat();

        // Track (soft cream)
        g.setColour (findColour (juce::Slider::backgroundColourId));
        juce::Path p;
        p.addRoundedRectangle (bounds, 8.0f);
        g.fillPath (p);

        // Fill (terracotta accent)
        auto trackBounds = bounds;
        if (slider.isHorizontal())
            trackBounds.setWidth (sliderPos - x);
        else
            trackBounds.setTop (sliderPos);

        g.setColour (findColour (juce::Slider::thumbColourId));
        juce::Path fillPath;
        fillPath.addRoundedRectangle (trackBounds, 8.0f);
        g.fillPath (fillPath);

        // Thumb (rounded, organic)
        auto thumbBounds = bounds;
        if (slider.isHorizontal())
        {
            thumbBounds.setWidth (16.0f);
            thumbBounds.setCentre (bounds.getCentre().withX (sliderPos));
        }
        else
        {
            thumbBounds.setHeight (16.0f);
            thumbBounds.setCentre (bounds.getCentre().withY (sliderPos));
        }

        g.setColour (findColour (juce::Slider::thumbColourId));
        juce::Path thumbPath;
        thumbPath.addEllipse (thumbBounds);
        g.fillPath (thumbPath);

        // Soft shadow
        auto shadowPath = thumbPath;
        g.setColour (juce::Colour (0x40000000));
        g.fillPath (shadowPath, juce::AffineTransform::translation (2.0f, 2.0f));

        // Subtle border
        g.setColour (juce::Colour (0xFF8B7355).withAlpha (0.3f));
        juce::Path borderPath;
        borderPath.addRoundedRectangle (bounds, 8.0f);
        g.strokePath (borderPath, juce::PathStrokeType (1.0f));
    }

    void drawComboBox (juce::Graphics& g, int width, int height, bool isButtonDown,
                       int buttonX, int buttonY, int buttonW, int buttonH,
                       juce::ComboBox& box) override
    {
        auto bounds = juce::Rectangle<float> (0, 0, width, height).reduced (2.0f);

        // Background (warm beige)
        g.setColour (findColour (juce::ComboBox::backgroundColourId)
                         .contrasting (isButtonDown ? 0.1f : 0.0f));
        juce::Path p;
        p.addRoundedRectangle (bounds, 6.0f);
        g.fillPath (p);

        // Subtle border
        g.setColour (findColour (juce::ComboBox::outlineColourId).withAlpha (0.5f));
        g.strokePath (p, juce::PathStrokeType (1.0f));

        // Text (warm, friendly)
        g.setColour (findColour (juce::ComboBox::textColourId));
        g.setFont (juce::FontOptions { 14.0f }); // System font (rounded on macOS)
        g.drawText (box.getTextWhenNothingSelected(),
                   bounds.reduced (10, 0),
                   juce::Justification::centredLeft, true);
    }

    void drawLabel (juce::Graphics& g, juce::Label& label) override
    {
        g.setColour (findColour (juce::Label::textColourId));
        g.setFont (juce::FontOptions { 14.0f }); // System rounded font

        auto bounds = label.getLocalBounds().reduced (2, 0);
        g.drawText (label.getText(), bounds,
                   label.getJustificationType(),
                   true);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OrganicStudioLookAndFeel)
};
