/*
  ==============================================================================
    A simple editor for the delay plugin
    It contains the basic framework code for a JUCE plugin editor.
  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
DelayAudioProcessorEditor::DelayAudioProcessorEditor(DelayAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    addAndMakeVisible(mdgain_slider);
    addAndMakeVisible(mdtime_slider);
    addAndMakeVisible(mdfeedback_slider);

    setSize(400, 250);
}

DelayAudioProcessorEditor::~DelayAudioProcessorEditor()
{
}

//==============================================================================
void DelayAudioProcessorEditor::paint(Graphics& g)
{
    g.fillAll(Colours::black);

    g.setColour(Colours::black);
    g.setFont(24.0f);

    auto box = getLocalBounds().reduced(20);
    box = box.withTop(box.getBottom() - 40);

    const auto width = box.getWidth() / 3;
    g.drawFittedText(TRANS("Dry Gain"), box.removeFromLeft(width), Justification::centred, 1);
    g.drawFittedText(TRANS("Time"), box.removeFromLeft(width), Justification::centred, 1);
    g.drawFittedText(TRANS("Feedback"), box.removeFromLeft(width), Justification::centred, 1);

}

void DelayAudioProcessorEditor::resized()
{
    auto box = getLocalBounds().reduced(20);
    box.removeFromBottom(40);

    const auto width = box.getWidth() / 3;
    mdgain_slider.setBounds(box.removeFromLeft(width).reduced(10));
    mdtime_slider.setBounds(box.removeFromLeft(width).reduced(10));
    mdfeedback_slider.setBounds(box.removeFromLeft(width).reduced(10));
}