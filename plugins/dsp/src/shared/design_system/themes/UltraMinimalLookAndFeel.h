/*
  ==============================================================================

    UltraMinimalLookAndFeel.h
    JUCE LookAndFeel implementation of White Room's Ultra Minimal Theme

    Inspired by: Swiss design, Dieter Rams,极致简约 (Ultimate Simplicity)
    - Primary Accent: #000000 (Pure Black)
    - Secondary Accent: #FFFFFF (Pure White)
    - Backgrounds: #FFFFFF → #F5F5F5 (Pure whites)
    - Corner Radius: 0px (Swiss precision)
    - Border Width: 1px (hairline)
    - Typography: Helvetica/Inter, Swiss grid system

    "Less is more" - Extreme minimalism with focus on function over form

  ==============================================================================
*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

//==============================================================================
class UltraMinimalLookAndFeel  : public juce::LookAndFeel_V4
{
public:
    UltraMinimalLookAndFeel()
    {
        // Ultra minimal palette - pure black and white
        // Background: Pure white
        setColour (juce::ResizableWindow::backgroundColourId, juce::Colour (0xFFFFFFFF));
        setColour (juce::Slider::thumbColourId, juce::Colour (0xFF000000));
        setColour (juce::Slider::trackColourId, juce::Colour (0xFFE5E5E5));
        setColour (juce::Slider::backgroundColourId, juce::Colour (0xFFF5F5F5));
        setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xFF000000));
        setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour (0xFFE5E5E5));

        // UI Elements
        setColour (juce::ComboBox::backgroundColourId, juce::Colour (0xFFFFFFFF));
        setColour (juce::ComboBox::textColourId, juce::Colour (0xFF000000));
        setColour (juce::ComboBox::outlineColourId, juce::Colour (0xFF000000));

        // Text colors
        setColour (juce::Label::textColourId, juce::Colour (0xFF000000));
        setColour (juce::TextEditor::textColourId, juce::Colour (0xFF000000));
        setColour (juce::TextEditor::backgroundColourId, juce::Colour (0xFFFFFFFF));
        setColour (juce::TextEditor::outlineColourId, juce::Colour (0xFF000000));

        // Buttons
        setColour (juce::TextButton::buttonColourId, juce::Colour (0xFFFFFFFF));
        setColour (juce::TextButton::textColourOnId, juce::Colour (0xFF000000));
        setColour (juce::TextButton::textColourOffId, juce::Colour (0xFF000000));

        // Scrollbars (minimal)
        setColour (juce::ScrollBar::thumbColourId, juce::Colour (0xFF000000));
        setColour (juce::ScrollBar::trackColourId, juce::Colour (0xFFF5F5F5));

        // Alert windows
        setColour (juce::AlertWindow::backgroundColourId, juce::Colour (0xFFFFFFFF));
        setColour (juce::AlertWindow::textColourId, juce::Colour (0xFF000000));

        // Popup menu
        setColour (juce::PopupMenu::backgroundColourId, juce::Colour (0xFFFFFFFF));
        setColour (juce::PopupMenu::textColourId, juce::Colour (0xFF000000));
        setColour (juce::PopupMenu::highlightedBackgroundColourId, juce::Colour (0xFFF5F5F5));

        // Tooltip
        setColour (juce::TooltipWindow::backgroundColourId, juce::Colour (0xFF000000));
        setColour (juce::TooltipWindow::textColourId, juce::Colour (0xFFFFFFFF));

        // Toggle buttons
        setColour (juce::ToggleButton::tickColourId, juce::Colour (0xFF000000));
        setColour (juce::ToggleButton::tickDisabledColourId, juce::Colour (0xFFCCCCCC));
    }

    //==============================================================================
    // SWISS PRECISION (0px radius - sharp)
    int getSliderThumbRadius (juce::Slider& slider) override
    {
        return 6;
    }

    void drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           const juce::Slider::SliderStyle style, juce::Slider& slider) override
    {
        auto bounds = slider.getLocalBounds().toFloat();

        // Track background (pure white)
        g.setColour (findColour (juce::Slider::backgroundColourId));
        g.fillRect (bounds);

        // Track border (hairline)
        g.setColour (juce::Colour (0xFFE5E5E5));
        g.drawRect (bounds.reduced (1), 1.0f);

        // Fill (black)
        g.setColour (juce::Colour (0xFF000000));
        if (slider.isHorizontal())
            g.fillRect (juce::Rectangle<float> (x + 2, y + 2, sliderPos - x - 4, height - 4));
        else
            g.fillRect (juce::Rectangle<float> (x + 2, y + 2, width - 4, sliderPos - y - 4));

        // Thumb (black square - Swiss minimal)
        if (slider.isHorizontal())
        {
            auto thumbWidth = 12.0f;
            g.fillRect (juce::Rectangle<float> (sliderPos - thumbWidth / 2, y + 1, thumbWidth, height - 2));
        }
        else
        {
            auto thumbHeight = 12.0f;
            g.fillRect (juce::Rectangle<float> (x + 1, sliderPos - thumbHeight / 2, width - 2, thumbHeight));
        }
    }

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                            float sliderPosProportional, float rotaryStartAngle,
                            float rotaryEndAngle, juce::Slider& slider) override
    {
        auto radius = (float) juce::jmin (width, height) / 2.0f - 2.0f;
        auto centerX = (float) x + (float) width * 0.5f;
        auto centerY = (float) y + (float) height * 0.5f;
        auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

        // Clean white circle
        g.setColour (juce::Colour (0xFFFFFFFF));
        g.fillEllipse (juce::Rectangle<float> (radius * 2.0f, radius * 2.0f)
                        .withCentre (juce::Point<float> (centerX, centerY)));

        // Hairline border
        g.setColour (juce::Colour (0xFFE5E5E5));
        g.drawEllipse (juce::Rectangle<float> (radius * 2.0f, radius * 2.0f)
                        .withCentre (juce::Point<float> (centerX, centerY)), 1.0f);

        // Value indicator (black line - Swiss precision)
        g.setColour (juce::Colour (0xFF000000));
        auto indicatorLength = radius - 6.0f;
        auto lineEnd = juce::Point<float> (centerX + std::cos (angle) * indicatorLength,
                                           centerY + std::sin (angle) * indicatorLength);
        g.drawLine (juce::Line<float> (juce::Point<float> (centerX, centerY), lineEnd), 2.0f);

        // Minimal thumb (black dot)
        g.fillEllipse (juce::Rectangle<float> (8, 8).withCentre (lineEnd));
    }

    //==============================================================================
    // BUTTONS (Swiss style - clean rectangles)
    void drawButtonBackground (juce::Graphics& g, juce::Button& button,
                               const bool shouldDrawButtonAsHighlighted,
                               const bool shouldDrawButtonAsDown) override
    {
        auto bounds = button.getLocalBounds().toFloat();

        if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
            g.setColour (juce::Colour (0xFFF5F5F5));
        else
            g.setColour (juce::Colour (0xFFFFFFFF));

        // Clean rectangle (no radius - Swiss precision)
        g.fillRect (bounds);

        // Hairline border (minimal)
        g.setColour (juce::Colour (0xFF000000));
        g.drawRect (bounds.reduced (0.5f), 1.0f);
    }

    //==============================================================================
    // TYPOGRAPHY (Helvetica/Inter - Swiss design)
    juce::Font getTextButtonFont (juce::TextButton&, int buttonHeight) override
    {
        return juce::Font (juce::Font::getDefaultSansSerifFontName(), 13.0f, juce::Font::plain);
    }

    juce::Font getLabelFont (juce::Label& label) override
    {
        return juce::Font (juce::Font::getDefaultSansSerifFontName(), 12.0f, juce::Font::plain);
    }

    juce::Font getComboBoxFont (juce::ComboBox&) override
    {
        return juce::Font (juce::Font::getDefaultSansSerifFontName(), 13.0f, juce::Font::plain);
    }

    juce::Font getPopupMenuFont () override
    {
        return juce::Font (juce::Font::getDefaultSansSerifFontName(), 13.0f, juce::Font::plain);
    }
};
