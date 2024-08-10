/*
  ==============================================================================

    FilterParameters.h
    Created: 10 Aug 2024 10:03:02am
    Author:  Marc Woodbury-Smith

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>


struct FilterParameters : public FilterParametersBase
{
    FilterInfo::FilterType filterType {FilterInfo::FilterType::LowPass};
    float gain {0.0f};
    
};
