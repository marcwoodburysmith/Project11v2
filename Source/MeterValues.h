/*
  ==============================================================================

    MeterValues.h
    Created: 15 Jan 2025 2:23:21pm
    Author:  Marc Woodbury-Smith

  ==============================================================================
*/

#pragma once

#include "Decibel.h"

struct MeterValues
{
    Decibel<float> leftPeakDb, rightPeakDb, leftRmsDb, rightRmsDb;
};

