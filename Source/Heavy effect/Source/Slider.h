#pragma once
#include <JuceHeader.h>
using Parameter = AudioProcessorValueTreeState::Parameter;

class PluginParametersManager
{
public:
    PluginParametersManager(AudioProcessor& p) : valueTreeState(p, nullptr)
    {
    }
    AudioProcessorValueTreeState valueTreeState;
    StringArray Types;
    Array<StringArray> boxLists;
};

class PluginParameter: 
    public LinearSmoothedValue<float>, 
    public AudioProcessorValueTreeState::Listener
{
protected:
    PluginParameter(PluginParametersManager& parametersManager,
        const std::function<float(float)> callback = nullptr) :
        parametersManager(parametersManager),
        callback(callback)
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

class PluginParameterSlider : public PluginParameter
{
protected:
    PluginParameterSlider(PluginParametersManager& parametersManager,
        const String& parameterName,
        const String& labelText,
        const float minValue,
        const float maxValue,
        const float defaultValue,
        const std::function<float(float)> callback,
        const bool logarithmic) :
        PluginParameter(parametersManager, callback),
        parameterName(parameterName),
        labelText(labelText),
        minValue(minValue),
        maxValue(maxValue),
        defaultValue(defaultValue)
    {
        paramID = parameterName.removeCharacters(" ").toLowerCase();
        parametersManager.Types.add("Slider");

        NormalisableRange<float> range(minValue, maxValue);
        if (logarithmic)
            range.setSkewForCentre(sqrt(minValue * maxValue));

        parametersManager.valueTreeState.createAndAddParameter(std::make_unique<Parameter>
        (paramID, parameterName, labelText, range, defaultValue,
            [](float value)
            {
                return String(value, 2);
            },
            [](const String& text)
            {
                return text.getFloatValue();
            }
                )
            );
        parametersManager.valueTreeState.addParameterListener(paramID, this);
        updateValue(defaultValue);
    }
public:
    const String& parameterName;
    const String& labelText;
    const float minValue;
    const float maxValue;
    const float defaultValue;
};

class PluginParameterLinSlider : public PluginParameterSlider
{
public:
    PluginParameterLinSlider(PluginParametersManager& parametersManager,
        const String& parameterName,
        const String& labelText,
        const float minValue,
        const float maxValue,
        const float defaultValue,
        const std::function<float(float)> callback = nullptr) :
        PluginParameterSlider(parametersManager,
            parameterName,
            labelText,
            minValue,
            maxValue,
            defaultValue,
            callback,
            false)
    {
    }
};

class PluginParameterLogSlider : public PluginParameterSlider
{
public:
    PluginParameterLogSlider(PluginParametersManager& parametersManager,
        const String& parameterName,
        const String& labelText,
        const float minValue,
        const float maxValue,
        const float defaultValue,
        const std::function<float(float)> callback = nullptr) :
        PluginParameterSlider(parametersManager,
            parameterName,
            labelText,
            minValue,
            maxValue,
            defaultValue,
            callback,
            true)
    {
    }
};

class PluginParameterToggle : public PluginParameter
{
public:
    PluginParameterToggle(PluginParametersManager& parametersManager,
        const String& parameterName,
        const bool defaultState = false,
        const std::function<float(float)> callback = nullptr) :
        PluginParameter(parametersManager, callback),
        parameterName(parameterName),
        defaultState(defaultState)
    {
        paramID = parameterName.removeCharacters(" ").toLowerCase();
        parametersManager.Types.add("ToggleButton");

        const StringArray toggleStates =
        {
            "False", "True"
        };
        NormalisableRange<float> range(0.0f, 1.0f, 1.0f);

        parametersManager.valueTreeState.createAndAddParameter
        (std::make_unique<Parameter>
        (paramID, parameterName, "", range, (float)defaultState,
            [toggleStates](float value)
            {
                return toggleStates[(int)value];
            },
            [toggleStates](const String& text)
            {
                return toggleStates.indexOf(text);
            }
            )
            );

        parametersManager.valueTreeState.addParameterListener(paramID, this);
        updateValue((float)defaultState);
    }
    const String& parameterName;
    const bool defaultState;
};

class PluginParameterComboBox : public PluginParameter
{
public:
    PluginParameterComboBox(PluginParametersManager& parametersManager,
        const String& parameterName,
        const StringArray items,
        const int defaultChoice = 0,
        const std::function<float(const float)> callback = nullptr) :
        PluginParameter(parametersManager, callback),
        parameterName(parameterName),
        items(items),
        defaultChoice(defaultChoice)
    {
        paramID = parameterName.removeCharacters(" ").toLowerCase();
        parametersManager.Types.add("ComboBox");

        parametersManager.boxLists.add(items);
        NormalisableRange<float> range(0.0f, (float)items.size() - 1.0f, 1.0f);

        parametersManager.valueTreeState.createAndAddParameter(std::make_unique<Parameter>
        (paramID, parameterName, "", range, (float)defaultChoice,
            [items](float value)
            {
                return items[(int)value];
            },
            [items](const String& text)
            {
                return items.indexOf(text);
            }
            )
            );
        parametersManager.valueTreeState.addParameterListener(paramID, this);
        updateValue((float)defaultChoice);
    }

    const String& parameterName;
    const StringArray items;
    const int defaultChoice;
};
