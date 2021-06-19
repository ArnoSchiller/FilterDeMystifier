/*
  ==============================================================================
    PNcontrolComponent.h

    This component controles the value of one pole/zero. Two slider rules the
    imaginary and the real part of the value. Also it is possible to deactivate
    the pole/zero. To choose between pole and zero the constructor needs a type.


    Authors:    Jannik Hartog (JH), Arno Schiller (AS)
    Version:    v0.0.4
    
    SPDX-License-Identifier: BSD-3-Clause

    Version history:
    Version:   (Author) Description:								Date:     \n
    v0.0.1     (AS) First initialize. Implemented component to 	    13.07.2020\n
                   change the value of pole/zero by slider.  				  \n
    v0.0.2     (AS) Added button to activate/deactivate pole/zero.  14.07.2020\n
    v0.0.3     (AS) Updated look when pole/zero is not activated.   19.07.2020\n
    v0.0.4     (AS) Added public function to change slider values.  23.07.2020\n
 ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <complex>

typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
//==============================================================================
/*
*/
class PNcontrolComponent : public Component
{
public:
    enum PNType
    {
        pole,
        zero,
        none
    };

    PNcontrolComponent(AudioProcessorValueTreeState&, int, PNcontrolComponent::PNType);
    ~PNcontrolComponent();

    void paint(Graphics&) override;
    void resized() override;

    void setSliderValues(std::complex<float> newValue)
    {
        m_PNimagSlider.setValue(newValue.imag());
        m_PNrealSlider.setValue(newValue.real());
    };

    void setAcivation(bool isActivated);
    void setConjugation(bool isActivated);

    std::function<void()> somethingChanged;

private:
    AudioProcessorValueTreeState& m_vts;

    int m_index;
    PNcontrolComponent::PNType m_type;
    String m_name;

    double m_prev_imag;
    double m_prev_real;

    Label m_PNLabel;
    Slider m_PNimagSlider;
    std::unique_ptr<SliderAttachment> m_PNimagAttachment;
    Slider m_PNrealSlider;
    std::unique_ptr<SliderAttachment> m_PNrealAttachment;
    TextButton m_PNbutton;

    bool m_isActivated = false;
    bool m_isConjugated = false;

    void updateName();
    void statusButtonClicked();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PNcontrolComponent)
};