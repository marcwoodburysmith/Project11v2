/*
  ==============================================================================

    Meter.h
    Created: 7 Jan 2025 5:23:53pm
    Author:  Marc Woodbury-Smith

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "DecayingValueHolder.h"
#include "EQConstants.h"
#include "Averager.h"


#define TICK_INTERVAL 6
#define DECAY_BAR_THICK 4.f

struct Meter : juce::Component
{
    Meter();
    ~Meter() override;
    
    void paint (juce::Graphics&) override;
    void update(float dbLevel);
    
private:
    
    void paintBar (juce::Graphics& g, float value, juce::Rectangle<float> bounds, float dWidth, juce::Colour color);

    
    float peakDb { NEGATIVE_INFINITY };
    
    DecayingValueHolder decayingValueHolder;
    
    Averager<float> averager {static_cast<size_t>(FRAME_RATE * AVG_TIME), NEGATIVE_INFINITY};
};
