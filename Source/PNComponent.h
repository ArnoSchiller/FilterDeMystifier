/*
  ==============================================================================
    PNComponent.h

    This class contains the pole zero plot and the control elements. 

    Authors:    Jannik Hartog (JH), Arno Schiller (AS)
    Version:    v0.0.2

    SPDX-License-Identifier: BSD-3-Clause

    Version history:
    Version:   (Author) Description:								Date:     \n
    v0.0.1     (AS) First initialize. Outsourced code from          21.07.2020\n
                    PNplot.                                                   \n
    v0.0.2     (AS) Added function to update slider values of       23.07.2020\n
                    selected pole/zero when it is dragged in plot.            \n
 ===============================================================================
*/


#pragma once
#define PLOT_HEIGHT_REL 0.5

#include <JuceHeader.h>
#include "PNplot.h"
#include "PNcontrolComponent.h"
#include "PluginProcessor.h"
#include "ProtectionComponent.h"

//==============================================================================
/*
*/
class PNComponent    : public Component
{
public:
    PNComponent(AudioProcessorValueTreeState& vts, FilterDeMystifierAudioProcessor& p)
        : m_processor(p), m_vts(vts), m_pnPlot(vts), m_meter(p.m_meter), m_protectionGUI(vts)

    {
        // add items to the combo-box
        addAndMakeVisible(m_filterOrderLabel);
        addAndMakeVisible (m_filterOrderMenu);

        for(int i = 1; i <= m_maxFilterOrder; i++)
        {
            m_filterOrderMenu.addItem (String(i), i);
        }
        m_filterOrderMenu.onChange = [this] { filterOrderMenuChanged(); };
        m_filterOrderMenu.setSelectedId (2);
        
        for (auto kk = 0; kk < m_numOfPole; ++kk)
        {
            m_poleControls.add(std::make_unique<PNcontrolComponent>(m_vts, kk,
                PNcontrolComponent::PNType::pole));
            m_poleControls[kk]->somethingChanged = [this]() {if (somethingChanged != nullptr) somethingChanged();};
            addAndMakeVisible(m_poleControls[kk]);
        }
        for (auto kk = 0; kk < m_numOfZero; ++kk)
        {
            m_zeroControls.add(std::make_unique<PNcontrolComponent>(m_vts, kk,
                PNcontrolComponent::PNType::zero));
            m_zeroControls[kk]->somethingChanged = [this]() {if (somethingChanged != nullptr) somethingChanged();};
            addAndMakeVisible(m_zeroControls[kk]);
        }

        m_pnPlot.somethingChanged = [this]() {if (somethingChanged != nullptr) somethingChanged();};
        m_pnPlot.updateSlider = [this]() {updateSliderValue();};
        addAndMakeVisible(m_pnPlot);

        addAndMakeVisible(m_meter);

        addAndMakeVisible(m_protectionGUI);
        
        m_b0Slider.setSliderStyle(Slider::SliderStyle::LinearBar);
        m_b0Slider.setColour(m_b0Slider.textBoxOutlineColourId, JadeGray);
        m_b0Slider.onValueChange = [this]() {if (somethingChanged != nullptr) somethingChanged();};
        m_b0Attachment = std::make_unique<SliderAttachment>(m_vts, paramb0.ID, m_b0Slider);
        addAndMakeVisible(m_b0Slider);
        m_b0Label.setText("g =", NotificationType::dontSendNotification);
        addAndMakeVisible(m_b0Label);
    }

    ~PNComponent()
    {
    }

    void paint (Graphics& g) override
    {
        g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   

        g.setColour(BorderColour);
        g.drawRect(getLocalBounds(), LINEWIDH_MEDIUM);
    }

    #define POLE_ZERO_CTRL_SPACE 10
    void resized() override
    {
        auto bounds = getLocalBounds();

        auto protectionBounds = bounds.removeFromTop(50);
        m_protectionGUI.setBounds(protectionBounds);

        auto pnPlotBounds = bounds.removeFromTop(bounds.getHeight() * PLOT_HEIGHT_REL);
        auto meterBounds = pnPlotBounds.removeFromRight(0.15*pnPlotBounds.getWidth()).reduced(10);
        m_meter.setBounds(meterBounds);
        m_pnPlot.setBounds(pnPlotBounds);

        auto configBounds = bounds.removeFromTop(bounds.getHeight()* 0.1);
        configBounds.reduce(7, 0);
        m_filterOrderLabel.setBounds(configBounds.removeFromLeft(100));
        m_filterOrderMenu.setBounds(configBounds.removeFromLeft(60));
        configBounds.removeFromLeft(10);
        m_b0Label.setBounds(configBounds.removeFromLeft(40));
        m_b0Slider.setBounds(configBounds);
        
        auto controlBounds = bounds;
        controlBounds.reduce(PADDING, PADDING / 2);
        int controlHeight = (controlBounds.getHeight() - 2 * POLE_ZERO_CTRL_SPACE) / (m_numOfZero + m_numOfPole);
        if (controlHeight > 40) controlHeight = 40;
        for (auto kk = 0U; kk < m_poleControls.size(); ++kk)
            m_poleControls[kk]->setBounds(controlBounds.removeFromTop(controlHeight));
        controlBounds.removeFromTop(POLE_ZERO_CTRL_SPACE);
        for (auto kk = 0U; kk < m_zeroControls.size(); ++kk)
            m_zeroControls[kk]->setBounds(controlBounds.removeFromTop(controlHeight));

    }

    void filterOrderMenuChanged()
    /**
     * @brief Callback function if filterOrderMenu changed. Calls updateFilterOrder.
     */
    {
        updateFilterOrder(m_filterOrderMenu.getSelectedId());
    }

    void updateFilterOrder(int newOrder)
    /**
     * @brief Function will change the visibility of the control widgets and deactivates the poles and zeros not displayed. 
     */
    {
        m_filterOrder = newOrder;

        for (auto kk = 0; kk < m_numOfPole; ++kk)
        {
            if(2 * kk < m_filterOrder){
                m_poleControls[kk]->setAcivation(true);
                m_poleControls[kk]->setVisible(true);
            }else{
                m_poleControls[kk]->setAcivation(false);
                m_poleControls[kk]->setVisible(false);
            }

            if(2 * kk+1 < m_filterOrder){
                m_poleControls[kk]->setConjugation(true);
            } else {
                m_poleControls[kk]->setConjugation(false);
            }
        }
        for (auto kk = 0; kk < m_numOfZero; ++kk)
        {
            if(2 * kk < m_filterOrder){
                m_zeroControls[kk]->setAcivation(true);
                m_zeroControls[kk]->setVisible(true);
            }else{
                m_zeroControls[kk]->setAcivation(false);
                m_zeroControls[kk]->setVisible(false);
            }
            if(2 * kk+1 < m_filterOrder){
                m_zeroControls[kk]->setConjugation(true);
            } else {
                m_zeroControls[kk]->setConjugation(false);
            }
        }


    }

    void updatePlot() 
    {
        for (auto kk = 0; kk < m_numOfPole; ++kk)
        {
            m_poleControls[kk]->setAcivation(m_vts.getParameter(paramPoleBool.ID[kk])->getValue());
        }
        for (auto kk = 0; kk < m_numOfZero; ++kk)
        {
            m_zeroControls[kk]->setAcivation(m_vts.getParameter(paramZeroBool.ID[kk])->getValue());
        }
    }

    std::function<void()> somethingChanged;

private:

    AudioProcessorValueTreeState& m_vts;
    
    FilterDeMystifierAudioProcessor& m_processor;
    SimpleMeterComponent m_meter;

    PNParameter m_PNparam;
    int m_numOfPole = m_PNparam.maxNrOfPole;
    int m_numOfZero = m_PNparam.maxNrOfZero;

    //// Configuration 
    // filter order
    int m_filterOrder = 0;
    int m_maxFilterOrder = 2 * std::min(m_numOfPole,m_numOfZero);

    // b0
    Slider m_b0Slider;
    std::unique_ptr<SliderAttachment> m_b0Attachment;
    Label m_b0Label;

    // Pole Zero Plot
    PNplot m_pnPlot;

    // Protection control
    ProtectionComponent m_protectionGUI;

    // Filter configuration
    juce::Font m_textFont   { 12.0f };
    juce::Label m_filterOrderLabel { {}, "Filter Order:" };
    juce::ComboBox m_filterOrderMenu;

    // Pole Zero control widgets
    OwnedArray<PNcontrolComponent> m_poleControls;
    OwnedArray<PNcontrolComponent> m_zeroControls;


    void updateSliderValue()
    {
        std::complex<float> newValue = m_pnPlot.getNewObjectValue();
        std::unordered_map<String, int> object = m_pnPlot.getSelectedObject();

        if (object["p"] != -1)
            m_poleControls[object["p"]]->setSliderValues(newValue);
        if (object["z"] != -1)
            m_zeroControls[object["z"]]->setSliderValues(newValue);
    };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PNComponent)
};
