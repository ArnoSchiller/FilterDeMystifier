/*
  ==============================================================================
    ScaledPlot.h

    This class serves as the basis for all plots. It has functions to convert 
    the values into coordinates and by default axes are plotted and labeled.

    Authors:    Jannik Hartog (JH), Arno Schiller (AS)
    Version:    v0.0.3
    
    SPDX-License-Identifier: BSD-3-Clause

    Version history:
    Version:   (Author) Description:								Date:     \n
    v0.0.1     (AS) First initialize. Added scale functions and     19.07.2020\n
                    paint of axis with axis description.                      \n 
    v0.0.2     (AS) Added axis styles to use different styles.      19.07.2020\n
                    Included axis style rectangular with ticks.               \n 
    v0.0.3     (AS) Included TransferFunLookAndFeel.                19.07.2020\n
    v0.0.3     (AS) Added auto scale to y axis.                     24.07.2020\n

    ToDo: - Added axis ticks to classic axis style.
 ==============================================================================
*/
#pragma once

#include <JuceHeader.h>
#include "TransferFunLookAndFeel.h"

//==============================================================================
/*
*/
#define PADDING 10

#define AXIS_DESCRIPTION_HEIGHT 10
#define AXIS_TEXT_WIDTH 25
#define AXIS_TEXT_HEIGHT 15

#define AXIS_DESCRIPTION_SPACE_HEIGHT (AXIS_TEXT_HEIGHT + AXIS_DESCRIPTION_HEIGHT)
#define AXIS_DESCRIPTION_SPACE_WIDTH (AXIS_TEXT_WIDTH + AXIS_DESCRIPTION_HEIGHT)

class ScaledPlot    : public Component
{
public:
    enum axisStyles
    {
        classic,
        rect,
        none
    };
    ScaledPlot(bool withAutoScale)
        : m_withAutoScale(withAutoScale), m_font(FontSize_medium),
        m_startPosX(0), m_endPosX(1), m_startPosY(0), m_endPosY(1), 
        m_minValueX(0.0), m_maxValueX(10.0), m_valueStepX(1.0),
        m_minValueY(0.0), m_maxValueY(10.0), m_valueStepY(1.0),
        m_axisLabelX("x-axis"), m_axisLabelY("y-axis"),
        m_withAxisDescriotion(true), m_axisStyle(axisStyles::rect)
    {
        // In your constructor, you should add any child components, and
        // initialise any special settings that your component needs.
       
    }

    ~ScaledPlot()
    {
    }

    void setData(std::vector<float> xValues, std::vector<float> yValues)
    {
        if (xValues.size() != yValues.size())
            return;

        if (m_withAutoScale)
        {
            float maxYValue = *std::max_element(yValues.begin(), yValues.end());
            int maxSteps = static_cast<int>(maxYValue / m_valueStepY) + 1;
            m_maxValueY = maxSteps * m_valueStepY;

            float minYValue = *std::min_element(yValues.begin(), yValues.end());
            int minSteps = static_cast<int>(minYValue / m_valueStepY) -1;
            m_minValueY = minSteps * m_valueStepY;
        }

        m_xValues = xValues;
        m_yValues = yValues;

        scaleValuesToPath();
    }

    void resized() override
    {
        // This method is where you should set the bounds of any child
        // components that your component contains..
        auto r = getLocalBounds();
        m_startPosX = r.getX() + PADDING + AXIS_DESCRIPTION_SPACE_WIDTH;
        m_startPosY = r.getY() + PADDING;
        m_endPosX = r.getX() + r.getWidth() - PADDING;
        m_endPosY = r.getY() + r.getHeight() - PADDING - AXIS_DESCRIPTION_SPACE_HEIGHT;
    }

    void paint (Graphics& g) override
    {
        /* This demo code just fills the component's background and
           draws some placeholder text to get you started.

           You should replace everything in this method with your own
           drawing code..
        */

        g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background
        
        g.setColour(BorderColour);
        g.drawRect(getLocalBounds(), LINEWIDH_MEDIUM);
        
        if (m_axisStyle == axisStyles::rect)
            paintAxisRect(g);
        if(m_axisStyle == axisStyles::classic)
            paintAxis(g);

        if (m_withAxisDescriotion)
            addAxisDescription(g);

        g.setColour(PlotColour);
        scaleValuesToPath();
        if (!m_valuesToPlot.isEmpty())
            g.strokePath(m_valuesToPlot, PathStrokeType(1.0f));
    }

protected:
    void paintAxis(Graphics& g)
    {
        g.setColour(AxisPlotColour);
        // Y axis
        int xPos = scaleToCoordsX(0.0);
        Line<float> yAxis = Line<float>(xPos, m_startPosY, xPos, m_endPosY);
        g.drawLine(yAxis, LINEWIDH_BIG);

        // X axis
        int yPos = scaleToCoordsY(0.0);
        Line<float> xAxis = Line<float>(m_startPosX, yPos, m_endPosX, yPos);
        g.drawLine(xAxis, LINEWIDH_BIG);
    }    
    
    void paintAxisRect(Graphics& g)
    {
        g.setColour(AxisPlotColour);
        g.drawRect(m_startPosX, m_startPosY, m_endPosX - m_startPosX,
            m_endPosY - m_startPosY, LINEWIDH_BIG);

        addAxisTicksRect(g);
    }

    void addAxisTicksRect(Graphics& g)
    {
        g.setFont(FontSize_small);
     
        // X axis
        float valueX = m_minValueX;
        while (valueX <= m_maxValueX)
        {
            int posX = scaleToCoordsX(valueX);
            if(valueX == 0)
                g.drawLine(posX, m_startPosY, posX, m_endPosY, LINEWIDH_MEDIUM);
            else
                g.drawLine(posX, m_startPosY, posX, m_endPosY, LINEWIDH_SMALL);
            g.drawText(String(valueX/1000), posX - AXIS_TEXT_WIDTH / 2,
                m_endPosY , AXIS_TEXT_WIDTH, AXIS_TEXT_HEIGHT, 
                Justification::centred);
            valueX += m_valueStepX;
        }

        // Y axis
        float valueY = m_minValueY;
        while (valueY <= m_maxValueY)
        {
            int posY = scaleToCoordsY(valueY);
            if(valueY == 0)
                g.drawLine(m_startPosX, posY, m_endPosX, posY, LINEWIDH_MEDIUM);
            else
                g.drawLine(m_startPosX, posY, m_endPosX, posY, LINEWIDH_SMALL);
            g.drawText(String(valueY), m_startPosX - AXIS_TEXT_WIDTH,
                posY- AXIS_TEXT_HEIGHT / 2, AXIS_TEXT_WIDTH-2, AXIS_TEXT_HEIGHT, 
                Justification::centredRight);
            valueY += m_valueStepY;
        }
    }

    void addAxisDescription(Graphics& g)
    {
        g.setColour(AxisTextColour);
        g.setFont(FontSize_medium);
        // Y axis
        // rotated text: see https://forum.juce.com/t/draw-rotated-text/14695/11
        GlyphArrangement ga;
        ga.addLineOfText(m_font, m_axisLabelY,
            m_startPosX - AXIS_TEXT_WIDTH, m_startPosY + (m_endPosY - m_startPosY) / 2);
        Path p;
        ga.createPath(p);
        auto pathBounds = p.getBounds();
        auto bottomLeft = pathBounds.getBottomLeft();
        p.applyTransform(AffineTransform()
            .rotated(degreesToRadians(-90.0),
                bottomLeft.x,
                bottomLeft.y)
            .translated(0.0, pathBounds.getWidth() / 2)
        );
        g.fillPath(p);

        // x axis
        g.drawText(m_axisLabelX, m_startPosX, m_endPosY + AXIS_TEXT_HEIGHT, 
            m_endPosX - m_startPosX, AXIS_DESCRIPTION_HEIGHT, Justification::centred);
    }

    float scaleToValueX(int pos)
    {
        if (pos < m_startPosX)
            pos = m_startPosX;
        if (pos > m_endPosX)
            pos = m_endPosX;

        float scaleValue = static_cast<float>(pos - m_startPosX) / static_cast<float>(m_endPosX - m_startPosX);
        float rV = scaleValue * (m_maxValueX - m_minValueX) + m_minValueX;
        return  scaleValue * (m_maxValueX - m_minValueX) + m_minValueX;
    }    
    
    float scaleToValueY(int pos)
    {
        if (pos < m_startPosY)
            pos = m_startPosY;
        if (pos > m_endPosY)
            pos = m_endPosY;

        float scaleValue = 1.0 - (static_cast<float>(pos - m_startPosY) / static_cast<float>(m_endPosY - m_startPosY));
        return  scaleValue * (m_maxValueY - m_minValueY) + m_minValueY;
    }

    int scaleToCoordsX(float value)
    {
        if (value < m_minValueX)
            value = m_minValueX;
        if (value > m_maxValueX)
            value = m_maxValueX;

        float scaleValue = (value - m_minValueX) / (m_maxValueX - m_minValueX);
        return  static_cast<int> (scaleValue * (m_endPosX - m_startPosX)) + m_startPosX;
    }

    int scaleToCoordsY(float value)
    {
        if (value < m_minValueY)
            value = m_minValueY;
        if (value > m_maxValueY)
            value = m_maxValueY;

        float scaleValue = 1.0 - ((value - m_minValueY) / (m_maxValueY - m_minValueY));
        return  static_cast<int> (scaleValue * (m_endPosY - m_startPosY)) + m_startPosY;
    }

    void scaleValuesToPath()
    {
        m_valuesToPlot.clear();
        for (auto kk = 0U; kk < m_xValues.size(); ++kk)
        {
            int xPos = scaleToCoordsX(m_xValues[kk]);
            int yPos = scaleToCoordsY(m_yValues[kk]);
            if (kk == 0)
                m_valuesToPlot.startNewSubPath(xPos, yPos);
            else
                m_valuesToPlot.lineTo(Point<float>(xPos, yPos));
        }
    }

    bool m_withAutoScale;

    Font m_font;

    String m_axisLabelX;
    String m_axisLabelY;

    // style setter
    bool m_withAxisDescriotion;
    axisStyles m_axisStyle;

    // coords
    int m_startPosX;
    int m_endPosX;

    int m_startPosY;
    int m_endPosY;

    // values
    float m_minValueX;
    float m_maxValueX;
    float m_valueStepX;

    float m_minValueY;
    float m_maxValueY;
    float m_valueStepY;

    std::vector<float> m_xValues;
    std::vector<float> m_yValues;

    // values to plot 
    Path m_valuesToPlot;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScaledPlot)
};
