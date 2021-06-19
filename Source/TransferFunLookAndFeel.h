/*
  ==============================================================================

    TransferFunLookAndFeel.h

    This LookAndFeel was created for the project TransferFun. 

    Authors:    Jannik Hartog (JH), Arno Schiller (AS)
    Version:    v0.0.1

    SPDX-License-Identifier: BSD-3-Clause

    Version history:
    Version:   (Author) Description:								Date:     \n
    v0.0.1     (AS) First initialize. Added colours from            19.07.2020\n
                    JadeLookAndFeel and some additional colours.              \n
 ==============================================================================
*/
#pragma once
#include <JuceHeader.h>
#define LINEWIDH_BIG 2
#define LINEWIDH_MEDIUM 1
#define LINEWIDH_SMALL 0.5

const auto JadeWhite(Colour::fromFloatRGBA(0.95, 0.95, 0.95, 1.0));
const auto JadeRed(Colour::fromFloatRGBA(0.890196078431373, 0.023529411764706, 0.074509803921569, 1.0));
const auto JadeTeal(Colour::fromFloatRGBA(0.352941176470588, 0.372549019607843, 0.337254901960784, 1.0));
const auto JadeGray(Colour::fromFloatRGBA(0.356862745098039, 0.372549019607843, 0.341176470588235, 1.0));

const auto JadeLightRed1(Colour::fromFloatRGBA(0.956862745098039, 0.631372549019608, 0.552941176470588, 1.0));
const auto JadeLightRed2(Colour::fromFloatRGBA(0.968627450980392, 0.733333333333333, 0.682352941176471, 1.0));
const auto JadeLightRed3(Colour::fromFloatRGBA(0.980392156862745, 0.807843137254902, 0.776470588235294, 1.0));

const auto BackgroundColour(JadeGray.brighter(0.5));

const auto XAxisColour(Colours::lightgreen);
const auto YAxisColour(Colours::orange);
const auto ZAxisColour(Colours::hotpink);

const auto AxisPlotColour(Colours::black);
const auto AxisTicksColour(Colours::black);
const auto AxisTextColour(Colours::black);

const auto BorderColour(JadeGray.darker(0.2));
const auto PlotColour(JadeRed);
const auto PlotBackgroundColour(BackgroundColour);

const auto PoleColour(Colours::red);
const auto PoleLightColour(Colours::orange);
const auto NeutralColour(Colours::grey.brighter(0.3));
const auto ZeroLightColour(Colours::green);
const auto ZeroColour(Colours::cyan);

const auto FontSize_small(10.0f);
const auto FontSize_medium(12.0f);
const auto FontSize_big(16.0f);

// JadeLightRed4 = [0.988235294117647 0.890196078431373 0.870588235294118];

class TransferFunLookAndFeel : public LookAndFeel_V4
{
public:

	TransferFunLookAndFeel();

private:
	//void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
	//	const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider) override;

};

