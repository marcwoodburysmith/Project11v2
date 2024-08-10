/*
  ==============================================================================

    HighCutLowCutParameters.h
    Created: 10 Aug 2024 10:03:40am
    Author:  Marc Woodbury-Smith

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

struct HighCutLowCutParameters : public FilterParametersBase
{
    int order {1};
    bool isLowcut {true};
    
};


