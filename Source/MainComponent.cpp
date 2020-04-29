#include "MainComponent.h"

MainComponent::MainComponent() : state(Stopped), openButton("Open"), playButton("Play"), stopButton("Stop")
{
    setSize(800, 600);
    setAudioChannels(0, 2);

    openButton.onClick = [this] { openButtonClicked(); };
    openButton.setColour(TextButton::buttonColourId, Colours::yellow);
    addAndMakeVisible(&openButton);

    playButton.onClick = [this] { playButtonClicked(); };
    playButton.setColour(TextButton::buttonColourId, Colours::blue);
    playButton.setEnabled(true);
    addAndMakeVisible(&playButton);

    stopButton.onClick = [this] {stopButtonClicked(); };
    stopButton.setColour(TextButton::buttonColourId, Colours::red);
    stopButton.setEnabled(false);
    addAndMakeVisible(&stopButton);

    formatManager.registerBasicFormats();
   //myTransport.addChangeListener(this);
}




MainComponent::~MainComponent()
{
    shutdownAudio();                                                                        //Wy³¹cza urz¹dzenie audio i usuwa Ÿród³o dŸwiêku
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transport.prepareToPlay(samplesPerBlockExpected, sampleRate);                           //Bierze 2 argumenty o naszym rozmiarze bufora
}

void MainComponent::openButtonClicked()
{
    //Wybraæ plik
    FileChooser chooser("Choose a wav or AIFF File", File::getSpecialLocation(File::userDocumentsDirectory),
        "*.wav; *.aiff; *.mp3");                                                            //Mo¿emy u¿ywaæ proste pliki WAV i AIFF i MP3
    //Jezeli zostanie wybrany 
    if (chooser.browseForFileToOpen())
    {
        File myFile;

        //Co wybierzemy?
        myFile = chooser.getResult();                                                       //Zwraca typ pliku

        //Czytaj plik
        AudioFormatReader* reader = formatManager.createReaderFor(myFile);                  //Tworzymy czytnik, który jest w stanie odczytaæ nasz plik 

        //Plik gotowy do grania
        if (reader != nullptr)
        {
            std::unique_ptr<AudioFormatReaderSource> tempSource(new AudioFormatReaderSource(reader, true));
            transport.setSource(tempSource.get());
            transportStateChanged(Stopped);
            playSource.reset(tempSource.release());                                         //Tymczasowe zwolnienie Ÿród³a
        }
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

        switch (state)
        {
        //stopped
            //Ustawienie na 0 - sprowadzenie utworu na sam pocz¹tek
        case Stopped:
            playButton.setEnabled(true);                                                   //Jeœli utwór jest zatrzymany, przycisk odtwarzania jest w³¹czony
            transport.setPosition(0.0);                                                    
            break;

        //Playing
           //play w³¹czony => bo jak gra kawa³ek i w tym czasie
           //wgram plik to muszê mieæ przycisk start a nie stop
        case Playing:
            playButton.setEnabled(true);
           //stopButton.setEnabled(false);
            break;

        //Starting
            //stop w³¹czone
            //play wy³¹czone
        case Starting:
            stopButton.setEnabled(true);
            playButton.setEnabled(false);
            transport.start();
            break;

        //Stopping
            //play w³¹czony
            //stop wy³¹czony
        case Stopping:
            playButton.setEnabled(true);
            stopButton.setEnabled(false);
            transport.stop();
            break;
        }
    }
}

void MainComponent::changeListenerCallback(ChangeBroadcaster* source)                      //Definiujemy co siê stanie jak nast¹pi¹ zmiany w pliku który czytamy
{
    if (source == &transport)
    {
        if (transport.isPlaying())
        {
            transportStateChanged(Playing);
        }
        else
        {
            transportStateChanged(Stopped);
        }
    }
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();
    transport.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{

}

void MainComponent::paint (Graphics& g)
{
    g.fillAll (Colours::black);                                                            //T³o
}

void MainComponent::resized()
{
    openButton.setBounds(10, 10, 50, 30);                                                 // x, y, szerokoœæ, wysokoœæ
    playButton.setBounds(700, 60, 50, 30);
    stopButton.setBounds(800, 60, 50, 30);
}
