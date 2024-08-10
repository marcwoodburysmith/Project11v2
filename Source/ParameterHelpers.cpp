/*
  ==============================================================================

    ParameterHelpers.cpp
    Created: 10 Aug 2024 10:04:41am
    Author:  Marc Woodbury-Smith

  ==============================================================================
*/

#include "ParameterHelpers.h"
#include <JuceHeader.h>




juce::String ParamString(juce::String label, int filterNum)
{
    return "Filter_" + juce::String(filterNum)+"_"+ label;
}


juce::String generateGainParamString(int filterNum)
{
    return ParamString("gain",filterNum);
}

juce::String generateQParamString(int filterNum)
{
    return ParamString("Q",filterNum);
}

juce::String generateFreqParamString(int filterNum)
{
    return ParamString("freq",filterNum);
}

juce::String generateTypeParamString(int filterNum)
{
    return ParamString("type",filterNum);
}

juce::String generateBypassParamString(int filterNum)
{
    return ParamString("bypass",filterNum);
}

juce::String generateSlopeParamString(int filterNum)
{
    return ParamString("slope",filterNum);
}


