/*
  ==============================================================================

    DesignSystem.h
    White Room Unified Design System for All Instruments

    Provides centralized theme management and LookAndFeel access.
    All instruments share the same design system for consistency.

    Available Themes:
    - BrutalistHardwareLookAndFeel (Industrial Orange)
    - RetroFuturisticTronLookAndFeel (Neon Cyan/Magenta)
    - OrganicStudioLookAndFeel (Warm Terracotta)
    - RobotechLookAndFeel (Mecha Red/White - NEW)
    - UltraMinimalLookAndFeel (Monochrome Swiss - NEW)

  ==============================================================================
*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "themes/BrutalistHardwareLookAndFeel.h"
#include "themes/RetroFuturisticTronLookAndFeel.h"
#include "themes/OrganicStudioLookAndFeel.h"
#include "themes/RobotechLookAndFeel.h"
#include "themes/UltraMinimalLookAndFeel.h"

namespace WhiteRoomDesign {

//==============================================================================
// Theme Enumeration
enum class ThemeID
{
    brutalistHardware,
    retroFuturisticTron,
    organicStudio,
    robotech,
    ultraMinimal
};

//==============================================================================
// Theme Manager
class ThemeManager
{
public:
    static ThemeManager& getInstance()
    {
        static ThemeManager instance;
        return instance;
    }

    // Get LookAndFeel for a theme
    juce::LookAndFeel* getLookAndFeel (ThemeID theme)
    {
        switch (theme)
        {
            case ThemeID::brutalistHardware:
                if (!brutalistLF) brutalistLF = std::make_unique<BrutalistHardwareLookAndFeel>();
                return brutalistLF.get();

            case ThemeID::retroFuturisticTron:
                if (!tronLF) tronLF = std::make_unique<RetroFuturisticTronLookAndFeel>();
                return tronLF.get();

            case ThemeID::organicStudio:
                if (!organicLF) organicLF = std::make_unique<OrganicStudioLookAndFeel>();
                return organicLF.get();

            case ThemeID::robotech:
                if (!robotechLF) robotechLF = std::make_unique<RobotechLookAndFeel>();
                return robotechLF.get();

            case ThemeID::ultraMinimal:
                if (!ultraMinimalLF) ultraMinimalLF = std::make_unique<UltraMinimalLookAndFeel>();
                return ultraMinimalLF.get();
        }

        jassertfalse;
        return nullptr;
    }

    // Get theme name
    static juce::String getThemeName (ThemeID theme)
    {
        switch (theme)
        {
            case ThemeID::brutalistHardware: return "Brutalist Hardware";
            case ThemeID::retroFuturisticTron: return "Retro Futuristic Tron";
            case ThemeID::organicStudio: return "Organic Studio";
            case ThemeID::robotech: return "Robotech";
            case ThemeID::ultraMinimal: return "Ultra Minimal";
        }
        return "Unknown";
    }

    // Get all available themes
    static juce::Array<ThemeID> getAllThemes()
    {
        return { ThemeID::brutalistHardware,
                 ThemeID::retroFuturisticTron,
                 ThemeID::organicStudio,
                 ThemeID::robotech,
                 ThemeID::ultraMinimal };
    }

    // Apply theme to component
    static void applyTheme (juce::Component* component, ThemeID theme)
    {
        if (auto* lf = getInstance().getLookAndFeel (theme))
            component->setLookAndFeel (lf);
    }

private:
    ThemeManager() = default;

    std::unique_ptr<BrutalistHardwareLookAndFeel> brutalistLF;
    std::unique_ptr<RetroFuturisticTronLookAndFeel> tronLF;
    std::unique_ptr<OrganicStudioLookAndFeel> organicLF;
    std::unique_ptr<RobotechLookAndFeel> robotechLF;
    std::unique_ptr<UltraMinimalLookAndFeel> ultraMinimalLF;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ThemeManager)
};

//==============================================================================
// Convenience Functions
inline void applyBrutalistTheme (juce::Component* c) { ThemeManager::applyTheme (c, ThemeID::brutalistHardware); }
inline void applyTronTheme (juce::Component* c) { ThemeManager::applyTheme (c, ThemeID::retroFuturisticTron); }
inline void applyOrganicTheme (juce::Component* c) { ThemeManager::applyTheme (c, ThemeID::organicStudio); }
inline void applyRobotechTheme (juce::Component* c) { ThemeManager::applyTheme (c, ThemeID::robotech); }
inline void applyUltraMinimalTheme (juce::Component* c) { ThemeManager::applyTheme (c, ThemeID::ultraMinimal); }

} // namespace WhiteRoomDesign
