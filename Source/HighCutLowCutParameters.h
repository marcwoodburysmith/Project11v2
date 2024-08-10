/*
  ==============================================================================

    HighCutLowCutParameters.h
    Created: 10 Aug 2024 10:03:40am
    Author:  Marc Woodbury-Smith

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

struct HighCutLowCutParameters : public FilterParametersBase
{
    int order {1};
    bool isLowcut {true};
    
};

inline bool operator==(const HighCutLowCutParameters& lhs, const HighCutLowCutParameters& rhs)
{
    return (lhs.order == rhs.order && lhs.isLowcut == rhs.isLowcut &&
            static_cast<const FilterParametersBase&>(lhs) == static_cast<const FilterParametersBase&>(rhs) );
}


inline bool operator!=(const HighCutLowCutParameters& lhs, const HighCutLowCutParameters& rhs)
{
    return !(lhs == rhs);
}

