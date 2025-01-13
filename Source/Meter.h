/*
  ==============================================================================

    Meter.h
    Created: 7 Jan 2025 5:23:53pm
    Author:  Marc Woodbury-Smith

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

#define NEGATIVE_INFINITY -66.f
#define MAX_DECIBELS 12.f
#define TICK_INTERVAL 6

struct Meter : juce::Component
{
    Meter();
    ~Meter() override;
    
    void paint (juce::Graphics&) override;
    void update(float dbLevel);
    
private:
    float peakDb { NEGATIVE_INFINITY };
};
