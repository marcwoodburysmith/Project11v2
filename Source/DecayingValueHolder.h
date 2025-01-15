/*
  ==============================================================================

    DecayingValueHolder.h
    Created: 15 Jan 2025 8:36:28am
    Author:  Marc Woodbury-Smith

  ==============================================================================
*/

#pragma once


#include <JuceHeader.h>
#include "EQConstants.h"



struct DecayingValueHolder : juce::Timer
{
    DecayingValueHolder();
    
    void updateHeldValue(float input);
    
    float getCurrentValue() const;
    bool isOverThreshold() const;
    
    void setHoldTime(int ms);
    void setDecayRate(float dbPerSec);
    
    void timerCallback() override;
    
private:
    float currentValue { NEGATIVE_INFINITY };
    juce::int64 peakTime = getNow();
    float threshold = 0.f;
    juce::int64 holdTime = 2000; //2 seconds
    float decayRatePerFrame { 0 };
    float decayRateMultiplier { 1 };
    
    static juce::int64 getNow();
   
    void resetDecayRateMultiplier();
    
    static const int frameRate {60};
    
    constexpr static float rateAccel { 1.01f };

};
