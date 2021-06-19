/*
  ==============================================================================

    JadeLookAndFeel.h

    Authors:    Joerg Bitzer (JB)
    
    SPDX-License-Identifier: BSD-3-Clause
 ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

/**
 * @brief define the Jade color scheme given by the CD 
 */
const auto JadeWhite(Colour::fromFloatRGBA(0.95, 0.95, 0.95, 1.0));
const auto JadeRed(Colour::fromFloatRGBA(0.890196078431373, 0.023529411764706, 0.074509803921569, 1.0));
const auto JadeTeal(Colour::fromFloatRGBA(0.352941176470588, 0.372549019607843, 0.337254901960784, 1.0));
const auto JadeGray(Colour::fromFloatRGBA(0.356862745098039, 0.372549019607843, 0.341176470588235, 1.0));

const auto JadeLightRed1(Colour::fromFloatRGBA(0.956862745098039, 0.631372549019608, 0.552941176470588, 1.0));
const auto JadeLightRed2(Colour::fromFloatRGBA(0.968627450980392, 0.733333333333333, 0.682352941176471, 1.0));
const auto JadeLightRed3(Colour::fromFloatRGBA(0.980392156862745, 0.807843137254902, 0.776470588235294, 1.0));
// JadeLightRed4 = [0.988235294117647 0.890196078431373 0.870588235294118];
#define MIN_COMBO_WITH_PRESET 120
#define MIN_ELEMENT_DIST_PRESET 10
#define MIN_BUTTON_WIDTH_PRESET 40
#define MIN_ELEMENT_HEIGHT_PRESET 20
// Category Lists for different usage (max number of Cat is 20);
const StringArray JadeSynthCategories("Unknown", "Lead", "Brass", "Template", "Bass",
	"Key", "Organ" , "Pad", "Drums_Perc", "SpecialEffect","Sequence", "String" );
/**
 * @brief the Look and Feel implements the rotary knob in a special way and adjust the font size according to scale
 * 
 */
class JadeLookAndFeel : public LookAndFeel_V4
{
public:
	JadeLookAndFeel();
	void setFontSize(int newFontSize){m_fontSize = newFontSize;};

private:
	int m_fontSize;
	void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
		const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider) override;

void drawButtonText (Graphics& g, TextButton& button,
		bool /*shouldDrawButtonAsHighlighted*/, bool /*shouldDrawButtonAsDown*/)
	{
		// Font font (button.getHeight () * 0.6f);
		Font font (m_fontSize);

		g.setFont (font);
		g.setColour (button.findColour (button.getToggleState () ? TextButton::textColourOnId
			: TextButton::textColourOffId)
			.withMultipliedAlpha (button.isEnabled () ? 1.0f : 0.5f));

		const int yIndent = button.proportionOfHeight (0.1f);
		const int cornerSize = jmin (button.getHeight (), button.getWidth ()) / 2;

		const int leftIndent = cornerSize / (button.isConnectedOnLeft () ?
                  yIndent * 2 : yIndent);
		const int rightIndent = cornerSize / (button.isConnectedOnRight () ? 
                  yIndent * 2 : yIndent);
		const int textWidth = button.getWidth () - leftIndent - rightIndent;

		if (textWidth > 0)
			g.drawFittedText (button.getButtonText (),
				leftIndent, yIndent, textWidth, button.getHeight () - yIndent * 2,
				Justification::centred, 2, 0.5f);
	}	
	Font getComboBoxFont (ComboBox &c)
	{
		juce::ignoreUnused (c);
		//return Font(c.getHeight () * 0.6f);
		return Font(m_fontSize);

	}	
	Font 	getPopupMenuFont ()
	{
		return Font(m_fontSize);
	}
	Font getLabelFont()
	{
		return Font(m_fontSize);
	}
	Font getLabelFont(Label &l)
	{
		l.setFont(Font(m_fontSize));
		return Font(m_fontSize);
	}


};

