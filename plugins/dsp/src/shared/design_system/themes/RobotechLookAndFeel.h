/*
  ==============================================================================

    RobotechLookAndFeel.h
    JUCE LookAndFeel implementation of White Room's Robotech Theme

    Inspired by: Mecha anime, Japanese robots, military hardware
    - Primary Accent: #E63946 (Mecha Red)
    - Secondary Accent: #F1FAEE (Mecha White)
    - Backgrounds: #1D3557 → #457B9D (Navy blues)
    - Corner Radius: 2-4px (technical, precise)
    - Border Width: 2px (mecha panel lines)
    - Typography: Technical, military-industrial

  ==============================================================================
*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

//==============================================================================
class RobotechLookAndFeel  : public juce::LookAndFeel_V4
{
public:
    RobotechLookAndFeel()
    {
        // Mecha color palette - Japanese mecha inspired
        // Primary: Mecha Red
        setColour (juce::ResizableWindow::backgroundColourId, juce::Colour (0xFF1D3557));
        setColour (juce::Slider::thumbColourId, juce::Colour (0xFFE63946));
        setColour (juce::Slider::trackColourId, juce::Colour (0xFF457B9D));
        setColour (juce::Slider::backgroundColourId, juce::Colour (0xFF1D3557));
        setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xFFE63946));
        setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour (0xFF457B9D));

        // Secondary: Mecha White
        setColour (juce::ComboBox::backgroundColourId, juce::Colour (0xFF457B9D));
        setColour (juce::ComboBox::textColourId, juce::Colour (0xFFF1FAEE));
        setColour (juce::ComboBox::outlineColourId, juce::Colour (0xFFE63946));

        // Text colors
        setColour (juce::Label::textColourId, juce::Colour (0xFFF1FAEE));
        setColour (juce::TextEditor::textColourId, juce::Colour (0xFFF1FAEE));
        setColour (juce::TextEditor::backgroundColourId, juce::Colour (0xFF1D3557));
        setColour (juce::TextEditor::outlineColourId, juce::Colour (0xFFE63946));

        // Buttons
        setColour (juce::TextButton::buttonColourId, juce::Colour (0xFF457B9D));
        setColour (juce::TextButton::textColourOnId, juce::Colour (0xFFFFFFFF));
        setColour (juce::TextButton::textColourOffId, juce::Colour (0xFFF1FAEE));

        // Mecha panel lines
        setColour (juce::ScrollBar::thumbColourId, juce::Colour (0xFFE63946));
        setColour (juce::ScrollBar::trackColourId, juce::Colour (0xFF1D3557));

        // Alert windows
        setColour (juce::AlertWindow::backgroundColourId, juce::Colour (0xFF1D3557));
        setColour (juce::AlertWindow::textColourId, juce::Colour (0xFFF1FAEE));

        // Popup menu
        setColour (juce::PopupMenu::backgroundColourId, juce::Colour (0xFF1D3557));
        setColour (juce::PopupMenu::textColourId, juce::Colour (0xFFF1FAEE));
        setColour (juce::PopupMenu::highlightedBackgroundColourId, juce::Colour (0xFFE63946));

        // Tooltip
        setColour (juce::TooltipWindow::backgroundColourId, juce::Colour (0xFF457B9D));
        setColour (juce::TooltipWindow::textColourId, juce::Colour (0xFFF1FAEE));

        // Toggle buttons
        setColour (juce::ToggleButton::tickColourId, juce::Colour (0xFFE63946));
        setColour (juce::ToggleButton::tickDisabledColourId, juce::Colour (0xFF457B9D));
    }

    //==============================================================================
    // MECHA PRECISION (2-4px radius - technical feel)
    int getSliderThumbRadius (juce::Slider& slider) override
    {
        return 8;
    }

    void drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           const juce::Slider::SliderStyle style, juce::Slider& slider) override
    {
        auto bounds = slider.getLocalBounds().toFloat();

        // Track background (navy)
        g.setColour (findColour (juce::Slider::backgroundColourId));
        g.fillRoundedRectangle (bounds.reduced (2), 3.0f);

        // Fill (mecha red)
        g.setColour (findColour (juce::Slider::trackColourId));
        auto trackBounds = bounds;
        if (slider.isHorizontal())
            trackBounds.setWidth (sliderPos - x);
        else
            trackBounds.setHeight (sliderPos - y);
        g.fillRoundedRectangle (trackBounds.reduced (2), 3.0f);

        // Thumb (mecha white)
        g.setColour (findColour (juce::Slider::thumbColourId));
        if (slider.isHorizontal())
        {
            auto thumbWidth = 16.0f;
            g.fillRoundedRectangle (juce::Rectangle<float> (sliderPos - thumbWidth / 2, y + 2, thumbWidth, height - 4), 2.0f);
        }
        else
        {
            auto thumbHeight = 16.0f;
            g.fillRoundedRectangle (juce::Rectangle<float> (x + 2, sliderPos - thumbHeight / 2, width - 4, thumbHeight), 2.0f);
        }
    }

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                            float sliderPosProportional, float rotaryStartAngle,
                            float rotaryEndAngle, juce::Slider& slider) override
    {
        auto radius = (float) juce::jmin (width, height) / 2.0f - 4.0f;
        auto centreX = (float) x + (float) width * 0.5f;
        auto centerY = (float) y + (float) height * 0.5f;
        auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

        // Background track (navy)
        g.setColour (findColour (juce::Slider::rotarySliderOutlineColourId));
        g.drawEllipse (juce::Rectangle<float> (radius * 2.0f, radius * 2.0f)
                        .withCentre (juce::Point<float> (centreX, centerY)), 2.0f);

        // Mecha panel lines (decoration)
        g.setColour (juce::Colour (0xFF457B9D));
        for (int i = 0; i < 8; ++i)
        {
            auto lineAngle = angle + (i * juce::MathConstants<float>::twoPi / 8.0f);
            auto lineStart = juce::Point<float> (centreX + std::cos (lineAngle) * (radius - 6),
                                                   centerY + std::sin (lineAngle) * (radius - 6));
            auto lineEnd = juce::Point<float> (centreX + std::cos (lineAngle) * (radius - 2),
                                                 centerY + std::sin (lineAngle) * (radius - 2));
            g.drawLine (juce::Line<float> (lineStart, lineEnd), 1.0f);
        }

        // Value arc (mecha red)
        g.setColour (findColour (juce::Slider::rotarySliderFillColourId));
        auto path = juce::Path();
        path.addArc (juce::Rectangle<float> (radius * 2.0f, radius * 2.0f)
                     .withCentre (juce::Point<float> (centreX, centerY)).reduced (4),
                     rotaryStartAngle, angle, true);
        path.applyTransform (juce::AffineTransform::identity
                               .rotated (juce::MathConstants<float>::pi * 0.5f, centreX, centerY));
        g.strokePath (path, juce::PathStrokeType (3.0f));

        // Thumb (mecha white)
        g.setColour (juce::Colour (0xFFF1FAEE));
        auto thumbX = centreX + std::cos (angle) * (radius - 8);
        auto thumbY = centerY + std::sin (angle) * (radius - 8);
        g.fillEllipse (juce::Rectangle<float> (12, 12).withCentre (juce::Point<float> (thumbX, thumbY)));
    }

    //==============================================================================
    // BUTTONS (Mecha panel style)
    void drawButtonBackground (juce::Graphics& g, juce::Button& button,
                               const bool shouldDrawButtonAsHighlighted,
                               const bool shouldDrawButtonAsDown) override
    {
        auto bounds = button.getLocalBounds().toFloat().reduced (2.0f);

        if (shouldDrawButtonAsDown)
            g.setColour (findColour (juce::TextButton::buttonColourId).brighter (0.2f));
        else if (shouldDrawButtonAsHighlighted)
            g.setColour (findColour (juce::TextButton::buttonColourId).brighter (0.1f));
        else
            g.setColour (findColour (juce::TextButton::buttonColourId));

        // Mecha panel with rounded corners
        g.fillRoundedRectangle (bounds, 3.0f);

        // Mecha panel border
        g.setColour (findColour (juce::TextButton::textColourOffId));
        g.drawRoundedRectangle (bounds, 3.0f, 2.0f);

        // Tech details (corner accents)
        auto cornerSize = 6.0f;
        g.setColour (juce::Colour (0xFFE63946));
        auto tl = bounds.getTopLeft();
        auto br = bounds.getBottomRight();
        g.fillRect (juce::Rectangle<float> (tl.x, tl.y, cornerSize, 2.0f));
        g.fillRect (juce::Rectangle<float> (tl.x, tl.y, 2.0f, cornerSize));
        g.fillRect (juce::Rectangle<float> (br.x - cornerSize, br.y - 2.0f, cornerSize, 2.0f));
        g.fillRect (juce::Rectangle<float> (br.x - 2.0f, br.y - cornerSize, 2.0f, cornerSize));
    }

    //==============================================================================
    // TEXT (Technical/military font style)
    juce::Font getTextButtonFont (juce::TextButton&, int buttonHeight) override
    {
        return juce::Font (juce::Font::getDefaultSansSerifFontName(), 14.0f, juce::Font::bold);
    }

    juce::Font getLabelFont (juce::Label& label) override
    {
        return juce::Font (juce::Font::getDefaultSansSerifFontName(), 13.0f, juce::Font::plain);
    }

    juce::Font getComboBoxFont (juce::ComboBox&) override
    {
        return juce::Font (juce::Font::getDefaultSansSerifFontName(), 14.0f, juce::Font::plain);
    }
};
