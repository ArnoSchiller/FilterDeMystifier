/*
  ==============================================================================

    TransferFunLookAndFeel.cpp
    Created: 19 Jul 2020 10:34:34am
	
    Authors:    Jannik Hartog (JH), Arno Schiller (AS)
	
    SPDX-License-Identifier: BSD-3-Clause
  ==============================================================================
*/

#include "TransferFunLookAndFeel.h"
TransferFunLookAndFeel::TransferFunLookAndFeel()
{
	setColour(ResizableWindow::backgroundColourId, BackgroundColour);

	setColour(Slider::ColourIds::backgroundColourId, PlotBackgroundColour);
	setColour(Slider::ColourIds::thumbColourId, Colours::darkred);
	setColour(Slider::ColourIds::trackColourId, JadeGray.darker(0.2));
	setColour(Slider::ColourIds::textBoxTextColourId, JadeWhite);

	setColour(Label::ColourIds::textColourId, JadeWhite);

	setColour(TextButton::ColourIds::buttonColourId, Colours::darkred);
	setColour(TextButton::ColourIds::buttonOnColourId, Colours::darkgreen);
	setColour(TextButton::ColourIds::textColourOnId, JadeWhite);
	setColour(TextButton::ColourIds::textColourOffId, JadeWhite);

	setDefaultLookAndFeel(this);

	// drawSlider ueberschreiben, damit besser in Frame passt 
}
