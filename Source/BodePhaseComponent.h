/*
  ==============================================================================

    BodePhaseComponent.h

    This class represents the phase plot of the bode diagram. It uses the 
    basis class ScaledPlot.

    Authors:    Jannik Hartog (JH), Arno Schiller (AS)
    Version:    v0.0.1
    
    SPDX-License-Identifier: BSD-3-Clause

    Version history:
    Version:   (Author) Description:								Date:     \n
    v0.0.1     (AS) First initialize. Added first values to test    19.07.2020\n
                    the axis plot.                                            \n
 ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ScaledPlot.h"

//==============================================================================
/*
*/
class BodePhaseComponent    : public ScaledPlot
{
public:
    BodePhaseComponent(bool withAutoScale);
    ~BodePhaseComponent();

    void paint (Graphics&) override;
    void resized() override;

    void setSamplingRate(float fs) 
    { 
        m_maxValueX = fs; 
        m_valueStepX = (m_maxValueX - m_minValueX) / 6; 
    };

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BodePhaseComponent)
};
