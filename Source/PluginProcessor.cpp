/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"



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
/*
 Need to change this as per step 9 - the errors in 'No member named coefficients' is due to the addition of more chains to the filter. So first chain is a cutFilter so will need relevant parameters, etc for the other chains.
 1. Everything needs bypass and freq
 
 2. Cut needs slopes
 
 3. Parametric needs type, Q and gain*/

void Project11v2AudioProcessor::addFilterParamToLayout (juce::AudioProcessorValueTreeState::ParameterLayout& layout, 
                                                        int filterNum,
                                                        bool isCut)
{
    
    const int versionID = 1;
    
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{generateBypassParamString(filterNum),versionID},                                                   generateBypassParamString(filterNum),
                                                          false));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{generateFreqParamString(filterNum), versionID},
                                                          generateFreqParamString(filterNum),
                                                          juce::NormalisableRange<float>(20.f, 20000.f, 10.f, 20.f),
                                                          440.f));
    
    if (!isCut)
    {
        
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{generateGainParamString(filterNum), versionID},
                   generateGainParamString(filterNum),
                   juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f),
                                                               0.f));
        
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{generateQParamString(filterNum), versionID},
                                                               generateQParamString(filterNum),
                                                               juce::NormalisableRange<float>(0.f, 10.f, 0.5f, 1.0f),
                                                               1.f));
        
        juce::StringArray types;
        
        for ( const auto& [name, stringRep] : FilterInfo::filterToString)
        {
            types.add(stringRep);
        }
        
        layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{generateTypeParamString(filterNum), versionID},
                                                                generateTypeParamString(filterNum),
                                                                types,
                                                                0));
        
        
    }
    else
    {
        //Slopes here for cut filters
        juce::StringArray slopes;
        
        for ( const auto& [type, stringRep] : FilterInfo::slopeToString )
        {
            slopes.add(stringRep);
        }
        
        layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{generateTypeParamString(filterNum), versionID}, generateTypeParamString(filterNum), slopes, 1));
    }

}

//==============================================================================


juce::AudioProcessorValueTreeState::ParameterLayout Project11v2AudioProcessor::createParameterLayout()
{
    
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    addFilterParamToLayout(layout, 0, true);
//    addFilterParamToLayout(layout, 1, false);
//    addFilterParamToLayout(layout, 2, true);
    
    return layout;
}


void Project11v2AudioProcessor::updateFilters(double sampleRate, bool forceUpdate)
{
    updateCutFilter<0>(sampleRate, forceUpdate, oldHighCutParams, true);
    updateParametricFilter<1>(sampleRate, forceUpdate);
    updateCutFilter<2>(sampleRate, forceUpdate, oldLowCutParams, false);
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
    
    updateFilters( getSampleRate(), false );
    

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
