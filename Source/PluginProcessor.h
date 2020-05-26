/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include <JuceHeader.h>


//==============================================================================
/**
*/
using Parameter = AudioProcessorValueTreeState::Parameter;

//==============================================================================

class PluginParametersManager
{
public:
    PluginParametersManager(AudioProcessor& p) : valueTreeState(p, nullptr)
    {
    }

    AudioProcessorValueTreeState valueTreeState;
    StringArray parameterTypes;
    Array<StringArray> comboBoxItemLists;
};

//==============================================================================

class PluginParameter
    : public LinearSmoothedValue<float>
    , public AudioProcessorValueTreeState::Listener
{
protected:
    PluginParameter(PluginParametersManager& parametersManager,
        const std::function<float(float)> callback = nullptr)
        : parametersManager(parametersManager)
        , callback(callback)
    {
    }

public:
    void updateValue(float value)
    {
        if (callback != nullptr)
            setCurrentAndTargetValue(callback(value));
        else
            setCurrentAndTargetValue(value);
    }

    void parameterChanged(const String& parameterID, float newValue) override
    {
        updateValue(newValue);
    }

    PluginParametersManager& parametersManager;
    std::function<float(float)> callback;
    String paramID;
};

//==============================================================================

class PluginParameterSlider : public PluginParameter
{
protected:
    PluginParameterSlider(PluginParametersManager& parametersManager,
        const String& paramName,
        const String& labelText,
        const float minValue,
        const float maxValue,
        const float defaultValue,
        const std::function<float(float)> callback,
        const bool logarithmic)
        : PluginParameter(parametersManager, callback)
        , paramName(paramName)
        , labelText(labelText)
        , minValue(minValue)
        , maxValue(maxValue)
        , defaultValue(defaultValue)
    {
        paramID = paramName.removeCharacters(" ").toLowerCase();
        parametersManager.parameterTypes.add("Slider");

        NormalisableRange<float> range(minValue, maxValue);
        if (logarithmic)
            range.setSkewForCentre(sqrt(minValue * maxValue));

        parametersManager.valueTreeState.createAndAddParameter(std::make_unique<Parameter>
            (paramID, paramName, labelText, range, defaultValue,
                [](float value) { return String(value, 2); },
                [](const String& text) { return text.getFloatValue(); })
        );

        parametersManager.valueTreeState.addParameterListener(paramID, this);
        updateValue(defaultValue);
    }

public:
    const String& paramName;
    const String& labelText;
    const float minValue;
    const float maxValue;
    const float defaultValue;
};

//======================================

class PluginParameterLinSlider : public PluginParameterSlider
{
public:
    PluginParameterLinSlider(PluginParametersManager& parametersManager,
        const String& paramName,
        const String& labelText,
        const float minValue,
        const float maxValue,
        const float defaultValue,
        const std::function<float(float)> callback = nullptr)
        : PluginParameterSlider(parametersManager,
            paramName,
            labelText,
            minValue,
            maxValue,
            defaultValue,
            callback,
            false)
    {
    }
};

//======================================

class PluginParameterLogSlider : public PluginParameterSlider
{
public:
    PluginParameterLogSlider(PluginParametersManager& parametersManager,
        const String& paramName,
        const String& labelText,
        const float minValue,
        const float maxValue,
        const float defaultValue,
        const std::function<float(float)> callback = nullptr)
        : PluginParameterSlider(parametersManager,
            paramName,
            labelText,
            minValue,
            maxValue,
            defaultValue,
            callback,
            true)
    {
    }
};

//==============================================================================

class PluginParameterToggle : public PluginParameter
{
public:
    PluginParameterToggle(PluginParametersManager& parametersManager,
        const String& paramName,
        const bool defaultState = false,
        const std::function<float(float)> callback = nullptr)
        : PluginParameter(parametersManager, callback)
        , paramName(paramName)
        , defaultState(defaultState)
    {
        paramID = paramName.removeCharacters(" ").toLowerCase();
        parametersManager.parameterTypes.add("ToggleButton");

        const StringArray toggleStates = { "False", "True" };
        NormalisableRange<float> range(0.0f, 1.0f, 1.0f);

        parametersManager.valueTreeState.createAndAddParameter(std::make_unique<Parameter>
            (paramID, paramName, "", range, (float)defaultState,
                [toggleStates](float value) { return toggleStates[(int)value]; },
                [toggleStates](const String& text) { return toggleStates.indexOf(text); })
        );

        parametersManager.valueTreeState.addParameterListener(paramID, this);
        updateValue((float)defaultState);
    }

    const String& paramName;
    const bool defaultState;
};

//==============================================================================

class PluginParameterComboBox : public PluginParameter
{
public:
    PluginParameterComboBox(PluginParametersManager& parametersManager,
        const String& paramName,
        const StringArray items,
        const int defaultChoice = 0,
        const std::function<float(const float)> callback = nullptr)
        : PluginParameter(parametersManager, callback)
        , paramName(paramName)
        , items(items)
        , defaultChoice(defaultChoice)
    {
        paramID = paramName.removeCharacters(" ").toLowerCase();
        parametersManager.parameterTypes.add("ComboBox");

        parametersManager.comboBoxItemLists.add(items);
        NormalisableRange<float> range(0.0f, (float)items.size() - 1.0f, 1.0f);

        parametersManager.valueTreeState.createAndAddParameter(std::make_unique<Parameter>
            (paramID, paramName, "", range, (float)defaultChoice,
                [items](float value) { return items[(int)value]; },
                [items](const String& text) { return items.indexOf(text); })
        );

        parametersManager.valueTreeState.addParameterListener(paramID, this);
        updateValue((float)defaultChoice);
    }

    const String& paramName;
    const StringArray items;
    const int defaultChoice;
};

class ChorusAudioProcessor : public AudioProcessor
{
public:
    //==============================================================================

    ChorusAudioProcessor();
    ~ChorusAudioProcessor();

    //==============================================================================

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(AudioSampleBuffer&, MidiBuffer&) override;

    //==============================================================================






    //==============================================================================

    void getStateInformation(MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================

    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    //==============================================================================

    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const String getProgramName(int index) override;
    void changeProgramName(int index, const String& newName) override;

    //==============================================================================






    //==============================================================================

    StringArray waveformItemsUI = {
        "Sine",
        "Triangle",
        "Sawtooth (rising)",
        "Sawtooth (falling)"
    };

    enum waveformIndex {
        waveformSine = 0,
        waveformTriangle,
        waveformSawtooth,
        waveformInverseSawtooth,
    };

    //======================================

    StringArray interpolationItemsUI = {
        "None",
        "Linear",
        "Cubic"
    };

    enum interpolationIndex {
        interpolationNearestNeighbour = 0,
        interpolationLinear,
        interpolationCubic,
    };

    //======================================

    AudioSampleBuffer delayBuffer;
    int delayBufferSamples;
    int delayBufferChannels;
    int delayWritePosition;

    float lfoPhase;
    float inverseSampleRate;
    float twoPi;

    float lfo(float phase, int waveform);

    //======================================

    PluginParametersManager parameters;

    PluginParameterLinSlider paramDelay;
    PluginParameterLinSlider paramWidth;
    PluginParameterLinSlider paramDepth;
    PluginParameterComboBox paramNumVoices;
    PluginParameterLinSlider paramFrequency;
    PluginParameterComboBox paramWaveform;
    PluginParameterComboBox paramInterpolation;
    PluginParameterToggle paramStereo;

private:
    //==============================================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChorusAudioProcessor)
};