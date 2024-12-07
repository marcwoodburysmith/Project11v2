/*
  ==============================================================================

    ParameterHelpers.h
    Created: 10 Aug 2024 10:04:41am
    Author:  Marc Woodbury-Smith

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>


enum class Channel {Left, Right};


enum class ChannelMode {Stereo, DualMono, MidSide};

const std::map<ChannelMode,juce::String>  mapModeToString
{
    {ChannelMode::Stereo, "Stereo"},
    {ChannelMode::DualMono, "DualMono"},
    {ChannelMode::MidSide, "Mid-Side"},
};

juce::String ParamString(juce::String label, int filterNum);

juce::String generateGainParamString(Channel channel, int filterNum);

juce::String generateQParamString(Channel channel, int filterNum);

juce::String generateFreqParamString(Channel channel, int filterNum);

juce::String generateTypeParamString(Channel channel, int filterNum);

juce::String generateBypassParamString(Channel channel, int filterNum);

juce::String generateSlopeParamString(Channel channel, int filterNum);

juce::String generateChannelString(Channel channel);




