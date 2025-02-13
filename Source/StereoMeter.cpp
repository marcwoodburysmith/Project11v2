/*
  ==============================================================================

    StereoMeter.cpp
    Created: 15 Jan 2025 1:55:57pm
    Author:  Marc Woodbury-Smith

  ==============================================================================
*/

#include <JuceHeader.h>
#include "StereoMeter.h"

StereoMeter::StereoMeter(juce::String label) : meterLabel{label}
{
    addAndMakeVisible(leftMeter);
    addAndMakeVisible(rightMeter);
    addAndMakeVisible(dBScale);
}


void StereoMeter::paint (juce::Graphics& g)
{
 
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background
    // to do, add L R at top with overage indicator.
    auto bounds = getLocalBounds();
 
    auto nameBounds = bounds.removeFromBottom(labelSpace);
    
    g.setColour(juce::Colours::white);
    g.drawFittedText(meterLabel, nameBounds, juce::Justification::centred, 1);

}

void StereoMeter::resized()
{
    
    auto bounds = getLocalBounds();
   
    
    leftMeter.setBounds(bounds.removeFromLeft(meterWidth).withTrimmedBottom(labelSpace));
    rightMeter.setBounds(bounds.removeFromRight(meterWidth).withTrimmedBottom(labelSpace));
 
    dBScale.setBounds(bounds);
    dBScale.buildBackgroundImage(TICK_INTERVAL, rightMeter.getMeterBounds(), NEGATIVE_INFINITY, MAX_DECIBELS);
}


void StereoMeter::update(MeterValues values)
{
    leftMeter.update(values.leftPeakDb.getDb(), values.leftRmsDb.getDb());
    rightMeter.update(values.rightPeakDb.getDb(), values.rightRmsDb.getDb());
}
