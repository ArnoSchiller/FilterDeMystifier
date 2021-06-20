/*
  ==============================================================================

    This file was auto-generated!
    It contains the basic framework code for a JUCE plugin editor.
    
    Authors:    Jannik Hartog (JH), Arno Schiller (AS)

    SPDX-License-Identifier: BSD-3-Clause
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "MainComponent.h"
#include "TransferFunLookAndFeel.h"
#include "SimpleMeter.h"


//==============================================================================
/**
*/
class FilterDeMystifierAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    FilterDeMystifierAudioProcessorEditor (FilterDeMystifierAudioProcessor&,
        AudioProcessorValueTreeState&, PresetHandler&);
    ~FilterDeMystifierAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.

    juce::OpenGLContext m_context;

    TransferFunLookAndFeel m_lookAndFeel;

    AudioProcessorValueTreeState& m_vts;
    MainComponent m_mainGUI;

    FilterDeMystifierAudioProcessor& m_processor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterDeMystifierAudioProcessorEditor)
};
