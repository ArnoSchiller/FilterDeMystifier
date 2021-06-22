/*
  ==============================================================================

    This file was auto-generated!
    It contains the basic framework code for a JUCE plugin processor.
    
    Authors:    Jannik Hartog (JH), Arno Schiller (AS), Joerg Bitzer (JB)

    SPDX-License-Identifier: BSD-3-Clause
  ==============================================================================
*/
#include <cassert>
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "PNParameter.h"

//==============================================================================
FilterDeMystifierAudioProcessor::FilterDeMystifierAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : m_limiter(), AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    // Add Parameters (direct or by calling the addParameter functions)
    // HIER PARAMETER EINFUEGEN, ZB:
    // m_rate = m_parameterVTS->getRawParameterValue(paramLFORate.ID[0]);
    // m_oldrate = *m_rate;

    m_PNparams.addParameter(m_paramVector);

    // Build VTS and PresetHandler
    m_paramVTS = std::make_unique<AudioProcessorValueTreeState>(*this,
        nullptr, Identifier("FilterDeMystefierVTS"),
        AudioProcessorValueTreeState::ParameterLayout(m_paramVector.begin(),
            m_paramVector.end()));
    m_paramVTS->state.setProperty("version", JucePlugin_VersionString,
        nullptr);

    m_presetHandler.setAudioValueTreeState(m_paramVTS.get());
    // if needed add categories
    m_presetHandler.addCategory(StringArray("Classic Cut", "Audio Filter", "Vocals", "Synth", "DSP Specialities"));
    // m_presets.addCategory(JadeSynthCategories);
#ifdef FACTORY_PRESETS    
    m_presetHandler.DeployFactoryPresets();
#endif
	m_presetHandler.loadfromFileAllUserPresets();    

    m_filter.resize(m_nrofinputchannels);
    for (unsigned int kk = 0; kk < m_nrofinputchannels ;++kk)
    {
        m_filter[kk].resize(m_nrofSOS); // we will have 4 SOS filter per channel
    }

}


FilterDeMystifierAudioProcessor::~FilterDeMystifierAudioProcessor()
{
}

//==============================================================================
const String FilterDeMystifierAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FilterDeMystifierAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FilterDeMystifierAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool FilterDeMystifierAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double FilterDeMystifierAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FilterDeMystifierAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FilterDeMystifierAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FilterDeMystifierAudioProcessor::setCurrentProgram (int index)
{
}

const String FilterDeMystifierAudioProcessor::getProgramName (int index)
{
    if (index == 0)
        return "Init";
    return {};
}


void FilterDeMystifierAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void FilterDeMystifierAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{

    int nrofchannels = this->getMainBusNumInputChannels();
    assert(("number of channels should never be zero", nrofchannels>0));

    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    m_data.resize(samplesPerBlock);
    m_limiter.prepareToPlay(sampleRate,nrofchannels);

    m_limiter.setReleaseTime(2000.f);
    m_meter.prepareToPlay(sampleRate,samplesPerBlock);

}

void FilterDeMystifierAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FilterDeMystifierAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif



}
#endif

void FilterDeMystifierAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    bool poleProtect = m_paramVTS->getParameter(paramPoleProtectBool.ID)->getValue();
    for (unsigned int sossec = 0 ; sossec < m_nrofSOS ; ++sossec )
    {
        float b0 = 1.0,b1 = 0.0,b2 = 0.0,a1 = 0.0,a2 = 0.0;
        // Poles first
        auto poleOn = m_paramVTS->getRawParameterValue(paramPoleBool.ID[sossec]);
        float isPoleOn = *poleOn;
        auto poleWithConj = m_paramVTS->getRawParameterValue(paramPoleConjugated.ID[sossec]);
        float isPoleConj = *poleWithConj;

        auto poleReal = m_paramVTS->getRawParameterValue(paramPoleReal.ID[sossec]);
        float realvalpole = *poleReal;
        auto poleImag = m_paramVTS->getRawParameterValue(paramPoleImag.ID[sossec]);
        float imagvalpole = *poleImag;

        if ((isPoleOn > 0.5) && (isPoleConj < 0.5)) // odd filter
        {
            a2 = 0.0;
            a1 = -realvalpole;
        }
        if ((isPoleOn > 0.5) && (isPoleConj > 0.5)) // even filter
        {
            a2 = realvalpole*realvalpole + imagvalpole*imagvalpole;
            a1 = -2*realvalpole;
        }


        // Zeros second
        auto zeroOn = m_paramVTS->getRawParameterValue(paramZeroBool.ID[sossec]);
        float isZeroOn = *zeroOn;
        auto zeroWithConj = m_paramVTS->getRawParameterValue(paramZeroConjugated.ID[sossec]);
        float isZeroConj = *zeroWithConj;

        auto zeroReal = m_paramVTS->getRawParameterValue(paramZeroReal.ID[sossec]);
        float realvalzero =  *zeroReal;
        auto zeroImag = m_paramVTS->getRawParameterValue(paramZeroImag.ID[sossec]);
        float imagvalzero =  *zeroImag;

        if ((isZeroOn > 0.5) && (isZeroConj < 0.5)) // odd filter
        {
            b2 = 0.0;
            b1 = -realvalzero;
        }
        if ((isZeroOn > 0.5) && (isZeroConj > 0.5)) // even filter
        {
            b2 = realvalzero*realvalzero + imagvalzero*imagvalzero;
            b1 = -2*realvalzero;
        }
        
        if (sossec == 0)
        {
            auto gain = m_paramVTS->getRawParameterValue(paramb0.ID);
            float gainValdB = *gain;
            float gainlin = pow(10.f,gainValdB/20.0);
            b0 *= gainlin;
            b1 *= gainlin;
            b2 *= gainlin; 
        }
        if ((poleProtect == true) && a2 < 1)
        {   
            if ((isPoleConj > 0.5))
            {
                m_filter[0][sossec].setCoeffs(b0,b1,b2,a1,a2);
                m_filter[1][sossec].setCoeffs(b0,b1,b2,a1,a2);
            }
            else
            { 
                if (abs(a1) < 0.998)
                {
                    m_filter[0][sossec].setCoeffs(b0,b1,b2,a1,a2);
                    m_filter[1][sossec].setCoeffs(b0,b1,b2,a1,a2);
                }
            }
        }
        else 
        {
            if  (poleProtect == false)
            {
                m_filter[0][sossec].setCoeffs(b0,b1,b2,a1,a2);
                m_filter[1][sossec].setCoeffs(b0,b1,b2,a1,a2);
            }
        }
    }
    bool bypassLimiter = m_paramVTS->getParameter(paramLimiterBool.ID)->getValue();
    m_limiter.setBypass(!bypassLimiter);
    ScopedLock Sp(objectLock);

    auto bypass = getBypassParameter();

    if (buffer.getNumChannels() == 0)
        return;

    if (getBusCount(false) < 1)
        return;

    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    
    m_data.resize(buffer.getNumSamples());

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // change to vector
        for (auto idx = 0u; idx < buffer.getNumSamples(); idx++)
        {
            m_data[idx] = channelData[idx];
        }
        for (auto kk = 0; kk < m_nrofSOS; kk++)
        {
            m_filter[channel][kk].processDataTV(m_data,m_data);
        }
        for (auto idx = 0u; idx < buffer.getNumSamples(); idx++)
        {
            channelData[idx] = m_data[idx];
        }
    }

    m_limiter.processSamples(buffer);
    m_meter.analyseData(buffer);
}

//==============================================================================
bool FilterDeMystifierAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* FilterDeMystifierAudioProcessor::createEditor()
{
    return new FilterDeMystifierAudioProcessorEditor (*this, *m_paramVTS,
            m_presetHandler);
    }
    

//==============================================================================
void FilterDeMystifierAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = m_paramVTS->copyState();
    std::unique_ptr<XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void FilterDeMystifierAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data,
        sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(m_paramVTS->state.getType()))
            m_paramVTS->replaceState(ValueTree::fromXml(*xmlState));

}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FilterDeMystifierAudioProcessor();
}
