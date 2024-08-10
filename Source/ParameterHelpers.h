/*
  ==============================================================================

    ParameterHelpers.h
    Created: 10 Aug 2024 10:04:41am
    Author:  Marc Woodbury-Smith

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>


juce::String ParamString(juce::String label, int filterNum);

juce::String generateGainParamString(int filterNum);

juce::String generateQParamString(int filterNum);

juce::String generateFreqParamString(int filterNum);

juce::String generateTypeParamString(int filterNum);

juce::String generateBypassParamString(int filterNum);

juce::String generateSlopeParamString(int filterNum);




