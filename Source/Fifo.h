/*
  ==============================================================================

    Fifo.h
    Created: 11 Aug 2024 8:05:26am
    Author:  Marc Woodbury-Smith

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

template<typename T, size_t Size>
struct Fifo
{
    size_t getSize() const noexcept
    {
        return Size;
    }
    
    //used when T is AudioBuffer<float>
    void prepare(int numChannels, int numSamples)
    {
        static_assert( std::is_same_v<T, juce::AudioBuffer<float>>,
                      "prepare(numChannels, numSamples) should only be used when the Fifo is holding juce::AudioBuffer<float>");
        for( auto& audioBuffer : buffer)
        {
            buffer.setSize(numChannels,
                           numSamples,
                           false,   //clear everything?
                           true,    //including the extra space?
                           true);   //avoid reallocating if you can?
            buffer.clear();
        }
    }
    
    //used when T is std::vector<float>
    void prepare(size_t numElements)
    {
        static_assert( std::is_same_v<T, std::vector<float>>,
                      "prepare(numElements) should only be used when the Fifo is holding std::vector<float>");
        for( auto& audioBuffer : buffer )
        {
            buffer.clear();
            buffer.resize(numElements, 0);
        }
    }
    
    
    bool push(const T& t)
    {
        auto writeHandle = fifo.write(1);
                
        if (writeHandle.blockSize1 < 1)
            return false;
        
        if constexpr (isReferenceCountedObjectPtr<T>::value)
        {
            // save a copy of the ptr currently in buffer if any, increasing reference count.
            auto tempT {buffer[writeHandle.startIndex1]};
            buffer[writeHandle.startIndex1] = t;
            
            // verify we are not about to delete the object that was at this index, if any!
            if(tempT)
            {
                jassert (tempT.get()->getReferenceCount() > 1);
            }
        }
        else
        {
            buffer[writeHandle.startIndex1] = t;
        }
        
        buffer[writeHandle.startIndex1] = t;
        return true;
        
    }
    
    bool pull(T& t)
    {
        auto readHandle = fifo.read(1);
        if (readHandle.blockSize1 > 0)
        {
            t = buffer[readHandle.startIndex1];
            return true;
        }
        
        return false;
        
    }
    
    int getNumAvailableForReading() const
    {
        return fifo.getNumReady();
    }
    int getAvailableSpace() const
    {
        return fifo.getFreeSpace();
    }
    
private:
    juce::AbstractFifo fifo { Size };
    std::array<T, Size> buffer;
    
    template <typename U>
    struct isReferenceCountedObjectPtr : std::false_type { };

    template <typename W>
    struct isReferenceCountedObjectPtr<juce::ReferenceCountedObjectPtr<W>> : std::true_type { };
};
