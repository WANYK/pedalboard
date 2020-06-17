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
//klasa DelayAudioProcessorEditor
class DelayAudioProcessorEditor : public AudioProcessorEditor
{
public:
//konstruktor
    DelayAudioProcessorEditor(DelayAudioProcessor&);
//destruktor
    ~DelayAudioProcessorEditor();

    //==============================================================================
//do wersji graficznej
    void paint(Graphics&) override;
    void resized() override;

private:
//szybki sposob dostępu do obiektu procesora
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