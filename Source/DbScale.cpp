/*
  ==============================================================================

    DbScale.cpp
    Created: 12 Jan 2025 6:50:41am
    Author:  Marc Woodbury-Smith

  ==============================================================================
*/

#include <JuceHeader.h>
#include "DbScale.h"


void DbScale::paint (juce::Graphics& g)
{
//    All this does is draw bkgd with the same size as getLocalBounds().toFloat()
    
    g.drawImage(bkgd, getLocalBounds().toFloat() );
}


void DbScale::buildBackgroundImage(int dbDivision, juce::Rectangle<int> meterBounds, int minDb, int maxDb)
{
    jassert(minDb < maxDb);
    
    auto bounds = getLocalBounds();
    
    if (bounds.isEmpty() )
        return;
    
    auto scaleFactor = juce::Desktop::getInstance().getDisplays().getDisplayForPoint({0,0}) -> scale;
    
    bkgd = juce::Image(juce::Image::RGB, bounds.getWidth() * scaleFactor, bounds.getHeight() * scaleFactor, true);
    
    juce::Graphics g(bkgd);
    g.addTransform(juce::AffineTransform().scale(scaleFactor));
    g.setColour(juce::Colours::lightgrey);
//    g.drawRect(bounds);
    auto ticks =  getTicks(dbDivision, meterBounds, minDb, maxDb);
        
    for(auto tick:ticks)
    {
        juce::Rectangle<int> tickarea {bounds.getX(), tick.y-textHeight/2,  bounds.getWidth(), textHeight};
        g.drawFittedText(std::to_string(static_cast<int>(tick.db)),
                         tickarea, juce::Justification::centred, 1);
    }
}

std::vector<Tick> DbScale::getTicks(int dbDivision, juce::Rectangle<int> meterBounds, int minDb, int maxDb)
{
    if (minDb > maxDb )
        std::swap(minDb, maxDb);
    
    int numberOfTicks = (maxDb - minDb)/ dbDivision;
    std::vector<Tick> ticks;
    ticks.reserve(numberOfTicks);
    
    for ( int db = minDb; db <= maxDb; db += dbDivision )
    {
        Tick tick;
        tick.db = db;
        tick.y = juce::jmap(db, minDb, maxDb, meterBounds.getBottom(), meterBounds.getY() );
        ticks.push_back(tick);
    }
    
    return ticks;
    
}


