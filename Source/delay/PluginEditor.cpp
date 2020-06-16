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
     //Dodaje do niego komponent potomny, a także sprawia, że jest on widoczny, jeśli jeszcze nie jest
      //komponentem potomnym jest mdgain_slider
    addAndMakeVisible(mdgain_slider);
       //komponentem potomnym jest mdtime_slider
    addAndMakeVisible(mdtime_slider);
      //komponentem potomnym jest mdfeedback_slider
    addAndMakeVisible(mdfeedback_slider);
      //Zmienia rozmiar komponentu
    setSize(400, 250);
}

DelayAudioProcessorEditor::~DelayAudioProcessorEditor()
{
}

//==============================================================================
//funkcja do wersji garficznej
void DelayAudioProcessorEditor::paint(Graphics& g)
{
  //ustawia kolor tła
    g.fillAll(Colours::black);
//ustawia kolor przycisków
    g.setColour(Colours::red);
    g.setFont(24.0f);

    auto box = getLocalBounds().reduced(20);
    box = box.withTop(box.getBottom() - 40);

    const auto width = box.getWidth() / 3;
  //funkcje do wersji graficznej przycisków
  //do przycisku Dry Gain
    g.drawFittedText(TRANS("Dry Gain"), box.removeFromLeft(width), Justification::centred, 1);
  //do przycisku time
    g.drawFittedText(TRANS("Time"), box.removeFromLeft(width), Justification::centred, 1);
  //do przycisku feedback
    g.drawFittedText(TRANS("Feedback"), box.removeFromLeft(width), Justification::centred, 1);

}
//rozmiar tego komponentu zostały zmienione
void DelayAudioProcessorEditor::resized()
{
  //Zwraca granice komponentu względem jego własnego początku
    auto box = getLocalBounds().reduced(20);
  //zwraca box znajdujący się w miejscu oryginalnego box
    box.removeFromBottom(40);

 //Zwraca szerokość box w pikselach
    const auto width = box.getWidth() / 3;
    mdgain_slider.setBounds(box.removeFromLeft(width).reduced(10));
    mdtime_slider.setBounds(box.removeFromLeft(width).reduced(10));
    mdfeedback_slider.setBounds(box.removeFromLeft(width).reduced(10));
}
