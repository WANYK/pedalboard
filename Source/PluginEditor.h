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
class ChoursAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    ChoursAudioProcessorEditor (ChorusAudioProcessor&);
    ~ChoursAudioProcessorEditor();
    
    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    ChorusAudioProcessor& processor;
    enum {
        editor_width = 500,
        editor_margin = 10,
        editor_padding = 10,

        slider_text_entry_box_width = 100,
        slider_text_entry_box_height = 25,
        slider_height = 25,
        button_height = 25,
        combo_box_height = 25,
        label_width = 100,
    };
    OwnedArray<Slider> sliders;
    OwnedArray<ToggleButton> toggles;
    OwnedArray<ComboBox> comboBoxes;

    OwnedArray<Label> labels;
    Array<Component*> components;

    typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
    typedef AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

    OwnedArray<SliderAttachment> sliderAttachments;
    OwnedArray<ButtonAttachment> buttonAttachments;
    OwnedArray<ComboBoxAttachment> comboBoxAttachments;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChoursAudioProcessorEditor)
};
