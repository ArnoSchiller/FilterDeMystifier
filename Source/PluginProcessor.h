/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

    Authors:    Jannik Hartog (JH), Arno Schiller (AS), Joerg Bitzer (JB)

    SPDX-License-Identifier: BSD-3-Clause
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PresetHandler.h"
#include "PNParameter.h"

#include "SOSFilter.h"
#include "BrickwallLimiter.h"
#include "SimpleMeter.h"

//==============================================================================
/**
*/
class FilterDeMystifierAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    FilterDeMystifierAudioProcessor();
    ~FilterDeMystifierAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    SimpleMeter m_meter;
private:
    // PARAMETER HANDLING 
    std::unique_ptr<AudioProcessorValueTreeState> m_paramVTS;
    std::vector<std::unique_ptr<RangedAudioParameter>> m_paramVector;
    PresetHandler m_presetHandler;
    CriticalSection objectLock;

    PNParameter m_PNparams;

    std::vector<std::vector<SOSFilter<float> > > m_filter;
    const int m_nrofinputchannels = 8;
    const int m_nrofSOS = 4;
    std::vector<float> m_data;

    std::atomic<float>* m_poleOn;
    std::atomic<float>* m_zeroOn;

    BrickwallLimiter<float> m_limiter;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterDeMystifierAudioProcessor)
};
