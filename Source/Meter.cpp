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

Meter::~Meter() {}

void Meter::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    g.setColour(juce::Colours::white);
    auto bounds = getLocalBounds().toFloat();
    auto bar {bounds};

//    juce::Rectangle<float> rect {getLocalBounds().toFloat() };
//    auto bar {getLocalBounds().toFloat() };
    float y = juce::jmap(peakDb, NEGATIVE_INFINITY, MAX_DECIBELS, bar.getHeight(), 0.f);
    bar.setTop(y);
    g.fillRect (bar);
    
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
}

void Meter::update(float dBLevel)
{
    decayingValueHolder.updateHeldValue(dBLevel);
    peakDb = dBLevel;
    repaint();
}

