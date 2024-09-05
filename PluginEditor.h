/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class DistorzijaBasicAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    DistorzijaBasicAudioProcessorEditor (DistorzijaBasicAudioProcessor&);
    ~DistorzijaBasicAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DistorzijaBasicAudioProcessor& audioProcessor;
    juce::Slider gainSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainSliderAttachmentptr;
    juce::Slider levelSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> levelSliderAttachmentptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistorzijaBasicAudioProcessorEditor)
};
