/*
  ==============================================================================
	PNParameter.h

	This class contains every used parameter of the pole zero plane. 

	Authors:    Jannik Hartog (JH), Arno Schiller (AS)
	Version:    v0.0.3
	
    SPDX-License-Identifier: BSD-3-Clause

	Version history:
	Version:   (Author) Description:								Date:     \n
	v0.0.1     (AS) First initialize. Added parameters for imag     13.07.2020\n
					and real values for each 4 pole and zeros.                \n
	v0.0.2     (AS) Added bool value to each pole and zero to       14.07.2020\n
					change its status.                                        \n
	v0.0.3     (AS) Added parameter for b0 coefficient.             23.07.2020\n
 ===============================================================================
*/
#pragma once
#include <vector>
#include <JuceHeader.h>

#define MAX_POLE_INSTANCES 4 	// error using > 4
#define MAX_ZERO_INSTANCES 4

const struct
{
	const std::string ID[MAX_POLE_INSTANCES] = { "Pole1Imag", "Pole2Imag", "Pole3Imag", "Pole4Imag" };
	std::string name = "Imaginary part"; 
	std::string unitName = "";
	float minValue = 0.0; //-1.5;
	float maxValue = 1.5;
	float defaultValue = 0.0;
}paramPoleImag;

const struct
{
	const std::string ID[MAX_POLE_INSTANCES] = { "Pole1Real", "Pole2Real", "Pole3Real", "Pole4Real" };
	std::string name = "Real part";
	std::string unitName = "";
	float minValue = -1.5;
	float maxValue = 1.5;
	float defaultValue = 0.0;
}paramPoleReal;

const struct
{
	const std::string ID[MAX_POLE_INSTANCES] = { "Pole1Conj", "Pole2Conj", "Pole3Conj", "Pole4Conj" };
	std::string name = "is conjugated";
	std::string unitName = "";
	bool defaultValue = false;
}paramPoleConjugated;

const struct
{
	const std::string ID[MAX_POLE_INSTANCES] = { "Pole1Bool", "Pole2Bool", "Pole3Bool", "Pole4Bool" };
	std::string name = "is activated";
	std::string unitName = "";
	bool defaultValue = false;
}paramPoleBool;


const struct
{
	const std::string ID[MAX_ZERO_INSTANCES] = { "Zero1Imag", "Zero2Imag", "Zero3Imag", "Zero4Imag" };
	std::string name = "Imaginary part";
	std::string unitName = "";
	float minValue = 0.0; //-1.5;
	float maxValue = 1.5;
	float defaultValue = 0.0;
}paramZeroImag;

const struct
{
	const std::string ID[MAX_ZERO_INSTANCES] = { "Zero1Real", "Zero2Real", "Zero3Real", "Zero4Real" };
	std::string name = "Real part";
	std::string unitName = "";
	float minValue = -1.5;
	float maxValue = 1.5;
	float defaultValue = 0.0;
}paramZeroReal;

const struct
{
	const std::string ID[MAX_POLE_INSTANCES] = { "Zero1Conj", "Zero2Conj", "Zero3Conj", "Zero4Conj" };
	std::string name = "is conjugated";
	std::string unitName = "";
	bool defaultValue = false;
}paramZeroConjugated;

const struct
{
	const std::string ID[MAX_POLE_INSTANCES] = { "Zero1Bool", "Zero2Bool", "Zero3Bool", "Zero4Bool" };
	std::string name = "is activated";
	std::string unitName = "";
	bool defaultValue = false;
}paramZeroBool;

const struct
{
	const std::string ID = "b0Value";
	std::string name = "g";
	std::string unitName = " dB";
	float minValue = -90.0f;
	float maxValue = 20.0f;
	float defaultValue = 0.0f;
}paramb0;

const struct
{
	const std::string ID = "limiterBool";
	std::string name = "is activated";
	std::string unitName = "";
	bool defaultValue = true;
}paramLimiterBool;

const struct
{
	const std::string ID = "poleProtectBool";
	std::string name = "is activated";
	std::string unitName = "";
	bool defaultValue = true;
}paramPoleProtectBool;

#define VALUE_STEP 0.001
class PNParameter
{
public:
	int maxNrOfPole = MAX_POLE_INSTANCES;
	int maxNrOfZero = MAX_ZERO_INSTANCES;
	int addParameter(std::vector<std::unique_ptr<RangedAudioParameter>>& paramVector)
	{
		for(auto kk = 0U; kk < MAX_POLE_INSTANCES;++kk)
		{
			paramVector.push_back(std::make_unique<AudioParameterFloat>(paramPoleReal.ID[kk],
				paramPoleReal.name,
				NormalisableRange<float>(paramPoleReal.minValue, paramPoleReal.maxValue, VALUE_STEP),
				paramPoleReal.defaultValue,
				paramPoleReal.unitName,
				AudioProcessorParameter::genericParameter,
				[](float value, int MaxLen) {value = int(value * 1000) * 0.001; return String("Re=") + String(value, MaxLen);},
				[](const String& text) {return text.getFloatValue();} ));

			paramVector.push_back(std::make_unique<AudioParameterFloat>(paramPoleImag.ID[kk],
				paramPoleImag.name,
				NormalisableRange<float>(paramPoleImag.minValue, paramPoleImag.maxValue, VALUE_STEP),
				paramPoleImag.defaultValue,
				paramPoleImag.unitName,
				AudioProcessorParameter::genericParameter,
				[](float value, int MaxLen) {value = int(value * 1000) * 0.001; return String("Im=") + String(value, MaxLen);},
				[](const String& text) {return text.getFloatValue();}));

			paramVector.push_back(std::make_unique<AudioParameterBool>(paramPoleConjugated.ID[kk],
				paramPoleConjugated.name,
				paramPoleConjugated.defaultValue,
				paramPoleConjugated.unitName,
				[](bool value, int MaxLen) {if (value) return String("true"); else return String("false");},
				[](const String& text) {if (text.compareIgnoreCase("true") == 0) return true; else return false;}));

			paramVector.push_back(std::make_unique<AudioParameterBool>(paramPoleBool.ID[kk],
				paramPoleBool.name,
				paramPoleBool.defaultValue,
				paramPoleBool.unitName,
				[](bool value, int MaxLen) {if (value) return String("true"); else return String("false");},
				[](const String& text) {if (text.compareIgnoreCase("true") == 0) return true; else return false;}));
		}
		for (auto kk = 0U; kk < MAX_ZERO_INSTANCES;++kk)
		{
			paramVector.push_back(std::make_unique<AudioParameterFloat>(paramZeroReal.ID[kk],
				paramZeroReal.name,
				NormalisableRange<float>(paramZeroReal.minValue, paramZeroReal.maxValue, VALUE_STEP),
				paramZeroReal.defaultValue,
				paramZeroReal.unitName,
				AudioProcessorParameter::genericParameter,
				[](float value, int MaxLen) {value = int(value * 1000) * 0.001; return String("Re=") + String(value, MaxLen);},
				[](const String& text) {return text.getFloatValue();}));

			paramVector.push_back(std::make_unique<AudioParameterFloat>(paramZeroImag.ID[kk],
				paramZeroImag.name,
				NormalisableRange<float>(paramZeroImag.minValue, paramZeroImag.maxValue, VALUE_STEP),
				paramZeroImag.defaultValue,
				paramZeroImag.unitName,
				AudioProcessorParameter::genericParameter,
				[](float value, int MaxLen) {value = int(value * 1000) * 0.001; return String("Im=") + String(value, MaxLen);},
				[](const String& text) {return text.getFloatValue();}));
			
			paramVector.push_back(std::make_unique<AudioParameterBool>(paramZeroConjugated.ID[kk],
				paramZeroConjugated.name,
				paramZeroConjugated.defaultValue,
				paramZeroConjugated.unitName,
				[](bool value, int MaxLen) {if (value) return String("true"); else return String("false");},
				[](const String& text) {if (text.compareIgnoreCase("true") == 0) return true; else return false;}));

			paramVector.push_back(std::make_unique<AudioParameterBool>(paramZeroBool.ID[kk],
				paramZeroBool.name,
				paramZeroBool.defaultValue,
				paramZeroBool.unitName,
				[](bool value, int MaxLen) {if (value) return String("true"); else return String("false");},
				[](const String& text) {if (text.compareIgnoreCase("true") == 0) return true; else return false;}));
				//*/
		}
		paramVector.push_back(std::make_unique<AudioParameterFloat>(paramb0.ID,
			paramb0.name,
			NormalisableRange<float>(paramb0.minValue, paramb0.maxValue, VALUE_STEP),
			paramb0.defaultValue,
			paramb0.unitName,
			AudioProcessorParameter::genericParameter,
			[](float value, int MaxLen) 
			{value = static_cast<float>(int(value * 100)) * 0.01; return String(value, MaxLen) + " dB";},
			[](const String& text) {return text.getFloatValue();}));

		paramVector.push_back(std::make_unique<AudioParameterBool>(paramLimiterBool.ID,
				paramLimiterBool.name,
				paramLimiterBool.defaultValue,
				paramLimiterBool.unitName,
				[](bool value, int MaxLen) {if (value) return String("true"); else return String("false");},
				[](const String& text) {if (text.compareIgnoreCase("true") == 0) return true; else return false;}));

		paramVector.push_back(std::make_unique<AudioParameterBool>(paramPoleProtectBool.ID,
				paramPoleProtectBool.name,
				paramPoleProtectBool.defaultValue,
				paramPoleProtectBool.unitName,
				[](bool value, int MaxLen) {if (value) return String("true"); else return String("false");},
				[](const String& text) {if (text.compareIgnoreCase("true") == 0) return true; else return false;}));
		return 1;
	};
};

