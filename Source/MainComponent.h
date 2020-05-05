#pragma once

#include <JuceHeader.h>

class MainComponent   : public AudioAppComponent,
                        public ChangeListener
{
public:
    MainComponent();
    ~MainComponent();

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void paint (Graphics& g) override;
    void resized() override;

    void paintIfNoFileLoaded(Graphics& g, const Rectangle<int>& thumbnailBounds);
    void paintIfFileLoaded(Graphics& g, const Rectangle<int>& thumbnailBounds);

private:
    enum TransportState
    {
        Stopped,
        Starting,
        Stopping,
        Playing
    };

    void openButtonClicked();                                                                               //Okre�lamy co si� stanie jak przycisk zostanie klikni�ty
    void playButtonClicked();
    void stopButtonClicked();
    void transportStateChanged(TransportState newState);
    void changeListenerCallback(ChangeBroadcaster* source) override;                                        //musimy mie� czyst� funkcj� wirtualn�, bo dzie

    AudioFormatManager formatManager;                                                                       
    std::unique_ptr<AudioFormatReaderSource> playSource;                                                    //playSource - miejsce z kt�rego aktualnie odtwarzamy plik
    AudioTransportSource transport;

    TextButton openButton;
    TextButton playButton;
    TextButton stopButton;

    TransportState state;
    AudioThumbnailCache thumbnailCache;    
    AudioThumbnail thumbnail;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
