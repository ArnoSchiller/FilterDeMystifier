/*
  ==============================================================================

    BodeMagnitudeComponent.h
    Created: 19 Jul 2020 9:57:52am
    
    Authors:    Arno Schiller (AS)

    SPDX-License-Identifier: BSD-3-Clause
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ScaledPlot.h"

//==============================================================================
/*
*/
class BodeMagnitudeComponent    : public ScaledPlot
{
public:
    BodeMagnitudeComponent(bool withAutoScale);
    ~BodeMagnitudeComponent();

    void paint(Graphics&) override;
    void resized() override;

    void setSamplingRate(float fs)
    {
        m_maxValueX = fs;
        m_valueStepX = (m_maxValueX - m_minValueX) / 6;
    };
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BodeMagnitudeComponent)
};
