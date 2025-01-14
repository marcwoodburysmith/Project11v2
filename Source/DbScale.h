/*
  ==============================================================================

    DbScale.h
    Created: 12 Jan 2025 6:50:41am
    Author:  Marc Woodbury-Smith

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


struct Tick
{
    float db { 0.f };
    int y { 0 };
};

struct DbScale : juce::Component
{
    DbScale(){}
    ~DbScale() override = default;
    
    void paint (juce::Graphics& g) override;
    void buildBackgroundImage(int dbDivision, juce::Rectangle<int> meterBounds, int minDb, int maxDb);
    static std::vector<Tick> getTicks(int dbDivision, juce::Rectangle<int> meterBounds, int minDb, int maxDb);
    
private:
    juce::Image bkgd;
    const int textHeight = 14;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DbScale)
};
