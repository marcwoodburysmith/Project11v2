/*
  ==============================================================================

    Meter.cpp
    Created: 7 Jan 2025 5:23:53pm
    Author:  Marc Woodbury-Smith

  ==============================================================================
*/

#include "Meter.h"

/*
The job of paint() is to draw a rectangle that has the following properties:

the bottom is always equal to the bottom of the component
the width is always as wide as the component
the x value is 0
the y value is peakDb mapped to a position within the component.
You should fill this rectangle with a different color than the color you’re filling the component with.
 the 201994664(at)N04favoriteswith52444361652

for example, the component could be filled with all black, and the rectangle could be white:
g.fillAll(Colours::black);
g.setColour(Colours::white);
*/

Meter::Meter() 
{
    
}

Meter::Meter(juce::String aLabel) : chanLabel {aLabel}
{
    decayingValueHolder.setDecayRate(3.0f);
}

Meter::~Meter() {}

void Meter::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    auto labelBounds = bounds.removeFromTop(labelSize).toNearestInt();

//    g.fillAll(juce::Colours::black);
    g.setColour(juce::Colours::black);
    g.fillRect(bounds);


    paintBar(g, peakDb, bounds, 0, juce::Colours::yellow);
    paintBar(g, averager.getAvg(), bounds, bounds.getWidth()/4.0f, juce::Colours::green);

    if (decayingValueHolder.isOverThreshold() )
    {
        g.setColour(juce::Colours::red);
    } 
    else
    {
        g.setColour(juce::Colours::orange);
    }
    
    float decayBarY = juce::jmap(decayingValueHolder.getCurrentValue(), NEGATIVE_INFINITY, MAX_DECIBELS, bounds.getHeight(), 0.f);
    
    g.fillRect(bounds.getX(), decayBarY, bounds.getWidth(), DECAY_BAR_THICK);
    
    g.drawFittedText(chanLabel, labelBounds, juce::Justification::centred, 1);

}

void Meter::paintBar (juce::Graphics& g, float value, juce::Rectangle<float> bounds, float dWidth, juce::Colour color)
{
    auto bar { bounds };
        
    g.setColour(color);
    float y = juce::jmap(value, NEGATIVE_INFINITY, MAX_DECIBELS, bar.getHeight(), 0.f);
    bar.setTop(y);
    bar.reduce(dWidth, 0);
    g.fillRect(bar);

}


void Meter::update(float dBLevel, float dbLevelRMS)
{
    peakDb = dBLevel;
    averager.add(peakDb);
    decayingValueHolder.updateHeldValue(dBLevel);
    repaint();
}

juce::Rectangle<int> Meter::getMeterBounds()
{
    return getBounds().withTrimmedTop(labelSize);
}


