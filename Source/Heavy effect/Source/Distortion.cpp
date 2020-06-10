/*
  ==============================================================================

    Distortion.cpp
    Created: 2 Apr 2020 2:31:56pm
    Author:  Iza

  ==============================================================================
*/
#include "Distortion.h"
#include "Processor.h"

DistortionAudioProcessorEditor::DistortionAudioProcessorEditor(DistortionAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    const Array<AudioProcessorParameter*> Parameters = processor.getParameters();
    int comboBoxCounter = 0;

    int editorHeight = 2 * editorMargin;
    for (int i = 0; i < Parameters.size(); ++i)
    {
        if (const AudioProcessorParameterWithID* parameter =
            dynamic_cast<AudioProcessorParameterWithID*> (Parameters[i]))
        {
            if (processor.Parameters.Types[i] == "Slider")
            {
                Slider* aSlider;
                sliders.add(aSlider = new Slider());
                aSlider->setTextValueSuffix(parameter->label);
                aSlider->setTextBoxStyle(Slider::TextBoxLeft,
                    false,
                    sliderTextEntryBoxWidth,
                    sliderTextEntryBoxHeight);

                SliderAttachment* aSliderAttachment;
                sliderAttachments.add(aSliderAttachment =
                    new SliderAttachment(processor.Parameters.valueTreeState, parameter->paramID, *aSlider));

                components.add(aSlider);
                editorHeight += sliderHeight;
            }

            else if (processor.Parameters.Types[i] == "ToggleButton")
            {
                ToggleButton* aButton;
                toggles.add(aButton = new ToggleButton());
                aButton->setToggleState(parameter->getDefaultValue(), dontSendNotification);

                ButtonAttachment* aButtonAttachment;
                buttonAttachments.add(aButtonAttachment =
                    new ButtonAttachment(processor.Parameters.valueTreeState, parameter->paramID, *aButton));

                components.add(aButton);
                editorHeight += buttonHeight;
            }

            else if (processor.Parameters.Types[i] == "ComboBox")
            {
                ComboBox* aComboBox;
                comboBoxes.add(aComboBox = new ComboBox());
                aComboBox->setEditableText(false);
                aComboBox->setJustificationType(Justification::left);
                aComboBox->addItemList(processor.Parameters.boxLists[comboBoxCounter++], 1);

                ComboBoxAttachment* aComboBoxAttachment;
                comboBoxAttachments.add(aComboBoxAttachment =
                    new ComboBoxAttachment(processor.Parameters.valueTreeState, parameter->paramID, *aComboBox));

                components.add(aComboBox);
                editorHeight += comboBoxHeight;
            }

            Label* aLabel;
            labels.add(aLabel = new Label(parameter->name, parameter->name));
            aLabel->attachToComponent(components.getLast(), true);
            addAndMakeVisible(aLabel);

            components.getLast()->setName(parameter->name);
            components.getLast()->setComponentID(parameter->paramID);
            addAndMakeVisible(components.getLast());
        }
    }

    editorHeight += components.size() * editorPadding;
    setSize(editorWidth, editorHeight);
}

DistortionAudioProcessorEditor::~DistortionAudioProcessorEditor()
{
}

void DistortionAudioProcessorEditor::paint(Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
    g.fillAll(Colours::orange);
}

void DistortionAudioProcessorEditor::resized()
{
    Rectangle<int> r = getLocalBounds().reduced(editorMargin);
    r = r.removeFromRight(r.getWidth() - labelWidth);

    for (int i = 0; i < components.size(); ++i) {
        if (Slider* aSlider = dynamic_cast<Slider*> (components[i]))
            components[i]->setBounds(r.removeFromTop(sliderHeight));

        if (ToggleButton* aButton = dynamic_cast<ToggleButton*> (components[i]))
            components[i]->setBounds(r.removeFromTop(buttonHeight));

        if (ComboBox* aComboBox = dynamic_cast<ComboBox*> (components[i]))
            components[i]->setBounds(r.removeFromTop(comboBoxHeight));

        r = r.removeFromBottom(r.getHeight() - editorPadding);
    }
}