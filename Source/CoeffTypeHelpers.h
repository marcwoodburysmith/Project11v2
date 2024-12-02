/*
  ==============================================================================

    CoeffTypeHelpers.h
    Created: 2 Dec 2024 6:11:32am
    Author:  Marc Woodbury-Smith

  ==============================================================================
*/

#pragma once


// helpers for type checking
template <typename U>
struct isReferenceCountedObjectPtr : std::false_type { };

template <typename W>
struct isReferenceCountedObjectPtr<juce::ReferenceCountedObjectPtr<W>> : std::true_type { };

template <typename U>
struct isReferenceCountedArray : std::false_type { };

template <typename W>
struct isReferenceCountedArray<juce::ReferenceCountedArray<W>> : std::true_type { };

template <typename U>
struct isVector : std::false_type { };

template <typename W>
struct isVector<std::vector<W>> : std::true_type { };

template <typename U>
struct isAudioBuffer : std::false_type { };

template <typename W>
struct isAudioBuffer<juce::AudioBuffer<W>> : std::true_type { };
