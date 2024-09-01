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
    
    
    bool exchange(T&& t)
        {

        auto readHandle = fifo.read(1);
        if (readHandle.blockSize1 > 0)
        {
            if constexpr (isReferenceCountedObjectPtr<T>::value)
            {
                std::swap(t, buffer[readHandle.startIndex1]);
                jassert( buffer[readHandle.startIndex1].get() == nullptr); // only call this when t points to null
            }
            else if constexpr(isReferenceCountedArray<T>::value)
            {
                std::swap(t, buffer[readHandle.startIndex1]);
                jassert(buffer[readHandle.startIndex1].isEmpty());  //ony call when t is empty
            }
            else if constexpr(isVector<T>::value)
            {
                if(t.size() >= buffer[readHandle.startIndex1].size())
                {
                    std::swap(t, buffer[readHandle.startIndex1]);
                }
                else
                {
                    t = buffer[readHandle.startIndex1]; //can't swap.  must copy
                }
            }
            else if constexpr(isAudioBuffer<T>::value)
            {
                if(t.getNumSamples() >= buffer[readHandle.startIndex1].getNumSamples())
                {
                    std::swap(t, buffer[readHandle.startIndex1]);
                }
                else
                {
                    t = buffer[readHandle.startIndex1]; //can't swap.  must copy
                }
                
            }
            else
            {
                // blind swap
                std::swap(t, buffer[readHandle.startIndex1]);
                jassertfalse;  // temporary, check on this case if it occurs
            }

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
    
   
};
