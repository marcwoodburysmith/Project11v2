/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "MeterValues.h"

//==============================================================================
Project11v2AudioProcessorEditor::Project11v2AudioProcessorEditor (Project11v2AudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
   // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    addAndMakeVisible(inputMeter);
    addAndMakeVisible(outputMeter);
//    addAndMakeVisible(inputScale);
    setSize (800, 600);
    
    startTimerHz(60);
}

Project11v2AudioProcessorEditor::~Project11v2AudioProcessorEditor()
{
}

//==============================================================================
void Project11v2AudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void Project11v2AudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    const uint scaleAndMeterWidth = 75;
    auto bounds = getLocalBounds();
    
    bounds.reduce(10, 10);
//    auto scaledMeterBounds = bounds.removeFromLeft(50);
//    inputScale.setBounds(scaledMeterBounds.removeFromLeft(25));
//    auto meterBounds = scaledMeterBounds.withTrimmedTop(10).withTrimmedBottom(10);
    
#ifdef TESTMETER
    meterBounds.setY(JUCE_LIVE_CONSTANT(meterBounds.getY()));
    meterBounds.setHeight(JUCE_LIVE_CONSTANT(meterBounds.getHeight()));
#endif
    
    inputMeter.setBounds(bounds.removeFromLeft(scaleAndMeterWidth));
    outputMeter.setBounds(bounds.removeFromRight(scaleAndMeterWidth));
//    inputScale.buildBackgroundImage(TICK_INTERVAL, meterBounds, NEGATIVE_INFINITY, MAX_DECIBELS);
    
}

void Project11v2AudioProcessorEditor::timerCallback()
{
    auto& inputFifo = audioProcessor.inMeterValuesFifo;
    auto& outputFifo = audioProcessor.outMeterValuesFifo;
    
    MeterValues inputValues;
    
    if (inputFifo.getNumAvailableForReading() > 0)
    {
        while ( inputFifo.pull(inputValues) )
        {
            
        }

//        inputValues.leftPeakDb.setGain(buffer.getMagnitude(0, 0, buffer.getNumSamples()));
//        inputValues.rightPeakDb.setGain(buffer.getMagnitude(1, 0, buffer.getNumSamples()));
//        
//        inputValues.leftRmsDb.setGain(buffer.getRMSLevel(0, 0, buffer.getNumSamples()));
//        inputValues.rightRmsDb.setGain(buffer.getRMSLevel(1, 0, buffer.getNumSamples()));

        
        inputMeter.update(inputValues);
        
        
        
    }
    

}
