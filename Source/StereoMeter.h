/*
  ==============================================================================

    StereoMeter.h
    Created: 15 Jan 2025 1:55:57pm
    Author:  Marc Woodbury-Smith

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Meter.h"
#include "DbScale.h"
#include "MeterValues.h"


class StereoMeter  : public juce::Component
{
public:
    StereoMeter(juce::String s);
    ~StereoMeter() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void update(MeterValues values);

private:
    
    Meter leftMeter {"L"}, rightMeter {"R"};
    DbScale dBScale;
    
    const juce::String meterLabel;
    
    static const uint labelSpace {30};
    static const uint meterWidth {25};
   
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StereoMeter)
};

