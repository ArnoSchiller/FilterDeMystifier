/*
  ==============================================================================

    BodePhaseComponent.cpp
    Created: 19 Jul 2020 9:56:00am
    
    Authors:    Arno Schiller (AS)

    SPDX-License-Identifier: BSD-3-Clause

  ==============================================================================
*/

#include <JuceHeader.h>
#include "BodePhaseComponent.h"

//==============================================================================
BodePhaseComponent::BodePhaseComponent(bool withAutoScale)
    : ScaledPlot(withAutoScale)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    m_axisLabelLogX = String("Frequency / Hz");
    m_axisLabelX = String("Frequency / kHz");
    m_minValueX = 0.0f;
    m_maxValueX = 10000.0f;
    m_valueStepX = 1000.0f;

    m_axisLabelY = String("Phase / degrees");
    m_minValueY = -180.0f;
    m_maxValueY = 180.0f;
    m_valueStepY = 90.0f;

    addAndMakeVisible(m_logButton);
}

BodePhaseComponent::~BodePhaseComponent()
{
}

void BodePhaseComponent::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    ScaledPlot::paint(g);
}

void BodePhaseComponent::resized()
{
    ScaledPlot::resized();
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
