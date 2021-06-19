/*
  ==============================================================================

    BodeMagnitudeComponent.cpp
    Created: 19 Jul 2020 9:57:52am
    
    Authors:    Arno Schiller (AS)

    SPDX-License-Identifier: BSD-3-Clause
  ==============================================================================
*/

#include <JuceHeader.h>
#include "BodeMagnitudeComponent.h"

//==============================================================================
BodeMagnitudeComponent::BodeMagnitudeComponent(bool withAutoScale)
    : ScaledPlot(withAutoScale)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    m_axisLabelX    = String("Frequency / kHz");
    m_minValueX     = 0.0f;
    m_maxValueX     = 10000.0f;
    m_valueStepX    = 1000.0f;

    m_axisLabelY    = String("Magnitude / dB");
    m_minValueY     = -60.0f;
    m_maxValueY     = 20.0f;
    m_valueStepY    = 20.0f;
}

BodeMagnitudeComponent::~BodeMagnitudeComponent()
{
}

void BodeMagnitudeComponent::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    ScaledPlot::paint(g);
}

void BodeMagnitudeComponent::resized()
{
    ScaledPlot::resized();
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
