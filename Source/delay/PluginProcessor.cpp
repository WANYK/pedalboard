/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


String DelayAudioProcessor::paramGain("gain");
String DelayAudioProcessor::paramTime("time");
String DelayAudioProcessor::paramFeedback("feedback");


//==============================================================================
DelayAudioProcessor::DelayAudioProcessor()
    : mState(*this, &mUndoManager, "FFTapeDelay",
        {
//dla gain
            std::make_unique<AudioParameterFloat>(paramGain,
                                                  TRANS("Input Gain"),
                                                  NormalisableRange<float>(-100.0f, 6.0f, 0.1f, std::log(0.5f) / std::log(100.0f / 106.0f)),
                                                  mdgain.get(), "dB",
                                                  AudioProcessorParameter::genericParameter,
                                                  [](float v, int) { return String(v, 1) + " dB"; },
                                                  [](const String& t) { return t.dropLastCharacters(3).getFloatValue(); }),
//dla time
            std::make_unique<AudioParameterFloat>(paramTime,
                                                  TRANS("Delay TIme"),    NormalisableRange<float>(0.0, 2000.0, 1.0),
                                                  mdtime.get(), "ms",
                                                  AudioProcessorParameter::genericParameter,
                                                  [](float v, int) { return String(roundToInt(v)) + " ms"; },
                                                  [](const String& t) { return t.dropLastCharacters(3).getFloatValue(); }),
//dla feedback
            std::make_unique<AudioParameterFloat>(paramFeedback,
                                                  TRANS("Feedback Gain"), NormalisableRange<float>(-100.0f, 6.0f, 0.1f, std::log(0.5f) / std::log(100.0f / 106.0f)),
                                                  mdfeedback.get(), "dB", AudioProcessorParameter::genericParameter,
                                                  [](float v, int) { return String(v, 1) + " dB"; },
                                                  [](const String& t) { return t.dropLastCharacters(3).getFloatValue(); })
        })
{
//wywołanie zwrotne gain
    mState.addParameterListener(paramGain, this);
//wywołanie zwrotne time
    mState.addParameterListener(paramTime, this);
//wywołanie zwrotne feedback
    mState.addParameterListener(paramFeedback, this);
}

DelayAudioProcessor::~DelayAudioProcessor()
{
}

//==============================================================================
void DelayAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    mdsample_rate = sampleRate;

    // sample buffer dla 2 sekund + 2 buffers safety
    mDelayBuffer.setSize(getTotalNumOutputChannels(), 2.0 * (samplesPerBlock + sampleRate), false, false);
    mDelayBuffer.clear();

    mdexpected_read_position = -1;
}

void DelayAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void DelayAudioProcessor::parameterChanged(const String& parameterID, float newValue)
{
//jeśli parametr Gain jest zmieniany przypisuje mu nową wartosc
    if (parameterID == paramGain) {
        mdgain = newValue;
    }
//jeśli parametr Time jest zmieniany przypisuje mu nową wartosc
    else if (parameterID == paramTime) {
        mdtime = newValue;
    }
//jeśli parametr Feedback jest zmieniany przypisuje mu nową wartosc
    else if (parameterID == paramFeedback) {
        mdfeedback = newValue;
    }
}

bool DelayAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    // wejscia i wyjscia, tylko dla stereo and mono
    if (layouts.getMainInputChannels() == 0 || layouts.getMainInputChannels() > 2)
        return false;

    if (layouts.getMainOutputChannels() == 0 || layouts.getMainOutputChannels() > 2)
        return false;

    // nie zezwalamy na zawężanie liczby kanałów
    if (layouts.getMainInputChannels() > layouts.getMainOutputChannels())
        return false;

    return true;
}

void DelayAudioProcessor::processBlock(AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    if (Bus* inputBus = getBus(true, 0))
    {
        const float gain = Decibels::decibelsToGain(mdgain.get());
        const float time = mdtime.get();
        const float feedback = Decibels::decibelsToGain(mdfeedback.get());

        // orginał do delay
        for (int i = 0; i < mDelayBuffer.getNumChannels(); ++i)
        {
            const int inputChannelNum = inputBus->getChannelIndexInProcessBlockBuffer(std::min(i, inputBus->getNumberOfChannels()));
            writeToDelayBuffer(buffer, inputChannelNum, i, mdwrite_position, 1.0f, 1.0f, true);
        }

        // dostosowanie wzmocnienie na sucho
        buffer.applyGainRamp(0, buffer.getNumSamples(), mdlast_input_gain, gain);
        mdlast_input_gain = gain;

        // czytanie opoznionego sygnału
        auto readPos = roundToInt(mdwrite_position - (mdsample_rate * time / 1000.0));
        if (readPos < 0)
            readPos += mDelayBuffer.getNumSamples();

        if (Bus* outputBus = getBus(false, 0))
        {
            // jesli wczesniej wlaczone
            if (mdexpected_read_position >= 0)
            {
                // znikną, jeśli readPos jest wyłączone
                auto endGain = (readPos == mdexpected_read_position) ? 1.0f : 0.0f;
                for (int i = 0; i < outputBus->getNumberOfChannels(); ++i)
                {
                    const int outputChannelNum = outputBus->getChannelIndexInProcessBlockBuffer(i);
                    readFromDelayBuffer(buffer, i, outputChannelNum, mdexpected_read_position, 1.0, endGain, false);
                }
            }

            // znikną w nowej pozycji
            if (readPos != mdexpected_read_position)
            {
                for (int i = 0; i < outputBus->getNumberOfChannels(); ++i)
                {
                    const int outputChannelNum = outputBus->getChannelIndexInProcessBlockBuffer(i);
                    readFromDelayBuffer(buffer, i, outputChannelNum, readPos, 0.0, 1.0, false);
                }
            }
        }

        // dodanie feedback do delay
        for (int i = 0; i < inputBus->getNumberOfChannels(); ++i)
        {
            const int outputChannelNum = inputBus->getChannelIndexInProcessBlockBuffer(i);
            writeToDelayBuffer(buffer, outputChannelNum, i, mdwrite_position, mdlast_feedback_gain, feedback, false);
        }
        mdlast_feedback_gain = feedback;

        // advance positions
        mdwrite_position += buffer.getNumSamples();
        if (mdwrite_position >= mDelayBuffer.getNumSamples())
            mdwrite_position -= mDelayBuffer.getNumSamples();

        mdexpected_read_position = readPos + buffer.getNumSamples();
        if (mdexpected_read_position >= mDelayBuffer.getNumSamples())
            mdexpected_read_position -= mDelayBuffer.getNumSamples();
    }
}
//Zapisuje dokument do pliku jako UTF-8
void DelayAudioProcessor::writeToDelayBuffer(AudioSampleBuffer& buffer,
    const int channelIn, const int channelOut,
    const int writePos, float startGain, float endGain, bool replacing)
{
    if (writePos + buffer.getNumSamples() <= mDelayBuffer.getNumSamples())
    {
        if (replacing)
//Kopiuje próbki z szeregu liczb zmiennoprzecinkowych kanalu channelout, stosując rampę wzmocnienia
            mDelayBuffer.copyFromWithRamp(channelOut, writePos, buffer.getReadPointer(channelIn), buffer.getNumSamples(), startGain, endGain);
        else
//Dodaje próbki z szeregu liczb zmiennoprzecinkowych, stosując do nich rampę wzmocnienia
            mDelayBuffer.addFromWithRamp(channelOut, writePos, buffer.getReadPointer(channelIn), buffer.getNumSamples(), startGain, endGain);
    }
    else
    {
        const auto midPos = mDelayBuffer.getNumSamples() - writePos;
        const auto midGain = jmap(float(midPos) / buffer.getNumSamples(), startGain, endGain);
        if (replacing)
        {
//Kopiuje próbki z szeregu liczb zmiennoprzecinkowych channelout, stosując rampę wzmocnienia
            mDelayBuffer.copyFromWithRamp(channelOut, writePos, buffer.getReadPointer(channelIn), midPos, startGain, midGain);
            mDelayBuffer.copyFromWithRamp(channelOut, 0, buffer.getReadPointer(channelIn, midPos), buffer.getNumSamples() - midPos, midGain, endGain);
        }
        else
        {
//Dodaje próbki z szeregu liczb zmiennoprzecinkowych, stosując do nich rampę wzmocnienia
            mDelayBuffer.addFromWithRamp(channelOut, writePos, buffer.getReadPointer(channelIn), midPos, mdlast_input_gain, midGain);
            mDelayBuffer.addFromWithRamp(channelOut, 0, buffer.getReadPointer(channelIn, midPos), buffer.getNumSamples() - midPos, midGain, endGain);
        }
    }
}

void DelayAudioProcessor::readFromDelayBuffer(AudioSampleBuffer& buffer,
    const int channelIn, const int channelOut,
    const int readPos,
    float startGain, float endGain,
    bool replacing)
{
    if (readPos + buffer.getNumSamples() <= mDelayBuffer.getNumSamples())
    {
        if (replacing)
//Kopiuje próbki z szeregu liczb zmiennoprzecinkowych channelout, stosując rampę wzmocnienia
            buffer.copyFromWithRamp(channelOut, 0, mDelayBuffer.getReadPointer(channelIn, readPos), buffer.getNumSamples(), startGain, endGain);
        else
//Dodaje próbki z szeregu liczb zmiennoprzecinkowych, stosując do nich rampę wzmocnienia
            buffer.addFromWithRamp(channelOut, 0, mDelayBuffer.getReadPointer(channelIn, readPos), buffer.getNumSamples(), startGain, endGain);
    }
    else
    {
        const auto midPos = mDelayBuffer.getNumSamples() - readPos;
        const auto midGain = jmap(float(midPos) / buffer.getNumSamples(), startGain, endGain);
        if (replacing)
        {
//Kopiuje próbki z szeregu liczb zmiennoprzecinkowych channelout, stosując rampę wzmocnienia
            buffer.copyFromWithRamp(channelOut, 0, mDelayBuffer.getReadPointer(channelIn, readPos), midPos, startGain, midGain);
            buffer.copyFromWithRamp(channelOut, midPos, mDelayBuffer.getReadPointer(channelIn), buffer.getNumSamples() - midPos, midGain, endGain);
        }
        else
        {
//Dodaje próbki z szeregu liczb zmiennoprzecinkowych, stosując do nich rampę wzmocnienia
            buffer.addFromWithRamp(channelOut, 0, mDelayBuffer.getReadPointer(channelIn, readPos), midPos, startGain, midGain);
            buffer.addFromWithRamp(channelOut, midPos, mDelayBuffer.getReadPointer(channelIn), buffer.getNumSamples() - midPos, midGain, endGain);
        }
    }
}

AudioProcessorValueTreeState& DelayAudioProcessor::getValueTreeState()
{
    return mState;
}

//==============================================================================
bool DelayAudioProcessor::hasEditor() const
{
    return true;
}

AudioProcessorEditor* DelayAudioProcessor::createEditor()
{
    return new DelayAudioProcessorEditor(*this);
}

//==============================================================================
void DelayAudioProcessor::getStateInformation(MemoryBlock& destData)
{
    // przechowywanie parametrów w bloku pamięci
    MemoryOutputStream stream(destData, false);
    mState.state.writeToStream(stream);
}

void DelayAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
// aby przywrócić parametry z tego bloku pamięci,
// którego zawartość zostanie utworzona przez wywołanie getStateInformation ().
    ValueTree tree = ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid()) {
        mState.state = tree;
    }
}

//==============================================================================
const String DelayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DelayAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool DelayAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

double DelayAudioProcessor::getTailLengthSeconds() const
{
    return 2.0;
}

int DelayAudioProcessor::getNumPrograms()
{
    return 1;   
}

int DelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DelayAudioProcessor::setCurrentProgram(int index)
{
}

const String DelayAudioProcessor::getProgramName(int index)
{
    return String();
}

void DelayAudioProcessor::changeProgramName(int index, const String& newName)
{
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DelayAudioProcessor();
}