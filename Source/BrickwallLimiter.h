/*
  ==============================================================================

    BrickwallLimiter.h

    Authors:    Joerg Bitzer (JB)
    
    SPDX-License-Identifier: BSD-3-Clause
 ==============================================================================
*/

#pragma once

#include <vector>
#include <queue>
#include <JuceHeader.h>

class BrickwallLimiterParameter
{
public:
	int addParameter(std::vector < std::unique_ptr<RangedAudioParameter>>& paramVector);

public:
    std::atomic<float>* m_onoff;
    float m_onoffOld;
};


template <class T> class BrickwallLimiter
{
public:
    BrickwallLimiter();
    BrickwallLimiter(T sampleRate);
    void prepareParameter(std::unique_ptr<AudioProcessorValueTreeState>& vts);
    void prepareToPlay(T sampleRate, int nrofchannels){
    m_fs = sampleRate;
    m_nrofchannels = nrofchannels;
    buildAndResetDelayLine();
    };
    int processSamples(std::vector<std::vector<T>>& data);
    int processSamples(juce::AudioBuffer<T>& data);

    void setGainLimit(T newLimit){m_Limit = newLimit;};
    void setReleaseTime(T releaseTime_ms){m_releaseTime_ms = releaseTime_ms; m_alphaRelease = exp(-1.f/(m_releaseTime_ms*0.001f*m_fs));};
    void setSampleRate(T samplerate){m_fs = samplerate;m_alphaRelease = exp(-1.f/(m_releaseTime_ms*0.001f*m_fs));};
    T getReduction(){return m_curReduction;};
    int getDelaySamples(){return m_delaySamples;};
    T getReduction_db(){return 20.0*log10(m_Gain+0.00000001);};
    void setBypass(bool bypass){m_bypass = bypass;};
private:
    enum class State
    {
        Off,
        Att,
        Hold,
        Rel
    };

    T m_fs;
    int m_nrofchannels;
    T m_Limit;
    T m_curReduction;
    int m_delaySamples;
    T m_releaseTime_ms;
    T m_alphaRelease;
    std::vector<std::queue <T>> m_delayline;
    T m_attackTime_ms;
    T m_Gain;
    int m_attackCounter;
    int m_holdCounter;
    bool m_bypass;
    T m_attackIncrement;
    BrickwallLimiter::State m_state;
    void buildAndResetDelayLine();
    BrickwallLimiterParameter m_brickwallLimiterparamter;
    inline void processOneMaxVal(T maxVal)
    {
        T maxValwithGain = maxVal*(m_Gain);
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
    };

};

const struct
{
	const std::string ID = "BLOnOff";
	std::string name = "OnOff";
	std::string unitName = "";
	float minValue = 0.f;
	float maxValue = 1.f;
	float defaultValue = 1.f;
}paramBrickwallLimiterOnOff;

template <class T> class BrickwallLimiterComponent : public Component, public Timer
{
public:
	BrickwallLimiterComponent(AudioProcessorValueTreeState& , BrickwallLimiter<T> & );

	void paint(Graphics& g) override;
	void resized() override;
    std::function<void()> somethingChanged;
    void setScaleFactor(float newscale){m_scaleFactor = newscale;};
    void timerCallback() override
    {
        m_reduction = m_limiter.getReduction_db();
        repaint();
    };
private:
    AudioProcessorValueTreeState& m_vts; 
    BrickwallLimiter<T>& m_limiter;
    ToggleButton m_onoff;
    float m_reduction;
    float m_scaleFactor;

};

// BrickwallLimiter<float> blf;
// BrickwallLimiter<double> bld;

