/*
  ==============================================================================

    CoefficientsMaker.h
    Created: 10 Aug 2024 10:03:59am
    Author:  Marc Woodbury-Smith

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "FilterInfo.h"
#include "FilterParameters.h"
#include "HighCutLowCutParameters.h"
#include "HelperFunctions.h"


/*
 https://docs.juce.com/master/structdsp_1_1IIR_1_1Coefficients.html
 */

struct CoefficientsMaker
{
    
    static auto makeCoefficients(FilterInfo::FilterType filterType,
                     float freq,
                     float qual,
                     float gain,
                     double sampleRate)
    {
        using namespace juce::dsp::IIR;
        
        switch ( filterType )
        {
            case FilterInfo::FilterType::FirstOrderLowPass:
                return Coefficients<float>::makeFirstOrderLowPass(sampleRate, freq);
            case FilterInfo::FilterType::FirstOrderHighPass:
                return Coefficients<float>::makeFirstOrderHighPass(sampleRate, freq);
            case FilterInfo::FilterType::FirstOrderAllPass:
                return Coefficients<float>::makeFirstOrderAllPass(sampleRate, freq);
            case FilterInfo::FilterType::LowPass:
                return Coefficients<float>::makeLowPass(sampleRate, freq, qual);
            case FilterInfo::FilterType::HighPass:
                return Coefficients<float>::makeHighPass(sampleRate, freq, qual);
            case FilterInfo::FilterType::BandPass:
                return Coefficients<float>::makeBandPass(sampleRate, freq, qual);
            case FilterInfo::FilterType::Notch:
                return Coefficients<float>::makeNotch(sampleRate, freq, qual);
            case FilterInfo::FilterType::AllPass:
                return Coefficients<float>::makeAllPass(sampleRate, freq, qual);
            case FilterInfo::FilterType::LowShelf:
                return Coefficients<float>::makeLowShelf(sampleRate, freq, qual, gain);
            case FilterInfo::FilterType::HighShelf:
                return Coefficients<float>::makeHighShelf(sampleRate, freq, qual, gain);
            case FilterInfo::FilterType::Peak:
                return Coefficients<float>::makePeakFilter(sampleRate, freq, qual, gain);
        }
        
    }
    
    static auto makeCoefficients(FilterParameters filterParams)
    {
//        return makeCoefficients(filterParams.filterType, filterParams.frequency, filterParams.quality, filterParams.gain, filterParams.sampleRate);
        return makeCoefficients(filterParams.filterType, filterParams.frequency, filterParams.quality, filterParams.gain.getGain(), filterParams.sampleRate);
        
    }
    
    static auto makeCoefficients(HighCutLowCutParameters filterParams)
    {
        if (filterParams.isLowcut)
        {
            return HelperFunctions::designIIRHighpassHighOrderButterworthMethod(filterParams.frequency,
                                                                                filterParams.sampleRate,
                                                                                filterParams.order,
                                                                                filterParams.quality);
        }
        return HelperFunctions::designIIRLowpassHighOrderButterworthMethod(filterParams.frequency,
                                                                            filterParams.sampleRate,
                                                                            filterParams.order,
                                                                           filterParams.quality);

    }
    
    
    
};


 //convenience types

//using ParametricCoeffPtr = decltype(CoefficientsMaker::makeCoefficients (FilterParameters()));
using FilterCoeffPtr = decltype(CoefficientsMaker::makeCoefficients (FilterParameters()));
using CutCoeffArray = decltype(CoefficientsMaker::makeCoefficients (HighCutLowCutParameters()));


