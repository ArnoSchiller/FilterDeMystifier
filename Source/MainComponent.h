/*
  ==============================================================================
    MainComponent.h

    This component contains the three representations of the transfer function. 
    On the left side is the pole-zero diagram in the Z-plane, in the middle is 
    the 3D plot of the transfer function in the Z-plane and on the right side is
    the plot of the Bodediagram by magnitude and phase. 
    Also there is a slider to change the b0 value.

    Authors:    Jannik Hartog (JH), Arno Schiller (AS)
    Version:    v0.0.3
    
    SPDX-License-Identifier: BSD-3-Clause

    Version history:
    Version:   (Author) Description:								Date:     \n
    v0.0.1     (AS) First initialize. Added the pole zero plot.     12.06.2020\n
    v0.0.2     (AS) Added Bode diagram.                             19.07.2020\n
    v0.0.3     (AS) Added slider for b0 coefficent.                 23.07.2020\n
 ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "AboutBoxComponent.h"
#include "BodeDiagramComponent.h"
#include "PresetHandler.h" 
#include "PNComponent.h"
#include "TransferFun3DComponent.h"
#include "PluginProcessor.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public Component
{
public:
    //==========================================================================
    MainComponent(AudioProcessorValueTreeState&, PresetHandler&, FilterDeMystifierAudioProcessor&);
    ~MainComponent();

    //==========================================================================
    void paint (Graphics& g) override;
    void resized() override;
    
    //==========================================================================
    void setSamplingRate(float fs) { m_bodeComponent.setSamplingRate(fs); }

private:
    //==========================================================================
    
    AboutBoxComponent m_aboutBox;

    FilterDeMystifierAudioProcessor& m_processor;
    AudioProcessorValueTreeState& m_vts;
    PresetComponent m_presetGUI;
    
    PNComponent m_pnComponent;
    TransferFun3DComponent m_3DComponent;
    BodeDiagramComponent m_bodeComponent;

    Slider m_b0Slider;
    std::unique_ptr<SliderAttachment> m_b0Attachment;

    Label m_b0Label;

    float m_minHeight = 400;
    float m_minWidth = m_minHeight * 16 / 9;

    void updateGUI();
    
    void activateAboutBox();
    void deactivateAboutBox();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
