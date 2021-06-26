/*
  ==============================================================================
    PNplot.cpp
    
    Authors:    Jannik Hartog (JH), Arno Schiller (AS)

    SPDX-License-Identifier: BSD-3-Clause
 ==============================================================================
*/

#include <JuceHeader.h>

#include "PNplot.h"
#include "TransferFunLookAndFeel.h"

#define PADDING 10

#define TICKS_LENGTH 5
#define TICKS_STEP 0.5

#define POLE_DIAMETER 10
#define ZERO_DIAMETER POLE_DIAMETER 
#define INDEX_SIZE 10

//==============================================================================
PNplot::PNplot(AudioProcessorValueTreeState& vts)
    : ScaledPlot(false), m_vts(vts), m_isSelected(false),
    m_graphCenterX(0), m_graphCenterY(0), m_graphWidthHeight(0)
{
    m_axisLabelX = String("Real Part");
    m_minValueX = -1.5;  //jmax(paramPoleReal.minValue, paramZeroReal.minValue);
    m_maxValueX = 1.5;   //jmin(paramPoleReal.maxValue, paramZeroReal.maxValue);
    m_valueStepX = 0.5f;

    m_axisLabelY = String("Imaginary Part");
    m_minValueY = -1.5; // jmax(paramPoleImag.minValue, paramZeroImag.minValue);
    m_maxValueY = 1.5;  //jmin(paramPoleImag.maxValue, paramZeroImag.maxValue);
    m_valueStepY = 0.5f;

    m_selectedObject["p"] = -1;
    m_selectedObject["z"] = -1;

    m_isLogScaleActivated = false;
}

PNplot::~PNplot()
{
}

void PNplot::paint(Graphics& g)
{   
    g.setColour(PlotBackgroundColour);

    g.fillRect(m_startPosX, m_startPosY, m_endPosX-m_startPosX, m_endPosY-m_startPosY);

    m_graphCenterX = scaleToCoordsX(0.0);
    m_graphCenterY = scaleToCoordsY(0.0);
    // AXIS 
    paintAxis(g);

    // UNITCIRCLE
    g.setColour(AxisPlotColour);
    auto unitCicleDiameter = (scaleToCoordsX(1.0) - m_graphCenterX) * 2;
    auto unitCircleRadius = unitCicleDiameter / 2;

    g.drawEllipse(m_graphCenterX - unitCircleRadius, m_graphCenterY - unitCircleRadius,
        unitCicleDiameter, unitCicleDiameter, LINEWIDH_MEDIUM);

    paintPolesAndZeros(g);
}

void PNplot::resized()
{
    ScaledPlot::resized();
    auto width = m_endPosX - m_startPosX;
    auto height = m_endPosY - m_startPosY;
    if (width > height)
    {
        auto difference = width - height;
        m_startPosX += difference / 2;
        m_endPosX -= difference / 2;
    }
    if (width < height)
    {
        auto difference = height - width;
        m_startPosY += difference / 2;
        m_endPosY -= difference / 2;
    }
}

void PNplot::mouseDown(const MouseEvent& event)
{
    int x = event.x;
    int y = event.y;

    if (!m_isSelected)
    { 
        getSelectedObjectFromCoords(x, y);
        if (m_selectedObject["p"] != -1 || m_selectedObject["z"] != -1)
            m_isSelected = true;
    }
    if (m_isSelected)
    {
        if (x > m_endPosX)
            x = m_endPosX;
        if (x < m_startPosX)
            x = m_startPosX;

        // updated because complex conjugation
        //if (y > m_endPosY)
        //    y = m_endPosY;
        if (y > m_graphCenterY)
            y = m_graphCenterY;
        if (y < m_startPosY)
            y = m_startPosY;

        updateObjectValue(m_selectedObject, x, y);
    }
}

void PNplot::mouseDrag(const MouseEvent& event)
{
    mouseDown(event);
}

void PNplot::mouseUp(const MouseEvent& event)
{
    m_isSelected = false;
    m_selectedObject["p"] = -1;
    m_selectedObject["z"] = -1;
}

void PNplot::getSelectedObjectFromCoords(int x, int y)
{
    m_selectedObject["p"] = -1;
    m_selectedObject["z"] = -1;

    for (int kk = 0; kk < m_numOfPole; ++kk)
    {
        if (m_vts.getParameter(paramPoleBool.ID[kk])->getValue())
        {
            float real = *(m_vts.getRawParameterValue(paramPoleReal.ID[kk]));
            float imag = *(m_vts.getRawParameterValue(paramPoleImag.ID[kk]));
            int xVal = scaleToCoordsX(real);
            int yVal = scaleToCoordsY(imag);
            if (x < xVal + POLE_DIAMETER / 2 && x > xVal - POLE_DIAMETER / 2 &&
                y < yVal + POLE_DIAMETER / 2 && y > yVal - POLE_DIAMETER / 2)
            {
                m_selectedObject["p"] = kk;
                return;
            }
        }
    }
    for (int kk = 0; kk < m_numOfPole; ++kk)
    {
        if (m_vts.getParameter(paramZeroBool.ID[kk])->getValue())
        {
            float real = *(m_vts.getRawParameterValue(paramZeroReal.ID[kk]));
            float imag = *(m_vts.getRawParameterValue(paramZeroImag.ID[kk]));
            int xVal = scaleToCoordsX(real);
            int yVal = scaleToCoordsY(imag);
            if (x < xVal + POLE_DIAMETER / 2 && x > xVal - POLE_DIAMETER / 2 &&
                y < yVal + POLE_DIAMETER / 2 && y > yVal - POLE_DIAMETER / 2)
            {
                m_selectedObject["z"] = kk;
                return;
            }
        }
    }
}

void PNplot::paintPolesAndZeros(Graphics& g)
{
    updatePolesAndZeros();
    if (m_poles.size() != m_polesCount.size() || m_zeros.size() != m_zerosCount.size())
        return;

    for (auto kk = 0U; kk < m_poles.size(); ++kk)
    {
        paintPole(g, m_poles[kk], m_polesCount[kk]);
    }
    for (auto kk = 0U; kk < m_zeros.size(); ++kk)
    {
        paintZero(g, m_zeros[kk], m_zerosCount[kk]);
    }
}

void PNplot::paintZero(Graphics& g, float real, float imaginary, int index)
{
    g.setColour(ZeroColour);
    auto xPos = scaleToCoordsX(real);
    auto yPos = scaleToCoordsY(imaginary);
    g.drawEllipse(xPos - ZERO_DIAMETER / 2, yPos - ZERO_DIAMETER / 2, ZERO_DIAMETER, ZERO_DIAMETER, LINEWIDH_MEDIUM);
    if (index > 1 && index < 10)
    {
        auto textX = xPos + POLE_DIAMETER / 4;
        auto textY = yPos + POLE_DIAMETER / 2;
        g.drawText(String(index), textX, textY, INDEX_SIZE, INDEX_SIZE, Justification::centred);
    }
} 

void PNplot::paintPole(Graphics& g, float real, float imaginary, int index)
{
    g.setColour(PoleColour);
    auto xPos = scaleToCoordsX(real);
    auto yPos = scaleToCoordsY(imaginary);
    g.drawLine(xPos - POLE_DIAMETER / 2, yPos - POLE_DIAMETER / 2, xPos + POLE_DIAMETER / 2, yPos + POLE_DIAMETER / 2, LINEWIDH_MEDIUM);
    g.drawLine(xPos + POLE_DIAMETER / 2, yPos - POLE_DIAMETER / 2, xPos - POLE_DIAMETER / 2, yPos + POLE_DIAMETER / 2, LINEWIDH_MEDIUM);
    if(index > 1 && index < 10)
    {
        auto textX = xPos + POLE_DIAMETER/2;
        auto textY = yPos + POLE_DIAMETER/4;
        g.drawText(String(index), textX, textY, INDEX_SIZE, INDEX_SIZE, Justification::centred);
    }
}

void PNplot::paintAxis(Graphics& g)
{
    g.setColour(AxisPlotColour);

    // frame 
    g.drawRect(m_startPosX, m_startPosY, m_endPosX - m_startPosX, m_endPosY - m_startPosY, LINEWIDH_BIG);

    // Y axis
    Line<float> yAxis = Line<float>(m_graphCenterX, m_startPosY, m_graphCenterX, m_endPosY);
    g.drawLine(yAxis, LINEWIDH_MEDIUM);

    // X axis
    Line<float> xAxis = Line<float>(m_startPosX, m_graphCenterY, m_endPosX, m_graphCenterY);
    g.drawLine(xAxis, LINEWIDH_MEDIUM);

    // AXIS DESCRIPTION 
    addAxisDescription(g);
    
    // AXIS TICKS
    g.setColour(AxisTicksColour);
    g.setFont(FontSize_medium);
    // Y ticks
    float valueY = static_cast<float>(static_cast<int>(m_minValueY * 2))/2;
    while (valueY <= m_maxValueY)
    {
        int posY = scaleToCoordsY(valueY);
        g.drawLine(Line<float>(m_startPosX, posY, m_startPosX + TICKS_LENGTH, posY), LINEWIDH_MEDIUM);
        g.drawLine(Line<float>(m_endPosX - TICKS_LENGTH, posY, m_endPosX, posY), LINEWIDH_MEDIUM);
       
        g.drawText(String(valueY), m_startPosX - AXIS_TEXT_WIDTH,
            posY - AXIS_TEXT_HEIGHT / 2, AXIS_TEXT_WIDTH, AXIS_TEXT_HEIGHT,
            Justification::centred);
        valueY += m_valueStepY;
    }

    // X ticks
    float valueX = static_cast<float>(static_cast<int>(m_minValueX * 2)) / 2;
    while (valueX <= m_maxValueX)
    {
        int posX = scaleToCoordsX(valueX);
        g.drawLine(Line<float>(posX, m_startPosY, posX, m_startPosY + TICKS_LENGTH), LINEWIDH_MEDIUM);
        g.drawLine(Line<float>(posX, m_endPosY - TICKS_LENGTH, posX, m_endPosY), LINEWIDH_MEDIUM);
        g.drawText(String(valueX), posX - AXIS_TEXT_WIDTH/2,
            m_endPosY, AXIS_TEXT_WIDTH,
            AXIS_TEXT_HEIGHT, Justification::centred);
        valueX += m_valueStepX;
    }
}

/// <summary>
/// Updating the data by getting the pole and zero from the vts and transfer 
/// them into a vector of unique values and a vector of numbers the same values
/// are given. Those member vectors are used to plot the pole zero diagram.
/// </summary>
void PNplot::updatePolesAndZeros()
{
    std::vector<std::complex<float>> poles;
    std::vector<std::complex<float>> zeros;
    m_poles.clear();
    m_zeros.clear();
    m_polesCount.clear();
    m_zerosCount.clear();

    for (auto kk = 0; kk < m_numOfPole; ++kk)
    {
        if (m_vts.getParameter(paramPoleBool.ID[kk])->getValue())
        {
            float real = *(m_vts.getRawParameterValue(paramPoleReal.ID[kk]));
            float imag = *(m_vts.getRawParameterValue(paramPoleImag.ID[kk]));
            poles.push_back(std::complex<float>(real, imag));

            if(m_vts.getParameter(paramPoleConjugated.ID[kk])->getValue())
                poles.push_back(std::complex<float>(real, -imag));
        }
    }
    for (auto kk = 0; kk < m_numOfPole; ++kk)
    {
        if (m_vts.getParameter(paramZeroBool.ID[kk])->getValue())
        {
            float real = *(m_vts.getRawParameterValue(paramZeroReal.ID[kk]));
            float imag = *(m_vts.getRawParameterValue(paramZeroImag.ID[kk]));
            zeros.push_back(std::complex<float>(real, imag));
            
            if(m_vts.getParameter(paramZeroConjugated.ID[kk])->getValue())
                zeros.push_back(std::complex<float>(real, -imag));
        }
    }

    countSameValues(poles, m_poles, m_polesCount);
    countSameValues(zeros, m_zeros, m_zerosCount);
}

/// <summary>
/// Returns a vector of unique elements (complex numbers) and a vector 
/// indicating the number of times the values are included in the input vector.
/// </summary>
/// <param name="valuesToCount"> Reference to a vector with complex numbers to 
/// count. (std::vector<std::complex<float>>&) </param>
/// <param name="uniqueValues"> Reference to a vector the unique values will be
/// saved in. (std::vector<std::complex<float>>&) </param>
/// <param name="counts"> Reference to a vector representing the number of times
/// the values of the unique vector are included in the input vector. </param>
void PNplot::countSameValues(std::vector<std::complex<float>>& valuesToCount, 
    std::vector<std::complex<float>>& uniqueValues, std::vector<int>& counts)
{
    for (auto kk = 0U; kk < valuesToCount.size(); ++kk)
    {
        int index = isValueInVector(uniqueValues, valuesToCount[kk]);
        if (index == -1)
        {
            uniqueValues.push_back(valuesToCount[kk]);
            counts.push_back(1);
        }
        else
        {
            counts[index] += 1;
        }
    }
}

void PNplot::updateObjectValue(std::unordered_map<String, int> object, int x, int y)
{
    float real = scaleToValueX(x);
    float imag = scaleToValueY(y);
    m_newObjectValue = std::complex<float>(real, imag);
    m_selectedObject = object;
    if (updateSlider != nullptr)
        updateSlider();
    /* 
    // Bei dieser Variante kommt es zu einem Fehler, wirkt wie ein 
    // Skalierungsfehler, liegt aber am Setzen der Werte.
    int poleIndex = object["p"];
    int zeroIndex = object["z"];
    if (poleIndex != -1)
    {
        m_vts.getParameter(paramPoleImag.ID[poleIndex])->setValueNotifyingHost(imag);
        m_vts.getParameter(paramPoleReal.ID[poleIndex])->setValueNotifyingHost(real);
    }
    if (zeroIndex != -1)
    {
        m_vts.getParameter(paramZeroImag.ID[zeroIndex])->setValueNotifyingHost(imag);
        m_vts.getParameter(paramZeroReal.ID[zeroIndex])->setValueNotifyingHost(real);
    }
    //*/
    
    if (somethingChanged != nullptr)
        somethingChanged();
}

/// <summary>
/// Returns the index, at which position the value corresponds to the input 
/// value. Returns -1 if the value is not included to the vector. 
/// </summary>
/// <param name="vector"> Reference to a vector with complex numbers to compare.
/// (std::vector<std::complex<float>>&) </param>
/// <param name="value">complex number to compare with.
/// (std::complex<float>)</param>
/// <returns></returns>
int PNplot::isValueInVector(std::vector<std::complex<float>>& vector, std::complex<float> value)
{
    for (auto kk = 0U; kk < vector.size(); ++kk)
    {
        if (real(vector[kk]) == real(value) && imag(vector[kk]) == imag(value))
            return int(kk);
    }
    return -1;
}

