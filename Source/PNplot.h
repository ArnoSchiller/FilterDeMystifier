/*
  ==============================================================================
    PNplot.h

    This class represents the pole-zero diagram. 

    Authors:    Jannik Hartog (JH), Arno Schiller (AS)
    Version:    v0.1.0

    SPDX-License-Identifier: BSD-3-Clause

    Version history:
    Version:   (Author) Description:								Date:     \n
    v0.0.1     (AS) First initialize. Added the PNplot class.       10.06.2020\n
                    see PNplot.h for more details.                            \n
    v0.0.2     (AS) Added minimum window size (see resized).        10.06.2020\n
    v0.0.3     (AS) Included preset handler rules the pole and      13.07.2020\n
                    zero values.                                              \n
    v0.0.4     (AS) Included the first PNcontrolsComponent to       13.07.2020\n
                    change the value of one pole/zero.                        \n
    v0.0.5     (AS) Included a vector with the PNcontrolsComponent  14.07.2020\n
                    used to control the pole/zero.                            \n
                    activated, it is not plotted                              \n
    v0.0.6     (AS) Included bool to VTS to represent the state     14.07.2020\n
                    of the pole/zero. If pole/zero is not                     \n
                    activated, it is not plotted                              \n
    v0.0.7     (AS) Added index to paintPole and paintZero          14.07.2020\n
                    functions. The automatically count of same                \n
                    pole or zero  does not work yet. (vector error)           \n
    v0.0.8     (AS) Added functions to change pole and zero         21.07.2020\n
                    values via mouse drag. Does not work yet,                 \n
                    maybe because the value setter does not work.             \n
    v0.0.9     (AS) Changed class so the plot is a child element    22.07.2020\n
                    of the ScaledPlot class. Also outsourced the              \n
                    control components to PNComponent class.                  \n
    v0.1.0     (AS) Updated function to drag pole and zeros.        23.07.2020\n
                    Outsourced the setter functions to update                 \n
                    the slider values (not via vts). Works fine.              \n
 ===============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <cmath>
#include <algorithm>
#include <complex>
#include <unordered_map>
#include "PNParameter.h"
#include "PNcontrolComponent.h"
#include "ScaledPlot.h"

//==============================================================================
/*
*/
using namespace std::complex_literals;
class PNplot : public ScaledPlot 
{
public:
    PNplot(AudioProcessorValueTreeState&);
    ~PNplot();

    void paint(Graphics&) override;
    void resized() override;

    void mouseDown(const MouseEvent& event) override;
    void mouseDrag(const MouseEvent& event) override;
    void mouseUp(const MouseEvent& event) override;
    
    std::complex<float> getNewObjectValue() { return m_newObjectValue; };
    std::unordered_map<String, int> getSelectedObject() { return m_selectedObject; };

    std::function<void()> somethingChanged;
    std::function<void()> updateSlider;

private:

    AudioProcessorValueTreeState& m_vts;

    PNParameter m_PNparam;
    int m_numOfPole = m_PNparam.maxNrOfPole;
    int m_numOfZero = m_PNparam.maxNrOfZero;

    std::vector<std::complex<float>> m_poles;
    std::vector<std::complex<float>> m_zeros;
    std::vector<int> m_polesCount;
    std::vector<int> m_zerosCount;

    int m_graphCenterX;
    int m_graphCenterY;
    int m_graphWidthHeight;

    bool m_isSelected;
    std::unordered_map<String, int> m_selectedObject;
    std::complex<float> m_newObjectValue;
    void getSelectedObjectFromCoords(int x, int y);

    void paintPolesAndZeros(Graphics& g);

    void paintZero(Graphics& g, std::complex<float> complexValue, int index)
    {   paintZero(g, complexValue.real(), complexValue.imag(), index);     };
    void paintPole(Graphics& g, std::complex<float> complexValue, int index)
    {   paintPole(g, complexValue.real(), complexValue.imag(), index);     };

    void paintZero(Graphics& g, Point<float> point, int index)
    {   paintZero(g, point.getX(), point.getY(), index);   };
    void paintPole(Graphics& g, Point<float> point, int index)
    {   paintPole(g, point.getX(), point.getY(), index);   };

    void paintZero(Graphics& g, float real, float imaginary, int index);
    void paintPole(Graphics& g, float real, float imaginary, int index);

    void paintAxis(Graphics& g);
    
    void updatePolesAndZeros();
    void countSameValues(std::vector<std::complex<float>>& valuesToCount,
        std::vector<std::complex<float>>& uniqueValues, std::vector<int>& counts);

    void updateObjectValue(std::unordered_map<String, int> object, int x, int y);

    int isValueInVector(std::vector<std::complex<float>>& vector, std::complex<float> value);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PNplot)
};

