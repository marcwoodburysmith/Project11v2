/*
  ==============================================================================

    FilterParametersBase.h
    Created: 10 Aug 2024 10:03:20am
    Author:  Marc Woodbury-Smith

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

struct FilterParametersBase
{
    float frequency {440.f};
    bool bypassed {false};
    float quality {1.f};
    double sampleRate {44100};
    
};
