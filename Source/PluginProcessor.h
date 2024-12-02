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




using Filter = juce::dsp::IIR::Filter<float>;
using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;
//using MonoChain = juce::dsp::ProcessorChain<CutFilter, Filter, CutFilter>;
using ParametricFilter = FilterLink<Filter, FilterCoeffPtr, FilterParameters, CoefficientsMaker>;
using MonoChain = juce::dsp::ProcessorChain<CutFilter,ParametricFilter,CutFilter>;



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
//       void updateParametricFilter(double sampleRate, bool forceUpdate)
         void updateParametricFilter(double sampleRate)
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
         
             
             leftChain.get<filterNum>().performPreLoopUpdate(parametricParams);
             leftChain.get<filterNum>().performInnerLoopFilterUpdate(true,0);
             rightChain.get<filterNum>().performPreLoopUpdate(parametricParams);
             rightChain.get<filterNum>().performInnerLoopFilterUpdate(true,0);
            

       }
       
       template <const int filterNum>
       void updateCutFilter(double sampleRate, bool forceUpdate, HighCutLowCutParameters& oldParams, bool isLowCut)
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
           
           
           
           if (forceUpdate || oldParams != cutParams)
           {
//               auto chainCoefficients = CoefficientsMaker::makeCoefficients(cutParams);
//               decltype(chainCoefficients) newChainCoefficients;
               
               if(isLowCut)
               {
                   //                   lowCutCoeffFifo.push(chainCoefficients);
                   //                   lowCutCoeffFifo.pull(newChainCoefficients);
                   lowCutCoeffGen.changeParameters(cutParams);
               }
               else
               {
                   //                   highCutCoeffFifo.push(chainCoefficients);
                   //                   highCutCoeffFifo.pull(newChainCoefficients);
                   highCutCoeffGen.changeParameters(cutParams);
               }
           }
           
           
           CutCoeffArray newChainCoefficients;
           bool newChainAvailable;
           
           if(isLowCut)
           {
               newChainAvailable = lowCutCoeffFifo.pull(newChainCoefficients);
           }
           else
           {
               newChainAvailable = highCutCoeffFifo.pull(newChainCoefficients);
           }
           
           if (newChainAvailable)
           {
               leftChain.setBypassed<filterNum>(bypassed);
               rightChain.setBypassed<filterNum>(bypassed);
               bypassSubChain<filterNum>();
               //set up the four filters
               if(!bypassed)
               {
                   int order {newChainCoefficients.size()};
                   switch(order)
                   {
                       case 4:
                           updateSingleCut<filterNum,3> (newChainCoefficients, isLowCut);
                       case 3:
                           updateSingleCut<filterNum,2> (newChainCoefficients, isLowCut);
                       case 2:
                           updateSingleCut<filterNum,1> (newChainCoefficients, isLowCut);
                       case 1:
                           updateSingleCut<filterNum,0> (newChainCoefficients, isLowCut);
                   }
               }
           }
           oldParams = cutParams;
       }
                   
                  
           
           
//               leftChain.setBypassed<filterNum>(bypassed);
//               rightChain.setBypassed<filterNum>(bypassed);
//               bypassSubChain<filterNum>();
//               //set up the four filters
//               if(!bypassed)
//               {
//                   /*not sure I understand this fully. See SimpleEQ Part 7 at 10 min point for simple version
//                   which is then refactored at 5min point in Part 8*/
//                   switch(slope)
//                   {
//                       case Slope::Slope_48:
//                       case Slope::Slope_42:
//                           updateSingleCut<filterNum,3> (newChainCoefficients);
//                           
//                       case Slope::Slope_36:
//                       case Slope::Slope_30:
//                           updateSingleCut<filterNum,2> (newChainCoefficients);
//                           
//                       case Slope::Slope_24:
//                       case Slope::Slope_18:
//                           updateSingleCut<filterNum,1> (newChainCoefficients);
//                  
//                       case Slope::Slope_12:
//                       case Slope::Slope_6:
//                           updateSingleCut<filterNum,0> (newChainCoefficients);
//
//                      }
//               }
//           }
//           oldParams = cutParams;
//       }
       
       template <const int filterNum, const int subFilterNum, typename CoefficientType>
       void updateSingleCut(CoefficientType& chainCoefficients, bool isLowCut)
        {
            auto& leftSubChain = leftChain.template get<filterNum>();
            auto& rightSubChain = rightChain.template get<filterNum>();
            
            
            
            *(leftSubChain.template get<subFilterNum>().coefficients) = *(chainCoefficients[subFilterNum]);
            *(rightSubChain.template get<subFilterNum>().coefficients) = *(chainCoefficients[subFilterNum]);
            
            // add to correct release pool
            if(isLowCut)
            {
                lowCutCoeffPool.add(chainCoefficients[subFilterNum]);
            }
            else
            {
                highCutCoeffPool.add(chainCoefficients[subFilterNum]);
            }
            
            leftSubChain.template setBypassed<subFilterNum>(false);
            rightSubChain.template setBypassed<subFilterNum>(false);
                
            }
//       {
//           auto& leftSubChain = leftChain.template get<filterNum>();
//           auto& rightSubChain = rightChain.template get<filterNum>();
//           
//           
//           
//           *(leftSubChain.template get<subFilterNum>().coefficients) = *(chainCoefficients[subFilterNum]);
//           *(rightSubChain.template get<subFilterNum>().coefficients) = *(chainCoefficients[subFilterNum]);
//           
//           leftSubChain.template setBypassed<subFilterNum>(false);
//           rightSubChain.template setBypassed<subFilterNum>(false);
//           
//       }
       
       template <const int filterNum>
       void bypassSubChain()
       {
           auto& leftSubChain = leftChain.template get<filterNum>();
           auto& rightSubChain = rightChain.template get<filterNum>();
           leftSubChain.template setBypassed<0>(true);
           leftSubChain.template setBypassed<1>(true);
           leftSubChain.template setBypassed<2>(true);
           leftSubChain.template setBypassed<3>(true);
           rightSubChain.template setBypassed<0>(true);
           rightSubChain.template setBypassed<1>(true);
           rightSubChain.template setBypassed<2>(true);
           rightSubChain.template setBypassed<3>(true);
       }
    
    
    
    void updateFilters(double sampleRate, bool forceUpdate = false);
    
    MonoChain leftChain, rightChain;
    
    FilterParameters oldParametricParams;
    HighCutLowCutParameters oldCutParams;
    FilterInfo::FilterType oldFilterType;
    
    HighCutLowCutParameters oldHighCutParams;
    HighCutLowCutParameters oldLowCutParams;
    
//    using ParametricCoeffPtr = decltype(CoefficientsMaker::makeCoefficients (oldParametricParams));
//    using CutCoeffArray = decltype(CoefficientsMaker::makeCoefficients (oldLowCutParams));
    
    static const int fifoSize = 100;
    // in testing i could fill the pool with enough fiddling with pool size =100, not so with 1000
    static const int poolSize = 1000;
    static const int cleanupInterval = 2000; // ms

//    Fifo <FilterCoeffPtr,100>  parametricCoeffFifo;
    //Fifo <FilterCoeffPtr, fifoSize>  parametricCoeffFifo;
    Fifo <CutCoeffArray,fifoSize>  cutCoeffFifo;
    
    Fifo <CutCoeffArray,fifoSize>  lowCutCoeffFifo;
    Fifo <CutCoeffArray,fifoSize>  highCutCoeffFifo;
    
    FilterCoefficientGenerator<CutCoeffArray, HighCutLowCutParameters, CoefficientsMaker, 100> highCutCoeffGen {highCutCoeffFifo};
    FilterCoefficientGenerator<CutCoeffArray, HighCutLowCutParameters, CoefficientsMaker, 100> lowCutCoeffGen {lowCutCoeffFifo};
    FilterCoefficientGenerator<CutCoeffArray, HighCutLowCutParameters, CoefficientsMaker, 100> cutCoeffGen {cutCoeffFifo};
//    FilterCoefficientGenerator<FilterCoeffPtr, FilterParameters, CoefficientsMaker, 100> parametricCoeffGen {parametricCoeffFifo};
    //FilterCoefficientGenerator<FilterCoeffPtr, FilterParameters, CoefficientsMaker, fifoSize> parametricCoeffGen {parametricCoeffFifo};
    
   
    // Release pools
    //
    using Coefficients = juce::dsp::IIR::Coefficients<float>;
    ReleasePool<Coefficients, poolSize> lowCutCoeffPool {poolSize, cleanupInterval};
//    ReleasePool<Coefficients, poolSize> parametricCoeffPool {poolSize, cleanupInterval};
    //ReleasePool<Coefficients, poolSize> parametricCoeffPool {poolSize, cleanupInterval};
    ReleasePool<Coefficients, poolSize> highCutCoeffPool {poolSize, cleanupInterval};
    
//    FilterLink<Filter, FilterCoeffPtr, FilterParameters, CoefficientsMaker> paraFilterLink;


    
    
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Project11v2AudioProcessor)
};
