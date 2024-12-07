/*
  ==============================================================================

    ParameterHelpers.cpp
    Created: 10 Aug 2024 10:04:41am
    Author:  Marc Woodbury-Smith

  ==============================================================================
*/

#include "ParameterHelpers.h"
#include <JuceHeader.h>


juce::String generateChannelString(Channel channel)
{
   if (channel == Channel::Left)
       return "left_";
    return "right_";
}


juce::String ParamString(Channel channel, juce::String label, int filterNum)
{
//    return "Filter_" + juce::String(filterNum)+"_"+ label;
    return "Filter_" + generateChannelString(channel) + juce::String(filterNum)+"_"+ label;
}


juce::String generateGainParamString(Channel channel, int filterNum)
{
    return ParamString(channel, "gain",filterNum);
}

juce::String generateQParamString(Channel channel, int filterNum)
{
    return ParamString(channel, "Q",filterNum);
}

juce::String generateFreqParamString(Channel channel, int filterNum)
{
    return ParamString(channel, "freq",filterNum);
}

juce::String generateTypeParamString(Channel channel, int filterNum)
{
    return ParamString(channel, "type",filterNum);
}

juce::String generateBypassParamString(Channel channel, int filterNum)
{
    return ParamString(channel, "bypass",filterNum);
}

juce::String generateSlopeParamString(Channel channel, int filterNum)
{
    return ParamString(channel, "slope",filterNum);
}


