/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
//klasa DelayAudioProcessor
class DelayAudioProcessor : public AudioProcessor,
    public AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
//konstruktor
    DelayAudioProcessor();
//destruktor
    ~DelayAudioProcessor();

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    void parameterChanged(const String& parameterID, float newValue) override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(AudioSampleBuffer&, MidiBuffer&) override;

    void writeToDelayBuffer(AudioSampleBuffer& buffer,
        const int channelIn, const int channelOut,
        const int writePos,
        float startGain, float endGain,
        bool replacing);

    void readFromDelayBuffer(AudioSampleBuffer& buffer,
        const int channelIn, const int channelOut,
        const int readPos,
        float startGain, float endGain,
        bool replacing);

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const String getProgramName(int index) override;
    void changeProgramName(int index, const String& newName) override;

    //==============================================================================
    void getStateInformation(MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    AudioProcessorValueTreeState& getValueTreeState();

    static String paramGain;
    static String paramTime;
    static String paramFeedback;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayAudioProcessor)

        Atomic<float>   mdgain{ 0.0f };
    Atomic<float>   mdtime{ 200.0f };
    Atomic<float>   mdfeedback{ -6.0f };

    UndoManager                  mUndoManager;
    AudioProcessorValueTreeState mState;
//stworzenie buforu do delay
    AudioSampleBuffer            mDelayBuffer;

    float mdlast_input_gain = 0.0f;
    float mdlast_feedback_gain = 0.0f;

    int    mdwrite_position = 0;
    int    mdexpected_read_position = -1;
    double mdsample_rate = 0;
};