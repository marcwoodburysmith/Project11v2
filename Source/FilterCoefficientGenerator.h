/*
  ==============================================================================

    FilterCoefficientGenerator.h
    Created: 30 Aug 2024 4:59:02pm
    Author:  Marc Woodbury-Smith

  ==============================================================================
*/

/*Before we can safely use heap-allocated filter coefficients on the audio thread, we need to build this FilterCoefficientGenerator class to produce them on a background thread, and the ReleasePool<> to destroy them on a non-audio thread.*/

#pragma once

#include <JuceHeader.h>
#include "Fifo.h"
#include "CoefficientsMaker.h"



template <typename CoefficientType, typename ParamType, typename MakeFunction, size_t Size>
struct FilterCoefficientGenerator : public juce::Thread
{
    FilterCoefficientGenerator(Fifo <CoefficientType, Size>& processFifo) : juce::Thread {"Filter Coefficient Generator Thread"}, coeffFifo{processFifo}
    {
        startThread();
    }
    ~FilterCoefficientGenerator() override
    {
        bool stopSucceeded = stopThread(2000);
        jassert(stopSucceeded);
    }
    
//  changeParameters will push params into a Fifo<> that the run() function will consume.
    void changeParameters(ParamType params)
    {
        paramFifo.push(params);
        paramChanged.set(true);
        
    }
    
//  The run() function computes the updated coefficients and sends the result into a Fifo that the audio thread can pull from.
    void run() override
    {
        while ( !threadShouldExit() )
        {
            wait (waitTime);
            if (paramChanged.compareAndSetBool(false, true))
            {
                while ( paramFifo.getNumAvailableForReading() > 0 )
                {
                    ParamType params;
                    paramFifo.pull(params);
                    auto coeffs = MakeFunction::makeCoefficients(params);
                    coeffFifo.push(coeffs);
                }
            }
        }
    }
    
private:
    int waitTime = 10;  //ms
        
    Fifo <CoefficientType, Size>& coeffFifo;
    Fifo <ParamType, 100> paramFifo;
    
    juce::Atomic<bool> paramChanged{false};
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR
    (FilterCoefficientGenerator)
    
    
};
