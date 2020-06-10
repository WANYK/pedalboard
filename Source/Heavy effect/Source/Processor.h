/*
  ==============================================================================

    Processor.h
    Created: 2 Apr 2020 2:50:31pm
    Author:  Iza

  ==============================================================================
*/

#pragma once
#include <cmath>
#define _USE_MATH_DEFINES
#include <JuceHeader.h>
#include "Slider.h"

class DistortionAudioProcessor : public AudioProcessor
{
public:
    DistortionAudioProcessor();
    ~DistortionAudioProcessor();

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(AudioSampleBuffer&, MidiBuffer&) override;

    void getStateInformation(MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const String getProgramName(int index) override;
    void changeProgramName(int index, const String& newName) override;

    StringArray distortionTypeItemsUI =
    {
        "Heavy",
        "Half-wave rectifier"
    };

    enum distortionTypeIndex 
    {
        distortionTypeHardClipping = 0,
        distortionTypeHalfWaveRectifier,
    };

    class Filter : public IIRFilter
    {
    public:
        void updateCoefficients(const double discreteFrequency,
            const double gain) noexcept
        {
            jassert(discreteFrequency > 0);

            double tan_half_wc = tan(discreteFrequency / 2.0);
            double sqrt_gain = sqrt(gain);

            coefficients = IIRCoefficients(
                sqrt_gain * tan_half_wc + gain,
                sqrt_gain * tan_half_wc - gain,
                0.0,
                sqrt_gain * tan_half_wc + 1.0,
                sqrt_gain * tan_half_wc - 1.0,
                0.0);

            setCoefficients(coefficients);
        }
    };

    OwnedArray<Filter> filters;
    void updateFilters();

    PluginParametersManager Parameters;
    PluginParameterComboBox paramDistortionType;
    PluginParameterLinSlider paramInputGain;
    PluginParameterLinSlider paramOutputGain;
    PluginParameterLinSlider paramTone;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DistortionAudioProcessor)
};