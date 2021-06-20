/*
  ==============================================================================

    This file was auto-generated!
    It contains the basic framework code for a JUCE plugin editor.

    Authors:    Jannik Hartog (JH), Arno Schiller (AS)

    SPDX-License-Identifier: BSD-3-Clause
  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "PluginGUISettings.h"


//==============================================================================
FilterDeMystifierAudioProcessorEditor::FilterDeMystifierAudioProcessorEditor (FilterDeMystifierAudioProcessor& p, 
    AudioProcessorValueTreeState& vts, PresetHandler& ph)
    : AudioProcessorEditor(&p), m_processor(p), m_vts(vts), m_mainGUI(vts, ph, p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setResizeLimits (g_minGuiSize_x,g_minGuiSize_x*g_guiratio , g_maxGuiSize_x, g_maxGuiSize_x*g_guiratio);
    setResizable(true,true);
    getConstrainer()->setFixedAspectRatio(1./g_guiratio);
    setSize (g_minGuiSize_x, g_minGuiSize_x*g_guiratio);

    addAndMakeVisible(m_mainGUI);

}

FilterDeMystifierAudioProcessorEditor::~FilterDeMystifierAudioProcessorEditor()
{
}

//==============================================================================
void FilterDeMystifierAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

void FilterDeMystifierAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    m_mainGUI.setBounds(getLocalBounds());
    
}
