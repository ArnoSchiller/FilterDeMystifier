/*
  ==============================================================================

    JadeLookAndFeel.cpp

    Authors:    Joerg Bitzer (JB)
    
    SPDX-License-Identifier: BSD-3-Clause
 ==============================================================================
*/

#include "JadeLookAndFeel.h"

JadeLookAndFeel::JadeLookAndFeel()
{
	setColour(ResizableWindow::backgroundColourId, JadeWhite);

	setColour(Slider::ColourIds::backgroundColourId, JadeGray);
	setColour(Slider::ColourIds::thumbColourId, JadeRed);
	setColour(Slider::ColourIds::trackColourId, JadeGray);
	setColour(Slider::ColourIds::textBoxTextColourId, JadeGray);

	setColour(Label::ColourIds::textColourId, JadeGray);

	setColour(TextButton::ColourIds::buttonColourId, JadeGray);
	setColour(TextButton::ColourIds::buttonOnColourId, Colours::darkgrey);
	setColour(TextButton::ColourIds::textColourOnId, JadeLightRed1);
	setColour(TextButton::ColourIds::textColourOffId, JadeWhite);
	m_fontSize = 12;

	setDefaultLookAndFeel(this);
}

void JadeLookAndFeel::drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider)
{
	juce::ignoreUnused (slider);
	auto radius = jmin(width / 2, height / 2) - 4.0f;
	auto centreX = x + width * 0.5f;
	auto centreY = y + height * 0.5f;
	auto rx = centreX - radius;
	auto ry = centreY - radius;
	auto rw = 2.0f * radius;
	auto angle = rotaryStartAngle + (rotaryEndAngle - rotaryStartAngle) * sliderPos;

	// fill
	g.setColour(JadeGray);
	g.fillEllipse(rx, ry, rw, rw);

	// outline
	Colour c1 = Colour::fromFloatRGBA(1.3*JadeRed.getFloatRed()*sliderPos,3.0*JadeRed.getFloatGreen()*sliderPos,3.0*JadeRed.getFloatBlue()*sliderPos,1.0);
	g.setColour(c1);
	//g.drawEllipse(rx, ry, rw, rw, 5.0);
	g.drawEllipse(rx, ry, rw, rw, jmax(int(width*0.07),5));

	// Point
	int PointSize = jmax(width/6,10);
	Path p;
	p.addEllipse(-PointSize / 2, -0.95 * radius, PointSize, PointSize);
	p.applyTransform(AffineTransform::rotation(angle).translated(centreX, centreY));

	g.setColour(JadeRed);
	g.fillPath(p);

}
