/*
  ==============================================================================

    Processor.cpp
    Created: 2 Apr 2020 3:09:39pm
    Author:  Iza

  ==============================================================================
*/

#include "Processor.h"
#include "Distortion.h"
#include "Slider.h"
#include "../JuceLibraryCode/JuceHeader.h"

DistortionAudioProcessor::DistortionAudioProcessor() :
    AudioProcessor(BusesProperties()
        //wejscie typu stereo
        .withInput("Input", AudioChannelSet::stereo(), true)
        //wyjscie typu mono
        .withOutput("Output", AudioChannelSet::stereo(), true)
        ),
    Parameters(*this),
    //lista wyboru
    paramDistortionType(Parameters, "Distortion type", distortionTypeItemsUI),
    //pierwszy slider, ktorym mozemy ustawiac gain na wejsciu
    //zakres od -24 do 24 dB, a 12 dB jest ustawione automatycznie przy uruchomieniu programu
    paramInputGain(Parameters, "Input gain", "dB", -24.0f, 24.0f, 12.0f,
        [](float value) { return powf(10.0f, value * 0.05f); }),
    //drugi slider, ktorym mozemy ustawiac gain na wyjsciu
    //zakres od -24 do 24 dB, a -24 dB jest ustawione automatycznie przy uruchomieniu programu
    paramOutputGain(Parameters, "Output gain", "dB", -24.0f, 24.0f, -24.0f,
        [](float value) { return powf(10.0f, value * 0.05f); }),
    //trzeci slider, ktorym mozemy ustawiac tone
    //zakres od -24 do 24 dB, a 12 dB jest ustawione automatycznie przy uruchomieniu programu
    paramTone(Parameters, "Tone", "dB", -24.0f, 24.0f, 12.0f,
        [this](float value)
        {
            paramTone.setCurrentAndTargetValue(value); updateFilters(); return value;
        })
{
    Parameters.valueTreeState.state = ValueTree(Identifier(getName().removeCharacters("- ")));
}

        DistortionAudioProcessor::~DistortionAudioProcessor()
        {
        }

        void DistortionAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
        {
            const double smoothTime = 1e-3;
            paramDistortionType.reset(sampleRate, smoothTime);
            paramInputGain.reset(sampleRate, smoothTime);
            paramOutputGain.reset(sampleRate, smoothTime);
            paramTone.reset(sampleRate, smoothTime);

            filters.clear();
            for (int i = 0; i < getTotalNumInputChannels(); ++i)
            {
                Filter* filter;
                filters.add(filter = new Filter());
            }
            updateFilters();
        }

        void DistortionAudioProcessor::releaseResources()
        {
        }

        void DistortionAudioProcessor::processBlock(AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
        {
            ScopedNoDenormals noDenormals;

            const int numInputChannels = getTotalNumInputChannels();
            const int numOutputChannels = getTotalNumOutputChannels();
            const int numSamples = buffer.getNumSamples();

            for (int channel = 0; channel < numInputChannels; ++channel)
            {
                float* channelData = buffer.getWritePointer(channel);

                float out;
                for (int sample = 0; sample < numSamples; ++sample)
                {
                    const float in = channelData[sample] * paramInputGain.getNextValue();

                    switch ((int)paramDistortionType.getTargetValue())
                    {
                    //przypisanie nowej wartosci po przekroczeniu danego progu
                    case distortionTypeHardClipping:
                    {
                        float threshold = 0.5f;
                        if (in > threshold)
                            out = threshold;
                        else if (in < -threshold)
                            out = -threshold;
                        else
                            out = in;
                        break;
                    }

                    //obciecie sygnalu ktory znajduje sie ponizej 0
                    case distortionTypeHalfWaveRectifier:
                    {
                        if (in > 0.0f)
                            out = in;
                        else
                            out = 0.0f;
                        break;
                    }
                    }

                    float filtered = filters[channel]->processSingleSampleRaw(out);
                    channelData[sample] = filtered * paramOutputGain.getNextValue();
                }
            }
            for (int channel = numInputChannels; channel < numOutputChannels; ++channel)
                buffer.clear(channel, 0, numSamples);
        }

        void DistortionAudioProcessor::updateFilters()
        {
            double discreteFrequency = M_PI * 0.01;
            double gain = pow(10.0, (double)paramTone.getTargetValue() * 0.05);

            for (int i = 0; i < filters.size(); ++i)
                filters[i]->updateCoefficients(discreteFrequency, gain);
        }

        void DistortionAudioProcessor::getStateInformation(MemoryBlock& destData)
        {
            auto state = Parameters.valueTreeState.copyState();
            std::unique_ptr<XmlElement> xml(state.createXml());
            copyXmlToBinary(*xml, destData);
        }

        void DistortionAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
        {
            std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

            if (xmlState.get() != nullptr)
                if (xmlState->hasTagName(Parameters.valueTreeState.state.getType()))
                    Parameters.valueTreeState.replaceState(ValueTree::fromXml(*xmlState));
        }

        AudioProcessorEditor* DistortionAudioProcessor::createEditor()
        {
            return new DistortionAudioProcessorEditor(*this);
        }

        bool DistortionAudioProcessor::hasEditor() const
        {
            return true; 
        }

        bool DistortionAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
        {
            ignoreUnused(layouts);
            return true;
            if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
                && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
                return false;

            if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
                return false;

            return true;
        }

        const String DistortionAudioProcessor::getName() const
        {
            return {};
        }

        bool DistortionAudioProcessor::acceptsMidi() const
        {
            return true;
        }

        bool DistortionAudioProcessor::producesMidi() const
        {
            return true;
        }

        bool DistortionAudioProcessor::isMidiEffect() const
        {
            return true;
        }

        double DistortionAudioProcessor::getTailLengthSeconds() const
        {
            return 0.0;
        }

        int DistortionAudioProcessor::getNumPrograms()
        {
            return 1;
        }

        int DistortionAudioProcessor::getCurrentProgram()
        {
            return 0;
        }

        void DistortionAudioProcessor::setCurrentProgram(int index)
        {
        }

        const String DistortionAudioProcessor::getProgramName(int index)
        {
            return {};
        }

        void DistortionAudioProcessor::changeProgramName(int index, const String& newName)
        {
        }

        AudioProcessor* JUCE_CALLTYPE createPluginFilter()
        {
            return new DistortionAudioProcessor();
        }