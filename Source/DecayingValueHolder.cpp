/*
  ==============================================================================

    DecayingValueHolder.cpp
    Created: 15 Jan 2025 8:36:28am
    Author:  Marc Woodbury-Smith

  ==============================================================================
*/

#include "DecayingValueHolder.h"


DecayingValueHolder::DecayingValueHolder()
{
    setDecayRate(3);
    startTimerHz(60);
    
}

void DecayingValueHolder::updateHeldValue(float input)
{
    if (input > currentValue)
    {
        peakTime = getNow();
        currentValue = input;
        resetDecayRateMultiplier();
    }
}

float DecayingValueHolder::getCurrentValue() const
{
    return currentValue;
}


bool DecayingValueHolder::isOverThreshold() const
{
    return currentValue > threshold;
}


void DecayingValueHolder::setHoldTime(int ms)
{
    holdTime = ms;
}

void DecayingValueHolder::setDecayRate(float dbPerSec)
{
    decayRatePerFrame = dbPerSec / frameRate;
}

void DecayingValueHolder::timerCallback()
{
    auto now = getNow();
    if ( now - peakTime > holdTime )
    {

        currentValue -= (decayRatePerFrame * decayRateMultiplier);
        
        currentValue = juce::jlimit(NEGATIVE_INFINITY, MAX_DECIBELS, currentValue);
        decayRateMultiplier *= rateAccel;
        if (currentValue == NEGATIVE_INFINITY)
        {
            resetDecayRateMultiplier();
        }
    }
}

juce::int64 DecayingValueHolder::getNow()
{
    return juce::Time::currentTimeMillis();
}

void DecayingValueHolder::resetDecayRateMultiplier()
{
    decayRateMultiplier = 1.f;
}
