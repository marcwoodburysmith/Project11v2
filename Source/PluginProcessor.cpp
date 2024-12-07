/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


#include "ParameterHelpers.h"
#include "FilterInfo.h"
#include "FilterParameters.h"
#include "HighCutLowCutParameters.h"

#include <string>

//==============================================================================



//==============================================================================
Project11v2AudioProcessor::Project11v2AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

Project11v2AudioProcessor::~Project11v2AudioProcessor()
{
}
//==============================================================================


void Project11v2AudioProcessor::performPreLoopUpdate(ChannelMode mode, double sampleRate)
{
//    preUpdateCutFilter<0>(sampleRate, true);
//    preUpdateParametricFilter<1>(sampleRate);
//    preUpdateParametricFilter<2>(sampleRate);
//    preUpdateParametricFilter<3>(sampleRate);
//    preUpdateParametricFilter<4>(sampleRate);
//    preUpdateParametricFilter<5>(sampleRate);
//    preUpdateParametricFilter<6>(sampleRate);
//    preUpdateCutFilter<7>(sampleRate, false);
   preUpdateCutFilter<0>(mode, sampleRate, true);
   preUpdateParametricFilter<1>(mode, sampleRate);
   preUpdateParametricFilter<2>(mode, sampleRate);
   preUpdateParametricFilter<3>(mode, sampleRate);
   preUpdateParametricFilter<4>(mode, sampleRate);
   preUpdateParametricFilter<5>(mode, sampleRate);
   preUpdateParametricFilter<6>(mode, sampleRate);
   preUpdateCutFilter<7>(mode, sampleRate, false);

}

void Project11v2AudioProcessor::performInnerLoopUpdate(int numSamplesToSkip)
{
    loopUpdateCutFilter<0>(numSamplesToSkip);
    loopUpdateParametricFilter<1>(numSamplesToSkip);
    loopUpdateParametricFilter<2>(numSamplesToSkip);
    loopUpdateParametricFilter<3>(numSamplesToSkip);
    loopUpdateParametricFilter<4>(numSamplesToSkip);
    loopUpdateParametricFilter<5>(numSamplesToSkip);
    loopUpdateParametricFilter<6>(numSamplesToSkip);
    loopUpdateCutFilter<7>(numSamplesToSkip);
}

//==============================================================================

void Project11v2AudioProcessor::initializeFilters(Channel channel, double sampleRate)
{
    
    // check if on realtime thread
        auto messMan = juce::MessageManager::getInstanceWithoutCreating();
        bool onRealTimeThread=  ! ((messMan != nullptr) && messMan->isThisTheMessageThread());
        
    // initialize filters
      
//       initializeChain<1>(getParametericFilterParams<1>(sampleRate),onRealTimeThread,sampleRate);
//       initializeChain<2>(getParametericFilterParams<2>(sampleRate),onRealTimeThread,sampleRate);
//       initializeChain<3>(getParametericFilterParams<3>(sampleRate),onRealTimeThread,sampleRate);
//       initializeChain<4>(getParametericFilterParams<4>(sampleRate),onRealTimeThread,sampleRate);
//       initializeChain<5>(getParametericFilterParams<5>(sampleRate),onRealTimeThread,sampleRate);
//       initializeChain<6>(getParametericFilterParams<6>(sampleRate),onRealTimeThread,sampleRate);
    
       initializeChain<1>(getParametericFilterParams<1>(channel, sampleRate), onRealTimeThread, sampleRate);
       initializeChain<2>(getParametericFilterParams<2>(channel, sampleRate), onRealTimeThread, sampleRate);
       initializeChain<3>(getParametericFilterParams<3>(channel, sampleRate), onRealTimeThread, sampleRate);
       initializeChain<4>(getParametericFilterParams<4>(channel, sampleRate), onRealTimeThread, sampleRate);
       initializeChain<5>(getParametericFilterParams<5>(channel, sampleRate), onRealTimeThread, sampleRate);
       initializeChain<6>(getParametericFilterParams<6>(channel, sampleRate), onRealTimeThread, sampleRate);
       
       
       //low cut filter, and then high cut
//       HighCutLowCutParameters lowCutParams = getCutFilterParams<0>(sampleRate, true);
//       initializeChain<0>(lowCutParams,onRealTimeThread,sampleRate);
//       HighCutLowCutParameters highCutParams = getCutFilterParams<7>(sampleRate, false);
//       initializeChain<7>(highCutParams,onRealTimeThread,sampleRate);
    HighCutLowCutParameters lowCutParams = getCutFilterParams<0>(channel, sampleRate, true);
    initializeChain<0>(lowCutParams,onRealTimeThread,sampleRate);
    HighCutLowCutParameters highCutParams = getCutFilterParams<7>(channel, sampleRate, false);
    initializeChain<7>(highCutParams,onRealTimeThread,sampleRate);


}


//==============================================================================

void Project11v2AudioProcessor::addFilterParamToLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout,
                                                        Channel channel,
                            int filterNum,
                            bool isCut)
{
    const int versionID = 1;
    
    auto label = generateBypassParamString(channel, filterNum);
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{label, 1}, label ,false) );
    
//    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{generateBypassParamString(filterNum), versionID},generateBypassParamString(filterNum),false) );
    
        
//    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{generateFreqParamString(filterNum), versionID}, generateFreqParamString(filterNum),
//                                       juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.25f), 20.0f));
    label = generateFreqParamString(channel, filterNum);
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{label, 1}, label,
                                          juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.25f), 20.0f));
    
    

    
    if (!isCut )
    {
//        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{generateQParamString(filterNum), versionID}, generateQParamString(filterNum),
//                                           juce::NormalisableRange<float>(0.1f, 10.f, 0.05f, 1.0f), 1.0f));
        
//        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{generateQParamString(filterNum), 1}, generateQParamString(filterNum),
//                                              juce::NormalisableRange<float>(0.1f, 10.f, 0.05f, 1.0f), 1.0f));
//        
//        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{generateGainParamString(filterNum), versionID}, generateGainParamString(filterNum),
//                                           juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.0f), 0.0f));
        
       label = generateQParamString(channel, filterNum);
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{label, 1}, label,
                                                  juce::NormalisableRange<float>(0.1f, 10.f, 0.05f, 1.0f), 1.0f));
               
       label = generateGainParamString(channel, filterNum);
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{label, 1}, label,
                                          juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.0f), 0.0f));


        
        juce::StringArray types;
        
        for (const auto& [type, stringRep] : FilterInfo::filterToString)
        {
          //Verify map is sorted like I believe it is by the standard
          // DBG( "Key as int:" + std::to_string(static_cast<int>(type)));
            types.add(stringRep);
        }
        
        label = generateTypeParamString(channel, filterNum);
        layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{label, 1}, label, types, 10));

        
        
//        layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{generateTypeParamString(filterNum), versionID}, generateTypeParamString(filterNum), types, 0));
        
    }
    else
    {
//        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{generateQParamString(filterNum), 1}, generateQParamString(filterNum),
//                                                  juce::NormalisableRange<float>(0.1f, 10.f, 0.01f, 1.0f), 0.71f));
        
        label = generateQParamString(channel, filterNum);
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{label, 1}, label,
                                                  juce::NormalisableRange<float>(0.1f, 10.f, 0.01f, 1.0f), 0.71f));


        juce::StringArray slopes;
                
                for (const auto& [order, stringRep] : FilterInfo::slopeToString)
                {
                    slopes.add(stringRep);
                }

//        layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{generateSlopeParamString(filterNum), versionID},
//                                                                generateSlopeParamString(filterNum), slopes, 0));
        label = generateSlopeParamString(channel, filterNum);
        layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{label, 1}, label, slopes, 0));
            
    }

}

void Project11v2AudioProcessor::createFilterLayouts(ParamLayout& layout, Channel channel)
{
    addFilterParamToLayout(layout, channel, 0, true);
    addFilterParamToLayout(layout, channel, 1, false);
    addFilterParamToLayout(layout, channel, 2, false);
    addFilterParamToLayout(layout, channel, 3, false);
    addFilterParamToLayout(layout, channel, 4, false);
    addFilterParamToLayout(layout, channel, 5, false);
    addFilterParamToLayout(layout, channel, 6, false);
    addFilterParamToLayout(layout, channel, 7, true);
}



juce::AudioProcessorValueTreeState::ParameterLayout Project11v2AudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    juce::StringArray modes;
       
       for (const auto& [mode, stringRep] : mapModeToString)
       {
           modes.add(stringRep);
       }
       
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{"Processing Mode", 1}, "Processing Mode", modes, 0));

    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"input_trim", 1}, "input_trim",
                                                               juce::NormalisableRange<float>(-18.f, 18.f, 0.5f, 1.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"output_trim", 1}, "output_trim",
                                                               juce::NormalisableRange<float>(-18.f, 18.f, 0.5f, 1.0f), 0.0f));
    
    createFilterLayouts(layout, Channel::Left);
    createFilterLayouts(layout, Channel::Right);



//    addFilterParamToLayout(layout, 0, true);
//    addFilterParamToLayout(layout, 1, false);
//    addFilterParamToLayout(layout, 2, true);
//    addFilterParamToLayout(layout, 0, true);
//    addFilterParamToLayout(layout, 1, false);
//    addFilterParamToLayout(layout, 2, false);
//    addFilterParamToLayout(layout, 3, false);
//    addFilterParamToLayout(layout, 4, false);
//    addFilterParamToLayout(layout, 5, false);
//    addFilterParamToLayout(layout, 6, false);
//    addFilterParamToLayout(layout, 7, true);
    
    return layout;
}

void Project11v2AudioProcessor::updateTrims()
{
    
//    float inputTrim = apvts.getRawParameterValue("input_trim")->load();
//    float outputTrim = apvts.getRawParameterValue("output_trim")->load();
//    leftChain.get<InputTrim>().setGainDecibels(inputTrim);
//    rightChain.get<InputTrim>().setGainDecibels(inputTrim);
//    leftChain.get<OutputTrim>().setGainDecibels(outputTrim);
//    rightChain.get<OutputTrim>().setGainDecibels(outputTrim);
    float inputGain= apvts.getRawParameterValue("input_trim")->load();
    float outputGain = apvts.getRawParameterValue("output_trim")->load();
    inputTrim.setGainDecibels(inputGain);
    outputTrim.setGainDecibels(outputGain);
     

}


//void Project11v2AudioProcessor::performHadamard(juce::dsp::AudioBlock<float>& A, juce::dsp::AudioBlock<float>& B)
//{
//    static const float sqrt2 = juce::MathConstants<float>::sqrt2;
//    static const float invSqrt2 = 1.0f/sqrt2;
//    
//    // Hadamard transformation, not that this is involutory.
//    // Anew = (A+B)/sqrt(2), Bnew = (A-B)/sqrt(2)
//    A.add(B).multiplyBy(invSqrt2);
//    // Bnew = -1*(sqrt(2)B-Anew) =  (A+B)/sqrt(2) -  sqrt(2)B= (A-B)/sqrt(2)
//    B.multiplyBy(sqrt2).subtract(A).negate();
//}

void Project11v2AudioProcessor::performMidSideTransform(juce::AudioBuffer<float>& buffer)
{
    // AKA Hadamard transformation
    // Anew = (A+B)/sqrt(2), Bnew = (A-B)/sqrt(2)
    // note that this is involutory , i.e. a second call will undo the transformation
    
    static const float minus3db = 1.0f/juce::MathConstants<float>::sqrt2;

    auto leftReadPtr = buffer.getReadPointer(0);
    auto rightReadPtr = buffer.getReadPointer(1);

    auto leftWritePtr = buffer.getWritePointer(0);
    auto rightWritePtr = buffer.getWritePointer(1);
    
    int numSamples = buffer.getNumSamples();

    for( int i=0; i < numSamples; i++ )
    {
         auto M = (leftReadPtr[i] + rightReadPtr[i]) * minus3db;
         auto S = (leftReadPtr[i] - rightReadPtr[i]) * minus3db;

         leftWritePtr[i] = M;
         rightWritePtr[i] = S;
    }
}



//==============================================================================
const juce::String Project11v2AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Project11v2AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Project11v2AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Project11v2AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Project11v2AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Project11v2AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Project11v2AudioProcessor::getCurrentProgram()
{
    return 0;
}

void Project11v2AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Project11v2AudioProcessor::getProgramName (int index)
{
    return {};
}

void Project11v2AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Project11v2AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;
    spec.sampleRate = sampleRate;
    
    leftChain.prepare(spec);
    rightChain.prepare(spec);
    
    // reuse spec
   spec.numChannels = 2;
   inputTrim.prepare(spec);
   outputTrim.prepare(spec);
    
// initialize filters first.
//   FilterParameters paramatricParams = getParametericFilterParams<1>(sampleRate);
//   leftChain.get<1>().initialize(paramatricParams, 0.0, false, sampleRate);
//   
//   //TODO replace these with initialize stuff.
//   updateCutFilter<0>(sampleRate, true, oldHighCutParams, true);
//   updateCutFilter<2>(sampleRate, true, oldLowCutParams, false);
//    initializeFilters(sampleRate);
    initializeFilters(Channel::Left, sampleRate);
    initializeFilters(Channel::Right, sampleRate);
//    updateFilters(sampleRate,true);
}

void Project11v2AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Project11v2AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void Project11v2AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
//    updateFilters( getSampleRate() );
    updateTrims();
//    performPreLoopUpdate(getSampleRate());
//    performInnerLoopUpdate(getSampleRate(), buffer.getNumSamples());
    
    ChannelMode mode = static_cast<ChannelMode>(apvts.getRawParameterValue("Processing Mode")->load());
    
    performPreLoopUpdate(mode, getSampleRate());

   
    
    
    juce::dsp::AudioBlock<float> block(buffer);

    int numSamples = buffer.getNumSamples();
    int offset = 0;
    
    juce::dsp::ProcessContextReplacing<float> stereoContext(block);
    inputTrim.process(stereoContext);
    
//    auto leftBlock = block.getSingleChannelBlock(0);
//    auto rightBlock = block.getSingleChannelBlock(1);
    
//    static const float sqrt2 = juce::MathConstants<float>::sqrt2;
//    static const float invSqrt2 = 1.0f/sqrt2;

    
    if(mode == ChannelMode::MidSide)
    {
        
//        leftBlock.add(rightBlock).multiplyBy(invSqrt2);
//        // right = -1*(sqrt(2)R-Lnew) =  (L+R)/sqrt(2) -  sqrt(2)R= (L-R)/sqrt(2)
//        rightBlock.multiplyBy(sqrt2).subtract(leftBlock).negate();
//        performHadamard(leftBlock, rightBlock);
        performMidSideTransform(buffer);
    }
    
    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);

    
    while(offset < numSamples)
    {
        int blockSize = std::min(numSamples - offset, innerLoopSize);
        auto leftSubBlock =  leftBlock.getSubBlock(offset, blockSize);
        auto rightSubBlock = rightBlock.getSubBlock(offset, blockSize);
 
        performInnerLoopUpdate(blockSize);
        
        juce::dsp::ProcessContextReplacing<float> leftContext(leftSubBlock);
        juce::dsp::ProcessContextReplacing<float> rightContext(rightSubBlock);
        leftChain.process(leftContext);
        rightChain.process(rightContext);
                
        offset += innerLoopSize;
    }
    
    if(mode == ChannelMode::MidSide)
    {
        
//       leftBlock.add(rightBlock).multiplyBy(invSqrt2);
//       // right is (M-S)/sqrt(2).   Right =-1( sqrt(2)*S-Lnew) = (M+S)/sqrt(2) - sqrt(2)S = (M-S)/sqrt(2)
//       rightBlock.multiplyBy(sqrt2).subtract(leftBlock).negate();
//        performHadamard(leftBlock, rightBlock);
        performMidSideTransform(buffer);
    }
        

    outputTrim.process(stereoContext);

}

//==============================================================================
bool Project11v2AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Project11v2AudioProcessor::createEditor()
{
//    return new Project11v2AudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void Project11v2AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);

}

void Project11v2AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
            if (xmlState->hasTagName (apvts.state.getType()))
                    apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
//    initializeFilters(getSampleRate());
    initializeFilters(Channel::Left, getSampleRate());
    initializeFilters(Channel::Right, getSampleRate());

}



//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Project11v2AudioProcessor();
}
