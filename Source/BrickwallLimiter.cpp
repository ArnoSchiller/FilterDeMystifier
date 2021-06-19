/*
  ==============================================================================

    BrickwallLimiter.cpp

    Authors:    Joerg Bitzer (JB)
    
    SPDX-License-Identifier: BSD-3-Clause
 ==============================================================================
*/

#include "BrickwallLimiter.h"

#include <cmath>
#include <iostream>
#include <fstream>
//#include <JuceHeader.h>

template <class T> BrickwallLimiter<T>::BrickwallLimiter()
:m_fs(48000.0),m_nrofchannels(2),m_Limit(1.0),m_attackTime_ms(2.0),m_releaseTime_ms(100.0),m_Gain(1.0),
m_attackCounter(0),m_state(BrickwallLimiter::State::Off),m_bypass(false)
{
    buildAndResetDelayLine();
}
template <class T> BrickwallLimiter<T>::BrickwallLimiter(T sampleRate)
:m_fs(sampleRate),m_nrofchannels(2), m_Limit(1.0),m_attackTime_ms(2.0),m_releaseTime_ms(100.0),m_Gain(1.0),
m_attackCounter(0),m_state(BrickwallLimiter::State::Off),m_bypass(false)
{
    buildAndResetDelayLine();
}

/*template <class T> void BrickwallLimiter<T>::prepareToPlay(T sampleRate, int nrofchannels)
{
    m_fs = sampleRate;
    m_nrofchannels = nrofchannels;
    buildAndResetDelayLine();
}//*/

double g_maxValLimit(1000.0);

template <class T> int BrickwallLimiter<T>::processSamples(std::vector<std::vector<T>>& data)
{
    size_t nrOfInputChannels= data.size();
    size_t nrOfSamples = data[0].size();

    for (auto kk = 0; kk < nrOfSamples; ++kk)
    {
        T maxVal = -1000.f;
        // put the input into delay and look for maximum over all channels
        for (auto cc = 0; cc < nrOfInputChannels; ++cc)
        {
            T inVal = data[cc][kk];
            if (inVal > g_maxValLimit)
                inVal = g_maxValLimit;
            
            if (inVal < -g_maxValLimit)
                inVal = -g_maxValLimit;

            m_delayline.at(cc).push(inVal);
            if (fabs(inVal)>maxVal)
                maxVal = fabs(inVal);
        }
        processOneMaxVal(maxVal);

/*        T maxValwithGain = maxVal*(m_Gain);
        T maxValafterrelease = maxValwithGain;
        int releaseSteps;
        T mgainhelp;
        switch (m_state)
        {
        case BrickwallLimiter::State::Off:
        case BrickwallLimiter::State::Att:
            break;
        case BrickwallLimiter::State::Hold:
            releaseSteps = m_delaySamples-m_holdCounter;
            // Das hier ist Mist
            mgainhelp = m_Gain;
            for (auto count = 0 ;  count <= releaseSteps; count++)
                mgainhelp = mgainhelp*m_alphaRelease + (1.0-m_alphaRelease)*1.01;

            maxValafterrelease = maxVal*(mgainhelp);
            break;
        case BrickwallLimiter::State::Rel:
            releaseSteps = m_delaySamples;
            // Das hier ist Mist
            mgainhelp = m_Gain;
            for (auto count = 0 ;  count <= releaseSteps; count++)
                mgainhelp = mgainhelp*m_alphaRelease + (1.0-m_alphaRelease)*1.01;
            
            maxValafterrelease = maxVal*(mgainhelp);
            
            break;
        
        default:
            break;
        }


        if (maxValwithGain>m_Limit)
        {
            m_state = BrickwallLimiter::State::Att;
            
            T attackIncrement = -(m_Gain-m_Gain/(maxValwithGain))/m_delaySamples;
            if (attackIncrement < m_attackIncrement)
            {
                m_attackCounter = m_delaySamples;
                m_attackIncrement = attackIncrement;
            }
            else
            {
                T nrofstepsneeded = (-(m_Gain-m_Gain/(maxValwithGain))/m_attackIncrement);
                T nrofstepsneededfrac = nrofstepsneeded - int(nrofstepsneeded);
                T addinc = -(m_Gain-m_Gain/(maxValwithGain))/(int(nrofstepsneeded));
                nrofstepsneeded = (-(m_Gain-m_Gain/(maxValwithGain))/(addinc));
                if (nrofstepsneeded>m_attackCounter)
                {
                    m_attackCounter = nrofstepsneeded;
                    m_attackIncrement = addinc;
                }
            }
        }
        else
        {
            if (maxValafterrelease > m_Limit)
            {
                m_state = BrickwallLimiter::State::Hold;
                m_holdCounter = m_delaySamples;
            }
        }
        
        switch (m_state)
        {
        case BrickwallLimiter::State::Off:
            
            break;
        case BrickwallLimiter::State::Att:
            m_Gain += m_attackIncrement;
            m_attackCounter--;
            if (m_attackCounter <= 0)
            {
                m_state = BrickwallLimiter::State::Hold;
                m_holdCounter = m_delaySamples;
                m_attackIncrement = 0.f;
            }
            break;
        case BrickwallLimiter::State::Hold:
            m_holdCounter--;
            if (m_holdCounter <= 0)            
            {
                m_state = BrickwallLimiter::State::Rel;
            }
            break;
        case BrickwallLimiter::State::Rel:
            m_Gain = m_Gain*m_alphaRelease + (1.0-m_alphaRelease)*1.01;
            if (m_Gain>=1.f)
            {
                m_state = BrickwallLimiter::State::Off;
                m_Gain = 1.f;
            }
            break;
        
        default:
            break;
        }

       //if (m_attackIncrement > 0.f)
       //     std::cout << "das darf nicht sein";
//*/
        // Get the data out of delay line and multiply with gain
        for (auto cc = 0; cc < nrOfInputChannels; ++cc)
        {
            T outVal = m_delayline.at(cc).front();
            m_delayline.at(cc).pop();
            if (m_bypass)
                data[cc][kk] = outVal;
            else
                data[cc][kk] = outVal * m_Gain;
              
        }
    }
    return 0;
}

template <class T> int BrickwallLimiter<T>::processSamples(juce::AudioBuffer<T>& data)
{
    size_t totalNrChannels = static_cast<size_t>(data.getNumChannels());
    auto readPointer = data.getArrayOfReadPointers();
    auto writePointer = data.getArrayOfWritePointers();
    size_t nrOfSamples = static_cast<size_t>(data.getNumSamples());
    for (auto kk = 0; kk < nrOfSamples; ++kk)
    {
        T maxVal = -1000.f;
        // put the input into delay and look for maximum over all channels
        for (auto cc = 0; cc < totalNrChannels; ++cc)
        {
            T inVal = readPointer[cc][kk];
            if (inVal > g_maxValLimit)
                inVal = g_maxValLimit;
            
            if (inVal < -g_maxValLimit)
                inVal = -g_maxValLimit;

            m_delayline.at(cc).push(inVal);
            if (fabs(inVal)>maxVal)
                maxVal = fabs(inVal);
        }
    
        processOneMaxVal(maxVal);
    
        // get value out of delay line and multiply with gain
        for (auto cc = 0; cc < totalNrChannels; ++cc)
        {
            T outVal = m_delayline.at(cc).front();
            m_delayline.at(cc).pop();
            if (m_bypass)
                writePointer[cc][kk] = outVal;
            else
                writePointer[cc][kk] = outVal * m_Gain;
        }
    
    
    }
    return 0;
}



template <class T> void BrickwallLimiter<T>::prepareParameter(std::unique_ptr<AudioProcessorValueTreeState>& vts)
{
    m_brickwallLimiterparamter.m_onoff = vts->getRawParameterValue(paramBrickwallLimiterOnOff.ID);
	m_brickwallLimiterparamter.m_onoffOld = paramBrickwallLimiterOnOff.defaultValue;

}
template <class T> void BrickwallLimiter<T>::buildAndResetDelayLine()
{
    m_delaySamples = int(m_attackTime_ms*0.001*m_fs + 0.5);
    m_delayline.resize(m_nrofchannels);

    for (size_t cc = 0; cc < m_delayline.size(); ++cc)
    {
        m_delayline.at(cc) = {};
        for (size_t kk = 0 ; kk < m_delaySamples-1; kk++)
            m_delayline.at(cc).push(0.f);
    }

    m_alphaRelease = exp(-1.f/(m_releaseTime_ms*0.001f*m_fs));
}

int BrickwallLimiterParameter::addParameter(std::vector < std::unique_ptr<RangedAudioParameter>>& paramVector)
{
       	paramVector.push_back(std::make_unique<AudioParameterFloat>(paramBrickwallLimiterOnOff.ID,
		paramBrickwallLimiterOnOff.name,
		NormalisableRange<float>(paramBrickwallLimiterOnOff.minValue, paramBrickwallLimiterOnOff.maxValue),
		paramBrickwallLimiterOnOff.defaultValue,
		paramBrickwallLimiterOnOff.unitName,
		AudioProcessorParameter::genericParameter,
		[](float value, int MaxLen) { return (String(1.0*int(value + 0.5), MaxLen)); },
		[](const String& text) {return text.getFloatValue(); }));
    return 0;
}

template <class T> BrickwallLimiterComponent<T>::BrickwallLimiterComponent(AudioProcessorValueTreeState& vts, BrickwallLimiter<T>& limiter)
: m_limiter(limiter), somethingChanged(nullptr),m_vts(vts),m_scaleFactor(1.f)
{

}

template <class T> void BrickwallLimiterComponent<T>::paint(Graphics& g) 
{
    // Reduction anzeige malen

}
template <class T> void BrickwallLimiterComponent<T>::resized()
{
    // oben onoff button

    // dadrunter Reduction

}
// explicit instantiations
template class BrickwallLimiter<float>;
template class BrickwallLimiter<double>;


// debug code
/*            if (!std::isfinite(data[cc][kk] ))
                {
                    switch(std::fpclassify(data[cc][kk])) 
                    {
                        case FP_INFINITE:  std::cout << ( "Inf");break;
                        case FP_NAN:       std::cout << ( "NaN");break;
                        case FP_NORMAL:    std::cout << ( "normal");break;
                        case FP_SUBNORMAL: std::cout << ( "subnormal");break;
                        case FP_ZERO:      std::cout << ( "zero");break;
                        default:           std::cout << ( "unknown");break;
                    }
                }	
            // DBG
            if (cc == 1) data[cc][kk] = m_Gain;

            if (fabs(data[cc][kk])>1.f)
                data[cc][kk] = outVal * m_Gain;
//*/  