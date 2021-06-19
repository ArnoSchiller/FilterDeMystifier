/*
  ==============================================================================
    ProtectionComponent.h

    GUI component for protection.


    Authors:    Arno Schiller (AS)
    Version:    v0.0.1

    SPDX-License-Identifier: BSD-3-Clause

    Version history:
    Version:   (Author) Description:								Date:     \n
    v0.0.1     (AS) First initialize. Implemented component   	    05.06.2021\n
 ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PNParameter.h"

//==============================================================================
/*
*/
class ProtectionComponent : public Component
{
public:

    ProtectionComponent(AudioProcessorValueTreeState& vts)
        : m_vts(vts)
        {
            
            m_buttonLimiter.onClick = [this]() 
            { 
                m_vts.getParameter(paramLimiterBool.ID)->setValue(!m_vts.getParameter(paramLimiterBool.ID)->getValue());
                updateButtons();
            };
            m_buttonPoleProtect.onClick = [this]() 
            { 
                m_vts.getParameter(paramPoleProtectBool.ID)->setValue(!m_vts.getParameter(paramPoleProtectBool.ID)->getValue());
                updateButtons();
                if(m_vts.getParameter(paramPoleProtectBool.ID)->getValue())
                {
                    double max_r = 0.98;
                    for (int kk = 0; kk < MAX_POLE_INSTANCES; kk++)
                    {
                        double real = m_vts.getParameter(paramPoleReal.ID[kk])->getValue();
                        double imag = m_vts.getParameter(paramPoleImag.ID[kk])->getValue();
                        double r = sqrt(real * real + imag * imag); 
                        if (r > max_r)
                        {
                            m_vts.getParameter(paramPoleReal.ID[kk])->setValue(real / (r/max_r));
                            m_vts.getParameter(paramPoleImag.ID[kk])->setValue(imag / (r/max_r));
                        }
                    }
                }
            };

            addAndMakeVisible(m_labelComponent);
            addAndMakeVisible(m_labelLimiter);
            addAndMakeVisible(m_labelPoleProtect);

            m_labelComponent.setText(String("Danger Zone"),NotificationType::dontSendNotification);
            m_labelPoleProtect.setText(String("Pole Protection"),NotificationType::dontSendNotification);
            m_labelLimiter.setText(String("Limiter"), NotificationType::dontSendNotification);
            
            addAndMakeVisible(m_buttonLimiter);
            m_buttonLimiter.setColour(m_buttonLimiter.textColourOffId, Colours::black);
            addAndMakeVisible(m_buttonPoleProtect);
            m_buttonPoleProtect.setColour(m_buttonPoleProtect.textColourOffId, Colours::black);
            updateButtons();

        };
    ~ProtectionComponent(){};

    void paint(Graphics& g) override
    {
        g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId).darker(0.2));
        g.drawRect(getLocalBounds(), 1.0f);
        g.setColour(Colours::yellow);
        g.drawRect(getLocalBounds().reduced(1), 1.0f);
        g.setColour(Colours::black);
        g.drawRect(getLocalBounds().reduced(2), 1.0f);
    };
    void resized() override
    {
        auto bounds = getBounds();
        auto width = bounds.getWidth();

        /*
        m_labelComponent.setBounds(bounds.removeFromTop(0.5 * bounds.getHeight()));

        bounds.removeFromLeft(0.1 * width);
        
        m_buttonLimiter.setBounds(bounds.removeFromLeft(0.3 * width).reduced(2));
        bounds.removeFromLeft(0.2 * width);

        m_buttonPoleProtect.setBounds(bounds.removeFromLeft(0.3 * width).reduced(2));
        */

        m_labelComponent.setBounds(bounds.removeFromLeft(0.35 * bounds.getWidth()).reduced(4));

        auto upperBounds = bounds.removeFromTop(0.5*bounds.getHeight());
        m_labelLimiter.setBounds(upperBounds.removeFromLeft(0.6 * upperBounds.getWidth()));
        m_buttonLimiter.setBounds(upperBounds.removeFromLeft(0.9 * upperBounds.getWidth()).reduced(4));

        m_labelPoleProtect.setBounds(bounds.removeFromLeft(0.6 * bounds.getWidth()));
        m_buttonPoleProtect.setBounds(bounds.removeFromLeft(0.9 * bounds.getWidth()).reduced(4));
        
    };


private:
    AudioProcessorValueTreeState& m_vts;

    Label m_labelComponent;
    Label m_labelPoleProtect;
    Label m_labelLimiter;

    TextButton m_buttonPoleProtect;
    TextButton m_buttonLimiter;

    void updateButtons()
    {
        if(m_vts.getParameter(paramLimiterBool.ID)->getValue())
        {
            m_buttonLimiter.setColour(m_buttonLimiter.buttonColourId, Colours::yellow);
            m_buttonLimiter.setButtonText("Activated");
        } else {
            m_buttonLimiter.setColour(m_buttonLimiter.buttonColourId, Colours::red);
            m_buttonLimiter.setButtonText("ATTENTION");
        }
        if(m_vts.getParameter(paramPoleProtectBool.ID)->getValue())
        {
            m_buttonPoleProtect.setColour(m_buttonLimiter.buttonColourId, Colours::yellow);
            m_buttonPoleProtect.setButtonText("Activated");
        } else {
            m_buttonPoleProtect.setColour(m_buttonLimiter.buttonColourId, Colours::red);
            m_buttonPoleProtect.setButtonText("ATTENTION");
        }

        if (!m_vts.getParameter(paramLimiterBool.ID)->getValue() || !m_vts.getParameter(paramPoleProtectBool.ID)->getValue())
        {
            m_labelComponent.setColour(m_labelComponent.textColourId, Colours::red);
        } else {
            m_labelComponent.setColour(m_labelComponent.textColourId, Colours::white);
        }
    }
};