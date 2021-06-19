/*
  ==============================================================================

    BodeDiagramComponent.h
    Created: 19 Jul 2020 9:55:20am

    Authors:    Arno Schiller (AS)
    Version:    v0.0.1
    
    SPDX-License-Identifier: BSD-3-Clause

  ==============================================================================
*/
#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <complex>
#include <JuceHeader.h>
#include "BodeMagnitudeComponent.h"
#include "BodePhaseComponent.h"
#include "PNParameter.h"

const float localPi = 3.14159265358979323846;
//==============================================================================
/*
*/
class BodeDiagramComponent    : public Component
{
public:
    enum LayoutTypes
    {
        compact,
        horizontal,
        none
    };

    BodeDiagramComponent(AudioProcessorValueTreeState& vts, LayoutTypes layout)
        : m_vts(vts), m_layout(layout), m_bodeMagnitudePlot(true),
        m_bodePhasePlot(false), m_numberOfSamples(1000), m_fs(48000.0f), m_plot_0_to_fs(false)
    {
        // In your constructor, you should add any child components, and
        // initialise any special settings that your component needs.
        addAndMakeVisible(m_bodeMagnitudePlot);
        addAndMakeVisible(m_bodePhasePlot);
        updatePlot();
    }

    ~BodeDiagramComponent()
    {
    }

    void paint (Graphics& g) override
    {
        /* This demo code just fills the component's background and
           draws some placeholder text to get you started.

           You should replace everything in this method with your own
           drawing code..
        */

        g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background
        
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        if(m_layout == LayoutTypes::compact)
        {
            m_bodeMagnitudePlot.setBounds(bounds.removeFromTop(bounds.getHeight() / 2));
            m_bodePhasePlot.setBounds(bounds);
        }
        if (m_layout == LayoutTypes::horizontal)
        {
            m_bodeMagnitudePlot.setBounds(bounds.removeFromLeft(bounds.getWidth() / 2));
            m_bodePhasePlot.setBounds(bounds);
        }
    }
    
    void setSamplingRate(float fs) 
    { 
        m_fs = fs;

        if (!m_plot_0_to_fs)
            fs = fs/2;

        m_bodePhasePlot.setSamplingRate(fs); 
        m_bodeMagnitudePlot.setSamplingRate(fs);
    }

    void updatePlot()
    {
        std::vector<float> frequencyVec;
        std::vector<float> magnitudeVec;
        std::vector<float> phaseVec;

        updateDataFromPoleZero(frequencyVec, magnitudeVec, phaseVec);

        m_bodePhasePlot.setData(frequencyVec, phaseVec);
        m_bodeMagnitudePlot.setData(frequencyVec, magnitudeVec);
        repaint();
    }

private:
  
    AudioProcessorValueTreeState& m_vts;

    PNParameter m_PNparam;
    int m_numOfPole = m_PNparam.maxNrOfPole;
    int m_numOfZero = m_PNparam.maxNrOfZero;

    int m_numberOfSamples;
    float m_fs;

    // set true to plot from 0 to fs, else 0 to fs/2 will be plotted
    bool m_plot_0_to_fs; 

    std::vector<std::complex<float>> m_poles;
    std::vector<std::complex<float>> m_zeros;

    LayoutTypes m_layout;

    BodePhaseComponent m_bodePhasePlot;
    BodeMagnitudeComponent m_bodeMagnitudePlot;

    void updateDataFromPoleZero(std::vector<float>& frequency,
        std::vector<float>& magnitude, std::vector<float>& phase)
    {
        updatePoleAndZero();
        
        float freqStep = 0;
        float angleStep = 0;
        if (m_plot_0_to_fs)
        {
            freqStep = m_fs / m_numberOfSamples;
            angleStep = 2 * localPi / m_numberOfSamples;
        } else {
            freqStep = m_fs/2 / m_numberOfSamples;
            angleStep = localPi / m_numberOfSamples;
        }
        

        for (auto kk = 0; kk <= m_numberOfSamples; ++kk)
        {
            float realPart = cos(kk * angleStep);
            float imagPart = sin(kk * angleStep);
            std::complex<float> z(realPart, imagPart);
            //std::complex<float> z;
            //z = exp(kk * angleStep);
            std::complex<float> h;
            float b0 = *(m_vts.getRawParameterValue(paramb0.ID));
            h = pow(10, (b0/20));       // from dB to linear
            for (auto ll = 0U; ll < m_zeros.size(); ++ll)
                h *= (z - m_zeros[ll]);
            for (auto ll = 0U; ll < m_poles.size(); ++ll)
                h /= (z - m_poles[ll]);

            frequency.push_back(kk * freqStep);
            magnitude.push_back(20 * log10(std::abs(h)));
            //magnitude.push_back(std::abs(h));
            phase.push_back(std::arg(h) * 180 / localPi);
        }
    }

    void updatePoleAndZero()
    {
        m_poles.clear();
        m_zeros.clear();

        for (auto kk = 0; kk < m_numOfPole; ++kk)
        {
            if (m_vts.getParameter(paramPoleBool.ID[kk])->getValue())
            {
                float real = *(m_vts.getRawParameterValue(paramPoleReal.ID[kk]));
                float imag = *(m_vts.getRawParameterValue(paramPoleImag.ID[kk]));
                m_poles.push_back(std::complex<float>(real, imag));

                if (m_vts.getParameter(paramPoleConjugated.ID[kk])->getValue())
                    m_poles.push_back(std::complex<float>(real, -imag));
            }
        }
        for (auto kk = 0; kk < m_numOfZero; ++kk)
        {
            if (m_vts.getParameter(paramZeroBool.ID[kk])->getValue())
            {
                float real = *(m_vts.getRawParameterValue(paramZeroReal.ID[kk]));
                float imag = *(m_vts.getRawParameterValue(paramZeroImag.ID[kk]));
                m_zeros.push_back(std::complex<float>(real, imag));

                if (m_vts.getParameter(paramZeroConjugated.ID[kk])->getValue())
                    m_zeros.push_back(std::complex<float>(real, -imag));
            }
        }

    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BodeDiagramComponent)
};
