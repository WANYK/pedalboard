#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class MainComponent : public AudioAppComponent
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint(Graphics& g) override;
    void resized() override;

private:
    AudioDeviceManager otherDeviceManager;
    std::unique_ptr <AudioDeviceSelectorComponent> audioSettings;

    enum TransportState
    {
        Stopped,
        Starting,
        Stopping,
    };

    TransportState state;

    void openButtonClicked();

    void playButtonClicked();
    void stopButtonClicked();
    void transportStateChanged(TransportState newState);

    AudioFormatManager formatManager;
    std::unique_ptr<AudioFormatReaderSource> playSource;
    AudioTransportSource transport;

    TextButton openButton;
    TextButton playButton;
    TextButton stopButton;

    //==============================================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};