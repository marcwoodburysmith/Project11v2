/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

//#define TESTMETER

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
using Trim = juce::dsp::Gain<float>;
using CutChain = juce::dsp::ProcessorChain<Filter,Filter,Filter,Filter>;
using CutFilter = FilterLink<CutChain, CutCoeffArray, HighCutLowCutParameters, CoefficientsMaker>;
using ParametricFilter = FilterLink<Filter, FilterCoeffPtr, FilterParameters, CoefficientsMaker>;

using ParamLayout = juce::AudioProcessorValueTreeState::ParameterLayout;

//enum ChainPos {InputTrim, FilterChain, OutputTrim };

const float rampTime = 0.05f;  //50 mseconds
const int innerLoopSize = 32;
 
//using MonoChain = juce::dsp::ProcessorChain<CutFilter,ParametricFilter,CutFilter>;
//using MonoChain = juce::dsp::ProcessorChain<CutFilter,
//                                            ParametricFilter,
//                                            ParametricFilter,
//                                            ParametricFilter,
//                                            ParametricFilter,
//                                            ParametricFilter,
//                                            ParametricFilter,
//                                            CutFilter>;

using MonoFilterChain = juce::dsp::ProcessorChain<CutFilter,
                                            ParametricFilter,
                                            ParametricFilter,
                                            ParametricFilter,
                                            ParametricFilter,
                                            ParametricFilter,
                                            ParametricFilter,
                                            CutFilter>;

//using MonoChain = juce::dsp::ProcessorChain<Trim,
//                                            MonoFilterChain,
//                                            Trim>;


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
    using ParamLayout = juce::AudioProcessorValueTreeState::ParameterLayout;

    
//    void addFilterParamToLayout(juce::AudioProcessorValueTreeState::ParameterLayout&, int, bool);
    void addFilterParamToLayout(ParamLayout&, Channel, int, bool);
    void createFilterLayouts(ParamLayout& layout, Channel channel);
    
    Fifo<juce::AudioBuffer<float>, 30> inputBuffers;


private:
    template <const int filterNum>
    FilterParameters getParametericFilterParams(Channel channel, double sampleRate)
    {
       using namespace FilterInfo;
         
       
    
       
       float frequency = apvts.getRawParameterValue(generateFreqParamString(channel, filterNum))->load();
       float quality  = apvts.getRawParameterValue(generateQParamString(channel, filterNum))->load();
       bool bypassed = apvts.getRawParameterValue(generateBypassParamString(channel, filterNum))->load() > 0.5f;
        
//      FilterType filterType = static_cast<FilterType> (apvts.getRawParameterValue(generateTypeParamString(channel, filterNum))->load());
        
        FilterType filterType = static_cast<FilterType> (apvts.getRawParameterValue(generateTypeParamString(channel, filterNum))->load());

         
      FilterParameters parametricParams;
             
      parametricParams.frequency = frequency;
      parametricParams.filterType = filterType;
      parametricParams.sampleRate = sampleRate;
      parametricParams.quality = quality;
      parametricParams.bypassed = bypassed;
      parametricParams.gain = Decibel <float> (apvts.getRawParameterValue(generateGainParamString(channel, filterNum))-> load());
 
      return parametricParams;
                        

       }
    
    
    template <const int filterNum>
       HighCutLowCutParameters getCutFilterParams(Channel channel, double sampleRate,bool isLowCut)
       {
           using namespace FilterInfo;
           
           float frequency = apvts.getRawParameterValue(generateFreqParamString(channel, filterNum))->load();
           float quality  = apvts.getRawParameterValue(generateQParamString(channel, filterNum))->load();
           bool bypassed = apvts.getRawParameterValue(generateBypassParamString(channel, filterNum))->load() > 0.5f;
           
           Slope slope = static_cast<Slope> (apvts.getRawParameterValue(generateSlopeParamString(channel, filterNum))->load());
           
           HighCutLowCutParameters cutParams;
               
           cutParams.isLowcut = isLowCut;
           cutParams.frequency = frequency;
           cutParams.bypassed = bypassed;
           cutParams.order = static_cast<int>(slope) + 1;
           cutParams.sampleRate = sampleRate;
           cutParams.quality  = quality;
           
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
    void preUpdateParametricFilter(ChannelMode mode, double sampleRate)
    {
//        FilterParameters parametricParams = getParametericFilterParams<filterNum>(sampleRate);
        FilterParameters parametricParamsLeft = getParametericFilterParams<filterNum>(Channel::Left, sampleRate);
//        FilterParameters parametricParamsRight = getParametericFilterParams<filterNum>(Channel::Right, sampleRate);
        
        FilterParameters parametricParamsRight;
                
        if(mode == ChannelMode::Stereo)
            parametricParamsRight = parametricParamsLeft;
        else
            parametricParamsRight = getParametericFilterParams<filterNum>(Channel::Right, sampleRate);
        
        leftChain.get<filterNum>().performPreLoopUpdate(parametricParamsLeft);
        rightChain.get<filterNum>().performPreLoopUpdate(parametricParamsRight);
    }
    
    
    template <const int filterNum>
    void loopUpdateParametricFilter(int samplesToSkip)
    {
        leftChain.get<filterNum>().performInnerLoopFilterUpdate(true, samplesToSkip);
        rightChain.get<filterNum>().performInnerLoopFilterUpdate(true, samplesToSkip);
    }
    
        
        
    template <const int filterNum>
    void preUpdateCutFilter(ChannelMode mode, double sampleRate, bool isLowCut)
    {
//        HighCutLowCutParameters cutParams = getCutFilterParams<filterNum>(sampleRate, isLowCut);
        HighCutLowCutParameters cutParamsLeft = getCutFilterParams<filterNum>(Channel::Left, sampleRate, isLowCut);
//        HighCutLowCutParameters cutParamsRight = getCutFilterParams<filterNum>(Channel::Right, sampleRate, isLowCut);
        
        HighCutLowCutParameters cutParamsRight;
                
        if(mode == ChannelMode::Stereo)
            cutParamsRight = cutParamsLeft;
        else
            cutParamsRight = getCutFilterParams<filterNum>(Channel::Right, sampleRate, isLowCut);
            
        leftChain.get<filterNum>().performPreLoopUpdate(cutParamsLeft);
        rightChain.get<filterNum>().performPreLoopUpdate(cutParamsRight);
   
    }
    
    template <const int filterNum>
    void loopUpdateCutFilter(int samplesToSkip)
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


    void initializeFilters(Channel channel, double sampleRate);
    void performInnerLoopUpdate(int samplesToSkip);
    void performPreLoopUpdate(ChannelMode mode, double sampleRate);
    void updateTrims();
    
    void performHadamard(juce::dsp::AudioBlock<float>& A, juce::dsp::AudioBlock<float>& B);
    
    void performMidSideTransform(juce::AudioBuffer<float>&);



//    void updateFilters(double sampleRate);
    
//    MonoChain leftChain, rightChain;
    MonoFilterChain leftChain, rightChain;
    Trim inputTrim, outputTrim;
    
    
#ifdef TESTMETER
    juce::dsp::Oscillator<float> testOsc {[] (float x) { return std::sin (x); } , 128};
    juce::dsp::Gain<float> testOscGain;
#endif

    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Project11v2AudioProcessor)
};
