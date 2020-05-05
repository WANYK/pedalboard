#include "MainComponent.h"

MainComponent::MainComponent() : state(Stopped), openButton("Open"), playButton("Play"), stopButton("Stop"),
                                 thumbnailCache(5),                            
                                 thumbnail(512, formatManager, thumbnailCache) 
{
    setSize(800, 600);
    setAudioChannels(0, 2);

    thumbnail.addChangeListener(this);

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
    shutdownAudio();                                                                        //Wy��cza urz�dzenie audio i usuwa �r�d�o d�wi�ku
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transport.prepareToPlay(samplesPerBlockExpected, sampleRate);                           //Bierze 2 argumenty o naszym rozmiarze bufora
}

void MainComponent::openButtonClicked()
{
    //Wybra� plik
    FileChooser chooser("Choose a wav or AIFF File", File::getSpecialLocation(File::userDocumentsDirectory),
        "*.wav; *.aiff; *.mp3");                                                            //Mo�emy u�ywa� proste pliki WAV i AIFF i MP3
    //Jezeli zostanie wybrany 
    if (chooser.browseForFileToOpen())
    {
        File myFile;

        //Co wybierzemy?
        myFile = chooser.getResult();                                                       //Zwraca typ pliku

        //Czytaj plik
        AudioFormatReader* reader = formatManager.createReaderFor(myFile);                  //Tworzymy czytnik, kt�ry jest w stanie odczyta� nasz plik 

        //Plik gotowy do grania
        if (reader != nullptr)
        {
            std::unique_ptr<AudioFormatReaderSource> tempSource(new AudioFormatReaderSource(reader, true));
            transport.setSource(tempSource.get());
            transportStateChanged(Stopped);
            thumbnail.setSource(new FileInputSource(myFile));
            playSource.reset(tempSource.release());                                         //Tymczasowe zwolnienie �r�d�a
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
            //Ustawienie na 0 - sprowadzenie utworu na sam pocz�tek
        case Stopped:
            playButton.setEnabled(true);                                                   //Je�li utw�r jest zatrzymany, przycisk odtwarzania jest w��czony
            transport.setPosition(0.0);                                                    
            break;

        //Playing
           //play w��czony => bo jak gra kawa�ek i w tym czasie
           //wgram plik to musz� mie� przycisk start a nie stop
        case Playing:
            playButton.setEnabled(true);
           //stopButton.setEnabled(false);
            break;

        //Starting
            //stop w��czone
            //play wy��czone
        case Starting:
            stopButton.setEnabled(true);
            playButton.setEnabled(false);
            transport.start();
            break;

        //Stopping
            //play w��czony
            //stop wy��czony
        case Stopping:
            playButton.setEnabled(true);
            stopButton.setEnabled(false);
            transport.stop();
            break;
        }
    }
}

void MainComponent::changeListenerCallback(ChangeBroadcaster* source)                      //Definiujemy co si� stanie jak nast�pi� zmiany w pliku kt�ry czytamy
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
    if (source == &thumbnail)
    {
        repaint();
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
    g.fillAll (Colours::black);                                                            //T�o

    Rectangle<int> thumbnailBounds(10, 100, getWidth() - 20, 120);

    if (thumbnail.getNumChannels() == 0)
        paintIfNoFileLoaded(g, thumbnailBounds);
    else
        paintIfFileLoaded(g, thumbnailBounds);
}

void MainComponent::paintIfNoFileLoaded(Graphics& g, const Rectangle<int>& thumbnailBounds)
{
    g.setColour(Colours::darkgrey);
    g.fillRect(thumbnailBounds);
    g.setColour(Colours::white);
    g.drawFittedText("No File Loaded", thumbnailBounds, Justification::centred, 1.0f);
}

void MainComponent::paintIfFileLoaded(Graphics& g, const Rectangle<int>& thumbnailBounds)
{
    g.setColour(Colours::white);
    g.fillRect(thumbnailBounds);

    g.setColour(Colours::red);

    thumbnail.drawChannels(g,
        thumbnailBounds,
        0.0,                                                                              // czas startu
        thumbnail.getTotalLength(),                                                       // koniec
        1.0f);
}

void MainComponent::resized()
{
    openButton.setBounds(10, 10, 50, 30);                                                 // x, y, szeroko��, wysoko��
    playButton.setBounds(690, 250, 50, 30);
    stopButton.setBounds(800, 250, 50, 30);
}
