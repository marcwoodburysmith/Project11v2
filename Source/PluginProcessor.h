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
    
    void addFilterParamToLayout (juce::AudioProcessorValueTreeState::ParameterLayout& layout, int filterNum, bool isCut);
    
    

private:
    
    using Filter = juce::dsp::IIR::Filter<float>;
    
    using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;
    
    using MonoChain = juce::dsp::ProcessorChain<CutFilter, Filter, CutFilter>;
    /*
     This templated function needs some work to add in filterNums which can be 0, 1 0r 2 for the 3 band filter
     This will need to be called 3 times in ProcessBlock for the three chains in the filter:
     updateFilters<0>( etc );
     updateFilters<1>( etc );
     updateFilters<2>( etc );
     
     */
    
//    template <const int filterNum>
//    void updateFilters(double sampleRate)
//    {
//        auto gain = apvts.getRawParameterValue(generateGainParamString(0))->load();
//        auto qual = apvts.getRawParameterValue(generateQParamString(0))->load();
//        auto freq = apvts.getRawParameterValue(generateFreqParamString(0))->load();
//        bool bypass = apvts.getRawParameterValue(generateBypassParamString(0))->load() > 0.5f;
//        
//        
//        FilterInfo::FilterType type = static_cast<FilterInfo::FilterType>(apvts.getRawParameterValue(generateTypeParamString(0))->load() );
//        
//        if( type == FilterInfo::FilterType::LowPass || type == FilterInfo::FilterType::HighPass || type == FilterInfo::FilterType::FirstOrderHighPass || type == FilterInfo::FilterType::FirstOrderLowPass )
//        {
//            HighCutLowCutParameters cutParams;
//            
//            cutParams.isLowcut = ( type == FilterInfo::FilterType::LowPass || type == FilterInfo::FilterType::FirstOrderLowPass );
//            cutParams.frequency = freq;
//            cutParams.bypassed = bypass;
//            cutParams.order = 1;
//            
//            if (type == FilterInfo::FilterType::HighPass || type == FilterInfo::FilterType::LowPass)
//                        cutParams.order = 2;
//            
//            cutParams.sampleRate = getSampleRate();
//            cutParams.quality = qual;
//            
//            if( type != oldFilterType || cutParams != oldHighLow )
//            {
//                auto chainCoefficients = CoefficientMaker::makeCoefficients(cutParams);
//                leftChain.setBypassed<filterNum>(bypass);
//                rightChain.setBypassed<filterNum>(bypass);
//                *(leftChain.get<filterNum>().coefficients) = *(chainCoefficients[0]);
//                *(rightChain.get<filterNum>().coefficients) = *(chainCoefficients[0]);
//            }
//            
//            
//            oldHighLow = cutParams;
//            
//        }
//        else
//        {
//            FilterParameters parametricParams;
//            
//            parametricParams.gain = gain;
//            parametricParams.frequency = freq;
//            parametricParams.sampleRate = getSampleRate();
//            parametricParams.quality = qual;
//            parametricParams.bypassed = bypass;
//            parametricParams.filterType = type;
//            
//            if ( type != oldFilterType || parametricParams != oldParams )
//            {
//                auto chainCoefficients = CoefficientMaker::makeCoefficients(parametricParams);
//                leftChain.setBypassed<filterNum>(bypass);
//                rightChain.setBypassed<filterNum>(bypass);
//                *(leftChain.get<filterNum>().coefficients) = *chainCoefficients;
//                *(rightChain.get<filterNum>().coefficients) = *chainCoefficients;
//            }
//            
//            
//            
//            oldParams = parametricParams;
//        }
//        
//        
//    }
    
    
    template <const int filterNum>
    void updateParametricFilter(double sampleRate, bool forceUpdate)
    {
        using namespace FilterInfo;


        
        float frequency = apvts.getRawParameterValue(generateFreqParamString(filterNum))->load();
        float quality  = apvts.getRawParameterValue(generateQParamString(filterNum))->load();
        bool bypassed = apvts.getRawParameterValue(generateBypassParamString(filterNum))->load() > 0.5f;
        
        FilterType filterType = static_cast<FilterType> (apvts.getRawParameterValue(generateTypeParamString(filterNum))->load());
        
        if (filterType == FilterType::LowPass || filterType == FilterType::HighPass || filterType == FilterType::FirstOrderHighPass || filterType == FilterType::FirstOrderLowPass)
        {
            HighCutLowCutParameters cutParams;
            
            cutParams.isLowcut = (filterType == FilterType::HighPass) || (filterType == FilterType::FirstOrderHighPass);
            cutParams.frequency = frequency;
            cutParams.bypassed = bypassed;
            cutParams.order = 1;
            
            if (filterType == FilterType::HighPass || filterType == FilterType::LowPass)
                cutParams.order = 2;
                
            
            cutParams.sampleRate = sampleRate;
            cutParams.quality  = quality;
            
            // set up filter chains.
           
            if (forceUpdate || filterType != oldFilterType || cutParams != oldCutParams)
            {
                auto chainCoefficients = CoefficientMaker::makeCoefficients(cutParams);
                
                cutCoeffFifo.push(chainCoefficients);
                decltype(chainCoefficients) newChainCoefficients;
                cutCoeffFifo.pull(newChainCoefficients);
                
                leftChain.setBypassed<filterNum>(bypassed);
                rightChain.setBypassed<filterNum>(bypassed);
                
                // Later this will be multiple filters for each of the bands i think.
                *(leftChain.get<filterNum>().coefficients) = *(newChainCoefficients[0]);
                *(rightChain.get<filterNum>().coefficients) = *(newChainCoefficients[0]);
            }
        
            oldCutParams = cutParams;
        }
        else
        {
            FilterParameters parametricParams;
            
            parametricParams.frequency = frequency;
            parametricParams.filterType = filterType;
            parametricParams.sampleRate = sampleRate;
            parametricParams.quality = quality;
            parametricParams.bypassed = bypassed;
            parametricParams.gain = juce::Decibels::decibelsToGain(apvts.getRawParameterValue(generateGainParamString(filterNum))-> load());
            
            // set up filter chains.

            if (forceUpdate || filterType != oldFilterType || parametricParams != oldParametricParams)
            {
                auto chainCoefficients = CoefficientMaker::makeCoefficients(parametricParams);
                
                // push and pull Fifo for testing.
                bool temp0 = parametricCoeffFifo.push(chainCoefficients);
                jassert(temp0);
                
                // pull a copy
                ParametricCoeffPtr newChainCoefficients;
                bool temp = parametricCoeffFifo.pull(newChainCoefficients);
                jassert(temp);
                

                
                leftChain.setBypassed<filterNum>(bypassed);
                rightChain.setBypassed<filterNum>(bypassed);
                *(leftChain.get<filterNum>().coefficients) = *newChainCoefficients;
                *(rightChain.get<filterNum>().coefficients) = *newChainCoefficients;
                
                // TEMPORARY To avoid releasing resources in audio thread, i increment the
                // reference count. But that means these will never get released for now.
                newChainCoefficients.get()->incReferenceCount();
      
            }
            
            oldParametricParams = parametricParams;
            oldFilterType = filterType;
        }
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
            auto chainCoefficients = CoefficientMaker::makeCoefficients(cutParams);
            decltype(chainCoefficients) newChainCoefficients;
            
            if(isLowCut)
            {
                lowCutCoeffFifo.push(chainCoefficients);
                lowCutCoeffFifo.pull(newChainCoefficients);
            }
            else
            {
                highCutCoeffFifo.push(chainCoefficients);
                highCutCoeffFifo.pull(newChainCoefficients);
            }
        
            leftChain.setBypassed<filterNum>(bypassed);
            rightChain.setBypassed<filterNum>(bypassed);
            bypassSubChain<filterNum>();
            //set up the four filters
            if(!bypassed)
            {
                
                switch(slope)
                {
                    case Slope::Slope_48:
                    case Slope::Slope_42:
                        updateSingleCut<filterNum,3> (newChainCoefficients);
                        
                    case Slope::Slope_36:
                    case Slope::Slope_30:
                        updateSingleCut<filterNum,2> (newChainCoefficients);
                        
                    case Slope::Slope_24:
                    case Slope::Slope_18:
                        updateSingleCut<filterNum,1> (newChainCoefficients);
               
                    case Slope::Slope_12:
                    case Slope::Slope_6:
                        updateSingleCut<filterNum,0> (newChainCoefficients);

                   }
            }
        }
        // side effect update. Code smell?
        oldParams = cutParams;
    }
    
    template <const int filterNum, const int subFilterNum, typename CoefficientType>
    void updateSingleCut(CoefficientType& chainCoefficients)
    {
        auto& leftSubChain = leftChain.template get<filterNum>();
        auto& rightSubChain = rightChain.template get<filterNum>();
        
        
        
        *(leftSubChain.template get<subFilterNum>().coefficients) = *(chainCoefficients[subFilterNum]);
        *(rightSubChain.template get<subFilterNum>().coefficients) = *(chainCoefficients[subFilterNum]);
        
        leftSubChain.template setBypassed<subFilterNum>(false);
        rightSubChain.template setBypassed<subFilterNum>(false);
        
    }
    
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
    
    void updateFilters(double sampleRate, bool forceUpdate);

    
    
    MonoChain leftChain, rightChain;
    
//    FilterParameters oldParams;
//    HighCutLowCutParameters oldHighLow;
    HighCutLowCutParameters oldCutParams;
    FilterParameters oldParametricParams;
    FilterInfo::FilterType oldFilterType;
    
    HighCutLowCutParameters oldHighCutParams;
    HighCutLowCutParameters oldLowCutParams;
    
    using ParametricCoeffPtr = decltype(CoefficientMaker::makeCoefficients (oldParametricParams));
    using CutCoeffArray = decltype(CoefficientMaker::makeCoefficients (oldLowCutParams));

    Fifo <ParametricCoeffPtr,10>  parametricCoeffFifo;
    Fifo <CutCoeffArray,10>  cutCoeffFifo;
    
    Fifo <CutCoeffArray,10>  lowCutCoeffFifo;
    Fifo <CutCoeffArray,10>  highCutCoeffFifo;
    
    
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Project11v2AudioProcessor)
};
