/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Meter.h"
#include "DbScale.h"
#include "StereoMeter.h"



//==============================================================================
/**
*/
class Project11v2AudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    Project11v2AudioProcessorEditor (Project11v2AudioProcessor&);
    ~Project11v2AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void timerCallback() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Project11v2AudioProcessor& audioProcessor;
    
    juce::AudioBuffer<float> buffer;
    
    Meter inputMeter;
    
    DbScale inputScale;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Project11v2AudioProcessorEditor)
};
