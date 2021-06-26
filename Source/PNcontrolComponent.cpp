/*
  ==============================================================================

    PNcontrolComponent.cpp
    Created: 13 Jul 2020 6:22:43pm

    Authors:    Jannik Hartog (JH), Arno Schiller (AS)

    SPDX-License-Identifier: BSD-3-Clause
  ==============================================================================
*/

#include <JuceHeader.h>
#include "PNcontrolComponent.h"
#include "PNParameter.h"

#define CTRL_PADDING 8
#define LABEL_WIDTH 30

PNcontrolComponent::PNcontrolComponent(AudioProcessorValueTreeState& vts,
    int index, PNcontrolComponent::PNType type)
    : m_vts(vts), m_index(index), m_type(type)
{
    if (m_type == PNcontrolComponent::PNType::pole)
    {
        m_isConjugated = m_vts.getParameter(paramPoleConjugated.ID[m_index])->getValue();
    }
    if (m_type == PNcontrolComponent::PNType::zero)
    {
        m_isConjugated = m_vts.getParameter(paramZeroConjugated.ID[m_index])->getValue();
    }

    updateName();

    m_PNLabel.setJustificationType(Justification::centredLeft);
    addAndMakeVisible(m_PNLabel);

    m_PNbutton.onClick = [this]() {statusButtonClicked(); };
    addAndMakeVisible(m_PNbutton);
    if (m_type == PNcontrolComponent::PNType::pole)
    {
        m_isActivated = m_vts.getParameter(paramPoleBool.ID[m_index])->getValue();
    }
    if (m_type == PNcontrolComponent::PNType::zero)
    {
        m_isActivated = m_vts.getParameter(paramZeroBool.ID[m_index])->getValue();
    }

    m_PNimagSlider.setSliderStyle(Slider::SliderStyle::LinearBar);
    m_PNrealSlider.setSliderStyle(Slider::SliderStyle::LinearBar);

    if(m_type == PNcontrolComponent::PNType::pole)
    {
        m_PNimagAttachment = std::make_unique<SliderAttachment>(m_vts, paramPoleImag.ID[m_index], m_PNimagSlider);
        m_PNrealAttachment = std::make_unique<SliderAttachment>(m_vts, paramPoleReal.ID[m_index], m_PNrealSlider);
    }
    if(m_type == PNcontrolComponent::PNType::zero)
    {
        m_PNimagAttachment = std::make_unique<SliderAttachment>(m_vts, paramZeroImag.ID[m_index], m_PNimagSlider);
        m_PNrealAttachment = std::make_unique<SliderAttachment>(m_vts, paramZeroReal.ID[m_index], m_PNrealSlider);
    }

    m_PNimagSlider.onValueChange = [this]() 
    {
        if (m_vts.getParameter(paramPoleProtectBool.ID)->getValue() && m_type == PNType::pole) // radius protection 
        {
            double real = m_PNrealSlider.getValue();
            double imag = m_PNimagSlider.getValue();
            double radius = sqrt(pow(real,2) + pow(imag,2));  

            if (radius > 0.99)
                {m_PNimagSlider.setValue(m_prev_imag); return;}
        }
        m_prev_imag = m_PNimagSlider.getValue();
        if (m_type == PNcontrolComponent::PNType::pole)
        {
            m_isConjugated = m_vts.getParameter(paramPoleConjugated.ID[m_index])->getValue();
        }
        if (m_type == PNcontrolComponent::PNType::zero)
        {
            m_isConjugated = m_vts.getParameter(paramZeroConjugated.ID[m_index])->getValue();
        }

        if (!m_isConjugated)
        {
            m_PNimagSlider.setValue(0.0); 
            return;
        }

        if (somethingChanged != nullptr) somethingChanged();
    };
    m_PNrealSlider.onValueChange = [this]() 
    {
        if (m_vts.getParameter(paramPoleProtectBool.ID)->getValue() && m_type == PNType::pole) // radius protection 
        {
            double real = m_PNrealSlider.getValue();
            double imag = m_PNimagSlider.getValue();
            double radius = sqrt(pow(real,2) + pow(imag,2));  

            if (radius > 0.998)
                {m_PNrealSlider.setValue(m_prev_real); return;}
        }
        m_prev_real = m_PNrealSlider.getValue();
        
        if (somethingChanged != nullptr) somethingChanged();
    };

    addAndMakeVisible(m_PNrealSlider);
    addAndMakeVisible(m_PNimagSlider);


    m_realLabel.setText("Re:", NotificationType::dontSendNotification);
    m_imagLabel.setText("Im:", NotificationType::dontSendNotification);    
    addAndMakeVisible(m_realLabel);
    addAndMakeVisible(m_imagLabel);
}

PNcontrolComponent::~PNcontrolComponent()
{
}

void PNcontrolComponent::paint(Graphics& g)
{
    m_isConjugated = m_vts.getParameter(paramPoleConjugated.ID[m_index])->getValue();
    updateName();
    
    auto bounds = getLocalBounds();
    bounds.reduced(CTRL_PADDING);

    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId).darker(0.2));
    g.drawRect(bounds, 1.0f);
   

    if (!m_isActivated)
    {
        m_PNbutton.setButtonText("O");
        m_PNbutton.setColour(TextButton::ColourIds::buttonColourId, Colours::darkred);
        m_PNimagSlider.setVisible(false);
        m_PNrealSlider.setVisible(false);
        g.drawLine(bounds.getX(), bounds.getY(),
            bounds.getX() + bounds.getWidth(), bounds.getY() + bounds.getHeight());
        g.drawLine(bounds.getX(), bounds.getY() + bounds.getHeight(),
            bounds.getX() + bounds.getWidth(), bounds.getY());
    }
    else
    {
        m_PNbutton.setButtonText("X");
        m_PNbutton.setColour(TextButton::ColourIds::buttonColourId, Colours::darkgreen);
        m_PNimagSlider.setVisible(true);
        m_PNrealSlider.setVisible(true);
    }
}

void PNcontrolComponent::resized()
{
    auto bounds = getLocalBounds();
    bounds.reduce(5, 3);
    int labelWidth = bounds.getWidth()/5;
    int sliderWidth = bounds.getWidth()/3;
    m_PNLabel.setBounds(bounds.removeFromLeft(labelWidth));
    bounds.removeFromLeft(CTRL_PADDING);

    m_realLabel.setBounds(bounds.removeFromLeft(LABEL_WIDTH));
    m_PNrealSlider.setBounds(bounds.removeFromLeft(sliderWidth - LABEL_WIDTH));
    bounds.removeFromLeft(CTRL_PADDING);

    m_imagLabel.setBounds(bounds.removeFromLeft(LABEL_WIDTH));
    m_PNimagSlider.setBounds(bounds.removeFromLeft(sliderWidth - LABEL_WIDTH));
    bounds.removeFromLeft(CTRL_PADDING);
    m_PNbutton.setBounds(bounds);
}

void PNcontrolComponent::updateName()
{
    
    if (m_type == PNcontrolComponent::PNType::pole)
        m_name = "Pole ";
    if (m_type == PNcontrolComponent::PNType::zero)
        m_name = "Zero ";
    
    m_name = m_name + String(2*m_index + 1);
    if (m_isConjugated)
        m_name = m_name + " & " + String(2*m_index + 2);

    m_PNLabel.setText(m_name, NotificationType::dontSendNotification);
}

void PNcontrolComponent::setAcivation(bool newValue)
{ 
    m_isActivated = newValue; 
    if (m_type == PNType::pole)
        m_vts.getParameter(paramPoleBool.ID[m_index])->setValueNotifyingHost(newValue);

    if (m_type == PNType::zero)
        m_vts.getParameter(paramZeroBool.ID[m_index])->setValueNotifyingHost(newValue);
        
}

void PNcontrolComponent::setConjugation(bool newValue)
{ 
    m_isConjugated = newValue; 
    if (m_type == PNType::pole)
        m_vts.getParameter(paramPoleConjugated.ID[m_index])->setValueNotifyingHost(newValue);

    if (m_type == PNType::zero)
        m_vts.getParameter(paramZeroConjugated.ID[m_index])->setValueNotifyingHost(newValue);

    // if not conjugated, set imag to zero
    if (!newValue)
        m_PNimagSlider.setValue(0.0);
    
    if (somethingChanged != nullptr)
        somethingChanged();
}

void PNcontrolComponent::setSliderValues(std::complex<float> newValue)
    {
        if (m_type == PNType::pole && !m_vts.getParameter(paramPoleConjugated.ID[m_index])->getValue())
        {
            m_PNimagSlider.setValue(0);
        } else if (m_type == PNType::zero && !m_vts.getParameter(paramZeroConjugated.ID[m_index])->getValue()) { 
            m_PNimagSlider.setValue(0);
        } else {
            m_PNimagSlider.setValue(newValue.imag());
        }
            
        m_PNrealSlider.setValue(newValue.real());
    };

void PNcontrolComponent::statusButtonClicked()
{
    // Hierbei koennte noch in betracht gezogen werden, dass immer die gleiche
    // Anzahl an Pole und Nullstellen verwendet werden (kausales System -> 
    // Zaehler <= Nenner)
    if (m_type == PNType::pole)
    {
        bool newValue = !m_vts.getParameter(paramPoleBool.ID[m_index])->getValue();
        if (newValue == m_isActivated)
            return;
        m_vts.getParameter(paramPoleBool.ID[m_index])->setValueNotifyingHost(newValue);
        m_isActivated = newValue;
    }
    if (m_type == PNType::zero)
    {
        bool newValue = !m_vts.getParameter(paramZeroBool.ID[m_index])->getValue();
        if (newValue == m_isActivated)
            return;
        m_vts.getParameter(paramZeroBool.ID[m_index])->setValueNotifyingHost(newValue);
        m_isActivated = newValue;
    }
    if (somethingChanged != nullptr)
        somethingChanged();
    repaint();
}
