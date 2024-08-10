/*
  ==============================================================================

    FilterInfo.h
    Created: 10 Aug 2024 10:02:41am
    Author:  Marc Woodbury-Smith

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

namespace FilterInfo
{

enum class FilterType
{
    FirstOrderLowPass,
    FirstOrderHighPass,
    FirstOrderAllPass,
    LowPass,
    HighPass,
    BandPass,
    Notch,
    AllPass,
    LowShelf,
    HighShelf,
    Peak
};

enum class Slope
  {
      Slope_6,
      Slope_12,
      Slope_18,
      Slope_24,
      Slope_30,
      Slope_36,
      Slope_42,
      Slope_48
  };

inline const std::map<FilterType, juce::String>& GetParams()
{
    static std::map<FilterType, juce::String> params =
    {
        {FilterType::FirstOrderLowPass, "FirstOrder LowPass"},
        {FilterType::FirstOrderHighPass, "FirstOrder HighPass"},
        {FilterType::FirstOrderAllPass, "FirstOrder AllPass"},
        {FilterType::LowPass, "LowPass"},
        {FilterType::HighPass, "HighPass"},
        {FilterType::BandPass, "BandPass"},
        {FilterType::Notch, "Notch"},
        {FilterType::AllPass, "Allpass"},
        {FilterType::LowShelf, "LowShelf"},
        {FilterType::HighShelf, "HighShelf"},
        {FilterType::Peak, "Peak"}
    };

    return params;
}

const std::map<FilterType, juce::String> filterToString
{
    {FilterType::FirstOrderLowPass, "FirstOrder LowPass"},
    {FilterType::FirstOrderHighPass, "FirstOrder HighPass"},
    {FilterType::FirstOrderAllPass, "FirstOrder AllPass"},
    {FilterType::LowPass, "LowPass"},
    {FilterType::HighPass, "HighPass"},
    {FilterType::BandPass, "BandPass"},
    {FilterType::Notch, "Notch"},
    {FilterType::AllPass, "Allpass"},
    {FilterType::LowShelf, "LowShelf"},
    {FilterType::HighShelf, "HighShelf"},
    {FilterType::Peak, "Peak"}
};

const std::map<Slope, juce::String> slopeToString
{
    {Slope::Slope_6,"6 dB/Oct"},
    {Slope::Slope_12,"12 dB/Oct"},
    {Slope::Slope_18,"18 dB/Oct"},
    {Slope::Slope_24,"24 dB/Oct"},
    {Slope::Slope_30,"30 dB/Oct"},
    {Slope::Slope_36,"36 dB/Oct"},
    {Slope::Slope_42,"42 dB/Oct"},
    {Slope::Slope_48,"48 dB/Oct"}
};

} //end namespace FilterInfo

