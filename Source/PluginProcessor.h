/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "FilterInfo.h"
#include "FilterParametersBase.h"
#include "FilterParameters.h"
#include "HighCutLowCutParameters.h"
#include "ParameterHelpers.h"
#include "CoefficientsMaker.h"


using Filter = juce::dsp::IIR::Filter<float>;
//using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;
using MonoChain = juce::dsp::ProcessorChain<Filter>;

//using ParamLayout = juce::AudioProcessorValueTreeState::ParameterLayout;

//==============================================================================
/**
*/
class Project11v2AudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    Project11v2AudioProcessor();
    ~Project11v2AudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    juce::AudioProcessorValueTreeState apvts { *this, nullptr, "Parameters", createParameterLayout() };
    
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    

private:
    
    
//    void addFilterParamToLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout,
//                                int filterNum,
//                                bool isCut);
    
    
    MonoChain leftChain, rightChain;
    
    FilterParameters oldParametricParams;
    HighCutLowCutParameters oldCutParams;
    
    FilterInfo::FilterType oldFilterType;
    
    void updateFilters(double sampleRate);
    
    
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Project11v2AudioProcessor)
};
