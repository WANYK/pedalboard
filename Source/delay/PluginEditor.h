/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class DelayAudioProcessorEditor : public AudioProcessorEditor
{
public:
    DelayAudioProcessorEditor(DelayAudioProcessor&);
    ~DelayAudioProcessorEditor();

    //==============================================================================
    void paint(Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DelayAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayAudioProcessorEditor)

        Slider mdgain_slider {
        Slider::RotaryHorizontalVerticalDrag, Slider::TextBoxBelow
    };
    Slider mdtime_slider{ Slider::RotaryHorizontalVerticalDrag,  Slider::TextBoxBelow };
    Slider mdfeedback_slider{ Slider::RotaryHorizontalVerticalDrag,  Slider::TextBoxBelow };

    AudioProcessorValueTreeState::SliderAttachment mGainAttachment{ processor.getValueTreeState(), DelayAudioProcessor::paramGain,     mdgain_slider };
    AudioProcessorValueTreeState::SliderAttachment mTimeAttachment{ processor.getValueTreeState(), DelayAudioProcessor::paramTime,     mdtime_slider };
    AudioProcessorValueTreeState::SliderAttachment mFeedbackAttachment{ processor.getValueTreeState(), DelayAudioProcessor::paramFeedback, mdfeedback_slider };
};