/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DistorzijaBasicAudioProcessorEditor::DistorzijaBasicAudioProcessorEditor (DistorzijaBasicAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (200, 200);
    addAndMakeVisible(gainSlider);
    gainSlider.setSliderStyle(juce::Slider::Rotary);
    gainSliderAttachmentptr.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.treeState, "input", gainSlider));
    addAndMakeVisible(levelSlider);
    levelSlider.setSliderStyle(juce::Slider::Rotary);
    levelSliderAttachmentptr.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.treeState, "output", levelSlider));
}

DistorzijaBasicAudioProcessorEditor::~DistorzijaBasicAudioProcessorEditor()
{
}

//==============================================================================
void DistorzijaBasicAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::black);

    g.setColour(juce::Colours::white);
    g.setFont(14);
    g.drawFittedText("Gain (dB)", gainSlider.getBounds(), juce::Justification::centredBottom, 1);
    g.drawFittedText("Level", levelSlider.getBounds(), juce::Justification::centredBottom, 1);

    juce::Line<float> line(juce::Point<float>(0, 100), juce::Point<float>(200, 100));
    g.drawLine(line, 1.0f);
    juce::Line<float> line1(juce::Point<float>(0, 200), juce::Point<float>(200, 200));
    g.drawLine(line1, 1.0f);
}

void DistorzijaBasicAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto bounds = getLocalBounds();
    gainSlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.5));
    levelSlider.setBounds(bounds.removeFromTop(bounds.getHeight()));
}
