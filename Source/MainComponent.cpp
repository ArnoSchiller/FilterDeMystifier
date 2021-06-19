/*
  ==============================================================================
    MainComponent.cpp
    
    Authors:    Jannik Hartog (JH), Arno Schiller (AS)
        
    SPDX-License-Identifier: BSD-3-Clause
 ==============================================================================
*/
#include "MainComponent.h"

#define GUI_ELEMENT_MIN_DISTANCE 5
#define PRESETHANDLER_HEIGHT 30
//==============================================================================
MainComponent::MainComponent(AudioProcessorValueTreeState& vts, PresetHandler& ph, TransferFunAudioPlugInAudioProcessor& p)
    :m_vts(vts), m_processor(p), m_pnComponent(m_vts, p), m_presetGUI(ph), m_3DComponent(m_vts), 
    m_bodeComponent(m_vts, BodeDiagramComponent::LayoutTypes::horizontal)
{
    setSize(m_minWidth, m_minHeight);
    ScopedLock sp();
    m_pnComponent.somethingChanged = [this]() { updateGUI(); };
    //m_presetGUI.somethingChanged = [this]() { updateGUI(); m_presetGUI.resetSomethingChanged();};

    setSamplingRate(48000.0f);
    addAndMakeVisible(m_presetGUI);
    addAndMakeVisible(m_pnComponent);
    addAndMakeVisible(m_3DComponent);
    addAndMakeVisible(m_bodeComponent);

}

MainComponent::~MainComponent()
{
    // This shuts down the GL system and stops the rendering calls.
}

//==============================================================================
void MainComponent::paint(Graphics& g)
{
    // You can add your component specific drawing code here!
    // This will draw over the top of the openGL background.

    auto bounds = m_b0Label.getBounds();
    bounds.setWidth(m_b0Label.getBounds().getWidth() + m_b0Slider.getBounds().getWidth());
    g.setColour(getLookAndFeel().findColour(ResizableWindow::backgroundColourId).darker(0.2));
    g.fillRect(bounds);

    g.setColour(BorderColour);
    g.drawRect(bounds);
}

void MainComponent::resized()
{
    auto bounds = getBounds();
    if (bounds.getWidth() < m_minWidth)
        bounds.setWidth(m_minWidth);
    if (bounds.getHeight() < m_minHeight)
        bounds.setHeight(m_minHeight);
    setBounds(bounds);
    auto topBounds = bounds.removeFromTop(PRESETHANDLER_HEIGHT);
    m_presetGUI.setBounds(topBounds);
    m_pnComponent.setBounds(bounds.removeFromLeft(0.4 * bounds.getWidth()));
    m_bodeComponent.setBounds(bounds.removeFromBottom(0.3 * bounds.getHeight()));
    m_3DComponent.setBounds(bounds);
}

void MainComponent::updateGUI()
{
    m_presetGUI.setSomethingChanged(); 
    m_bodeComponent.updatePlot();
    m_pnComponent.updatePlot();
    m_3DComponent.setSomethingChanged();
    repaint();
}
