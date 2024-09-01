/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ReleasePool.h"



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

//void Project11v2AudioProcessor::updateFilters(double sampleRate)
//{
//    
////    int filterNum = 0;
//    
//    auto gain = apvts.getRawParameterValue(generateGainParamString(0))->load();
//    auto qual = apvts.getRawParameterValue(generateQParamString(0))->load();
//    auto freq = apvts.getRawParameterValue(generateFreqParamString(0))->load();
//    bool bypass = apvts.getRawParameterValue(generateBypassParamString(0))->load() > 0.5f;
//    
//    
//    FilterInfo::FilterType type = static_cast<FilterInfo::FilterType>(apvts.getRawParameterValue(generateTypeParamString(0))->load() );
//    
//    if( type == FilterInfo::FilterType::LowPass || type == FilterInfo::FilterType::HighPass || type == FilterInfo::FilterType::FirstOrderHighPass || type == FilterInfo::FilterType::FirstOrderLowPass )
//    {
//        HighCutLowCutParameters cutParams;
//        
//        cutParams.isLowcut = ( type == FilterInfo::FilterType::LowPass || type == FilterInfo::FilterType::FirstOrderLowPass );
//        cutParams.frequency = freq;
//        cutParams.bypassed = bypass;
//        cutParams.order = 1;
//        
//        if (type == FilterInfo::FilterType::HighPass || type == FilterInfo::FilterType::LowPass)
//                    cutParams.order = 2;
//        
//        cutParams.sampleRate = getSampleRate();
//        cutParams.quality = qual;
//        
//        if( type != oldFilterType || cutParams != oldCutParams )
//        {
//            auto chainCoefficients = CoefficientMaker::makeCoefficients(cutParams);
//            leftChain.setBypassed<0>(bypass);
//            rightChain.setBypassed<0>(bypass);
//            *(leftChain.get<0>().coefficients) = *(chainCoefficients[0]);
//            *(rightChain.get<0>().coefficients) = *(chainCoefficients[0]);
//        }
//        
//        
//        oldCutParams = cutParams;
//        
//    }
//    else
//    {
//        FilterParameters parametricParams;
//        
//        parametricParams.gain = gain;
//        parametricParams.frequency = freq;
//        parametricParams.sampleRate = getSampleRate();
//        parametricParams.quality = qual;
//        parametricParams.bypassed = bypass;
//        parametricParams.filterType = type;
//        
//        if ( type != oldFilterType || parametricParams != oldParametricParams )
//        {
//            auto chainCoefficients = CoefficientMaker::makeCoefficients(parametricParams);
//            leftChain.setBypassed<0>(bypass);
//            rightChain.setBypassed<0>(bypass);
//            *(leftChain.get<0>().coefficients) = *chainCoefficients;
//            *(rightChain.get<0>().coefficients) = *chainCoefficients;
//        }
//        
//        oldParametricParams = parametricParams;
//    }
//    
//    
//}

void Project11v2AudioProcessor::updateFilters(double sampleRate, bool forceUpdate)
{
    updateCutFilter<0>(sampleRate, forceUpdate, oldHighCutParams, true);
    updateParametricFilter<1>(sampleRate, forceUpdate);
    updateCutFilter<2>(sampleRate, forceUpdate, oldLowCutParams, false);
}


//==============================================================================

void Project11v2AudioProcessor::addFilterParamToLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout,
                            int filterNum,
                            bool isCut)
{
    const int versionID = 1;
    
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{generateBypassParamString(filterNum), versionID},generateBypassParamString(filterNum),false) );
        
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{generateFreqParamString(filterNum), versionID}, generateFreqParamString(filterNum),
                                       juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.25f), 20.0f));
    
    if (!isCut )
    {
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{generateQParamString(filterNum), versionID}, generateQParamString(filterNum),
                                           juce::NormalisableRange<float>(0.1f, 10.f, 0.05f, 1.0f), 1.0f));
        
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{generateGainParamString(filterNum), versionID}, generateGainParamString(filterNum),
                                           juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.0f), 0.0f));

        
        juce::StringArray types;
        
        for (const auto& [type, stringRep] : FilterInfo::filterToString)
        {
          //Verify map is sorted like I believe it is by the standard
          // DBG( "Key as int:" + std::to_string(static_cast<int>(type)));
            types.add(stringRep);
        }
        
        
        layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{generateTypeParamString(filterNum), versionID}, generateTypeParamString(filterNum), types, 0));
        
    }
    else
    {
        juce::StringArray slopes;
                
                for (const auto& [order, stringRep] : FilterInfo::slopeToString)
                {
                    slopes.add(stringRep);
                }

        layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{generateSlopeParamString(filterNum), versionID},
                                                                generateSlopeParamString(filterNum), slopes, 0));
            
    }

}


juce::AudioProcessorValueTreeState::ParameterLayout Project11v2AudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    addFilterParamToLayout(layout, 0, true);
    addFilterParamToLayout(layout, 1, false);
    addFilterParamToLayout(layout, 2, true);
    
    return layout;
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
    
    updateFilters(sampleRate,true);
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
    
    updateFilters( getSampleRate() );
    
    juce::dsp::AudioBlock<float> block(buffer);
    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);
     
    
    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);
    leftChain.process(leftContext);
    rightChain.process(rightContext);

    // test filter functions
//    FilterParameters filterParams;  //use default values;
//    auto coefficients = CoefficientMaker::makeCoefficients(filterParams);
//       
//    HighCutLowCutParameters lowCutParams;
//    auto coefficientsArray = CoefficientMaker::makeCoefficients(lowCutParams);
    

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
}

void Project11v2AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Project11v2AudioProcessor();
}
