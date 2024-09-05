/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DistorzijaBasicAudioProcessor::DistorzijaBasicAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
, treeState(*this, nullptr, "PARAMETERS", createParameterLayout())
#endif
{
    treeState.addParameterListener("input", this);
    treeState.addParameterListener("output", this);
}

DistorzijaBasicAudioProcessor::~DistorzijaBasicAudioProcessor()
{
    treeState.removeParameterListener("input", this);
    treeState.removeParameterListener("output", this);
}

juce::AudioProcessorValueTreeState::ParameterLayout DistorzijaBasicAudioProcessor::createParameterLayout()
{
    std::vector <std::unique_ptr<juce::RangedAudioParameter>> params;

    auto pInput = std::make_unique<juce::AudioParameterFloat>("input", "Gain", -24.0f, 24.0f, 0.0f);
    auto pOutput = std::make_unique<juce::AudioParameterFloat>("output", "Level", 0.0f, 100.0f, 0.0f);
    
    params.push_back(std::move(pInput));
    params.push_back(std::move(pOutput));

    return { params.begin(), params.end() };
}

void DistorzijaBasicAudioProcessor::parameterChanged(const juce::String &parameterID, float newValue)
{
    updateParameters();
}

void DistorzijaBasicAudioProcessor::updateParameters()
{
    inputModule.setGainDecibels(treeState.getRawParameterValue("input")->load());
    outputModule.setGainDecibels(treeState.getRawParameterValue("output")->load());
}
//==============================================================================
const juce::String DistorzijaBasicAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DistorzijaBasicAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DistorzijaBasicAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DistorzijaBasicAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DistorzijaBasicAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DistorzijaBasicAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DistorzijaBasicAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DistorzijaBasicAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DistorzijaBasicAudioProcessor::getProgramName (int index)
{
    return {};
}

void DistorzijaBasicAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DistorzijaBasicAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();

    inputModule.prepare(spec);
    inputModule.setRampDurationSeconds(0.02);

    outputModule.prepare(spec);
    outputModule.setRampDurationSeconds(0.02);

    distortionModule.prepare(spec);
    limiterModule.prepare(spec);
    limiterModule.setThreshold(0.99);
    limiterModule.setRelease(1.0f);
    updateParameters();
}

void DistorzijaBasicAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DistorzijaBasicAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void DistorzijaBasicAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    juce::dsp::AudioBlock<float> block{ buffer };
    
    inputModule.process(juce::dsp::ProcessContextReplacing<float>(block));
    distortionModule.process(juce::dsp::ProcessContextReplacing<float>(block));
    limiterModule.process(juce::dsp::ProcessContextReplacing<float>(block));
    outputModule.process(juce::dsp::ProcessContextReplacing<float>(block));
}

//==============================================================================
bool DistorzijaBasicAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DistorzijaBasicAudioProcessor::createEditor()
{
    return new DistorzijaBasicAudioProcessorEditor (*this);
    // return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void DistorzijaBasicAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream stream(destData, false);
}

void DistorzijaBasicAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto tree = juce::ValueTree::readFromData(data, size_t(sizeInBytes));
    if (tree.isValid())
    {
        treeState.state = tree;
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DistorzijaBasicAudioProcessor();
}
