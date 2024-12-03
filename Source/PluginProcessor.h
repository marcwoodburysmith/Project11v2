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
#include "Fifo.h"
#include "FilterCoefficientGenerator.h"
#include "ReleasePool.h"
#include "FilterLink.h"




//using Filter = juce::dsp::IIR::Filter<float>;
//using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;
////using MonoChain = juce::dsp::ProcessorChain<CutFilter, Filter, CutFilter>;
//using ParametricFilter = FilterLink<Filter, FilterCoeffPtr, FilterParameters, CoefficientsMaker>;
//using MonoChain = juce::dsp::ProcessorChain<CutFilter,ParametricFilter,CutFilter>;

using Filter = juce::dsp::IIR::Filter<float>;
using CutChain = juce::dsp::ProcessorChain<Filter,Filter,Filter,Filter>;
using CutFilter = FilterLink<CutChain, CutCoeffArray, HighCutLowCutParameters, CoefficientsMaker>;
using ParametricFilter = FilterLink<Filter, FilterCoeffPtr, FilterParameters, CoefficientsMaker>;

const float rampTime = 0.05f;  //50 mseconds
const int innerLoopSize = 32;
 
//using MonoChain = juce::dsp::ProcessorChain<CutFilter,ParametricFilter,CutFilter>;
using MonoChain = juce::dsp::ProcessorChain<CutFilter,
                                            ParametricFilter,
                                            ParametricFilter,
                                            ParametricFilter,
                                            ParametricFilter,
                                            ParametricFilter,
                                            ParametricFilter,
                                            CutFilter>;



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
    
    void addFilterParamToLayout(juce::AudioProcessorValueTreeState::ParameterLayout&, int, bool);

private:
    template <const int filterNum>
    FilterParameters getParametericFilterParams(double sampleRate)
    {
       using namespace FilterInfo;
         
       FilterType filterType = static_cast<FilterType> (apvts.getRawParameterValue(generateTypeParamString(filterNum))->load());
    
       
       float frequency = apvts.getRawParameterValue(generateFreqParamString(filterNum))->load();
       float quality  = apvts.getRawParameterValue(generateQParamString(filterNum))->load();
       bool bypassed = apvts.getRawParameterValue(generateBypassParamString(filterNum))->load() > 0.5f;
         
       FilterParameters parametricParams;
             
      parametricParams.frequency = frequency;
      parametricParams.filterType = filterType;
      parametricParams.sampleRate = sampleRate;
      parametricParams.quality = quality;
      parametricParams.bypassed = bypassed;
      parametricParams.gain = Decibel <float> (apvts.getRawParameterValue(generateGainParamString(filterNum))-> load());
 
      return parametricParams;
                        

       }
    
    
    template <const int filterNum>
       HighCutLowCutParameters getCutFilterParams(double sampleRate,bool isLowCut)
       {
           using namespace FilterInfo;
           
           float frequency = apvts.getRawParameterValue(generateFreqParamString(filterNum))->load();
           bool bypassed = apvts.getRawParameterValue(generateBypassParamString(filterNum))->load() > 0.5f;
           
           Slope slope = static_cast<Slope> (apvts.getRawParameterValue(generateSlopeParamString(filterNum))->load());
           
           HighCutLowCutParameters cutParams;
               
           cutParams.isLowcut = isLowCut;
           cutParams.frequency = frequency;
           cutParams.bypassed = bypassed;
           cutParams.order = static_cast<int>(slope) + 1;
           cutParams.sampleRate = sampleRate;
           cutParams.quality  = 1.0f; //not used for cut filters
           
           return cutParams;
           
       }

    
    
//    template <const int filterNum>
//       void updateParametricFilter(double sampleRate)
//      {
//           
//           FilterParameters parametricParams = getParametericFilterParams<filterNum>(sampleRate);
//           
//           leftChain.get<filterNum>().performPreLoopUpdate(parametricParams);
//           leftChain.get<filterNum>().performInnerLoopFilterUpdate(true,0);
//           rightChain.get<filterNum>().performPreLoopUpdate(parametricParams);
//           rightChain.get<filterNum>().performInnerLoopFilterUpdate(true,0);
//           
//      }
//    
//    template <const int filterNum>
//       void updateCutFilter(double sampleRate, bool isLowCut)
//       {
//    
//           
//           HighCutLowCutParameters cutParams = getCutFilterParams<filterNum>(sampleRate, isLowCut);
//               
//           leftChain.get<filterNum>().performPreLoopUpdate(cutParams);
//           leftChain.get<filterNum>().performInnerLoopFilterUpdate(true,0);
//           rightChain.get<filterNum>().performPreLoopUpdate(cutParams);
//           rightChain.get<filterNum>().performInnerLoopFilterUpdate(true,0);
//           
//       }
//    
    template <const int filterNum>
    void preUpdateParametricFilter(double sampleRate)
    {
        FilterParameters parametricParams = getParametericFilterParams<filterNum>(sampleRate);
        
        leftChain.get<filterNum>().performPreLoopUpdate(parametricParams);
        rightChain.get<filterNum>().performPreLoopUpdate(parametricParams);
    }
    
    
    template <const int filterNum>
    void loopUpdateParametricFilter(double sampleRate, int samplesToSkip)
    {
        leftChain.get<filterNum>().performInnerLoopFilterUpdate(true, samplesToSkip);
        rightChain.get<filterNum>().performInnerLoopFilterUpdate(true, samplesToSkip);
    }
    
        
        
    template <const int filterNum>
    void preUpdateCutFilter(double sampleRate, bool isLowCut)
    {
        HighCutLowCutParameters cutParams = getCutFilterParams<filterNum>(sampleRate, isLowCut);
            
        leftChain.get<filterNum>().performPreLoopUpdate(cutParams);
        rightChain.get<filterNum>().performPreLoopUpdate(cutParams);
   
    }
    
    template <const int filterNum>
    void loopUpdateCutFilter(double sampleRate, bool isLowCut, int samplesToSkip)
    {
        leftChain.get<filterNum>().performInnerLoopFilterUpdate(true, samplesToSkip);
        rightChain.get<filterNum>().performInnerLoopFilterUpdate(true, samplesToSkip);
    }
        

    
    template <const int filterNum, typename ParamType>
    void initializeChain(ParamType params, bool onRealTimeThread, double sampleRate)
    {
        leftChain.get<filterNum>().initialize(params, rampTime, onRealTimeThread, sampleRate);
        rightChain.get<filterNum>().initialize(params, rampTime, onRealTimeThread, sampleRate);
    }


    void initializeFilters(double sampleRate);
    void performInnerLoopUpdate(double sampleRate, int samplesToSkip);
    void performPreLoopUpdate(double sampleRate);


//    void updateFilters(double sampleRate);
    
    MonoChain leftChain, rightChain;
    
    
    

    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Project11v2AudioProcessor)
};
