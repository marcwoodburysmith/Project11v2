/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================

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

//==============================================================================

struct FilterParametersBase
{
    float frequency {440.f};
    bool bypassed {false};
    float quality {1.f};
    double sampleRate {44100};
    
};

struct FilterParameters : public FilterParametersBase
{
    FilterInfo::FilterType filterType {FilterInfo::FilterType::LowPass};
    float gainInDecibels {0.0f};
    
};

struct HighCutLowCutParameters : public FilterParametersBase
{
    int order {1};
    bool isLowcut {true};
    
};





//==============================================================================
/**
*/
class Project11v2AudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    Project11v2AudioProcessor();
    ~Project11v2AudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Project11v2AudioProcessor)
};
