#include "MainComponent.h"


//==============================================================================
MainComponent::MainComponent() : state(Stopped), openButton("Open"), playButton("Play"), stopButton("Stop")
{
    setSize(200, 150);
    setAudioChannels(0, 2);

    openButton.onClick = [this] {  openButtonClicked(); };
    addAndMakeVisible(&openButton);

    playButton.onClick = [this] { playButtonClicked(); };
    playButton.setColour(TextButton::buttonColourId, Colours::green);
    playButton.setEnabled(true);
    addAndMakeVisible(&playButton);

    stopButton.onClick = [this] { stopButtonClicked(); };
    stopButton.setColour(TextButton::buttonColourId, Colours::red);
    stopButton.setEnabled(false);
    addAndMakeVisible(&stopButton);

    formatManager.registerBasicFormats();

}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transport.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::openButtonClicked()
{
    //choose a file
    FileChooser chooser("Choose a Wav or AIFF File", File::getSpecialLocation(File::userDesktopDirectory), "*.wav; *.mp3");

    //if the user chooses a file
    if (chooser.browseForFileToOpen())
    {
        File myFile;

        //what did the user choose?
        myFile = chooser.getResult();

        //read the file
        AudioFormatReader* reader = formatManager.createReaderFor(myFile);

          //get the file ready to play
            std::unique_ptr<AudioFormatReaderSource> tempSource(new AudioFormatReaderSource(reader, true));

            transport.setSource(tempSource.get());

            playSource.reset(tempSource.release());

    }
}

void MainComponent::playButtonClicked()
{
    transportStateChanged(Starting);
}

void MainComponent::stopButtonClicked()
{
    transportStateChanged(Stopping);
}

void MainComponent::transportStateChanged(TransportState newState)
{
    if (newState != state)
    {
        state = newState;

        switch (state) {
        case Stopped:
            playButton.setEnabled(true);
            transport.setPosition(0.0);
            break;

        case Starting:
            stopButton.setEnabled(true);
            playButton.setEnabled(false);
            transport.start();
            break;

        case Stopping:
            playButton.setEnabled(true);
            stopButton.setEnabled(false);
            transport.stop();
            break;
        }
    }
}

void MainComponent::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();

    transport.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.
}

//==============================================================================
void MainComponent::paint(Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

}

void MainComponent::resized()
{
    openButton.setBounds(10, 10, getWidth() - 20, 30);
    playButton.setBounds(10, 50, getWidth() - 20, 30);
    stopButton.setBounds(10, 90, getWidth() - 20, 30);
}