/*
  ==============================================================================
    SOSFilter.h

    This template class is a second order section filter, It provides clickfree time variant processing
	the smoothing is done via blending not via morphing (fast, but sonically sub-optimal)

    Authors:    Joerg Bitzer (JB)
    Version:    1.0
    Date:       2021-02-21
    
    SPDX-License-Identifier: BSD-3-Clause
 ==============================================================================
*/
/* ToDO:
	1) non lienarities with different Forms (clip, tanh)
	2) In terms of complexitiy it would be better to decide for NL processing outside by calling different functions
	   processDataNL and processDataTVNL should be designed
	3) Think about SSE
//*/

#pragma once
#include <vector>
template <class T> class SOSFilter
{
public:
    SOSFilter(){m_b0 = 1.0; m_b1 = 0.0; m_b2 = 0.0; m_a1 = 0.0; m_a2 = 0.0;
        m_b0Old = 1.0; m_b1Old = 0.0; m_b2Old = 0.0; m_a1Old = 0.0; m_a2Old = 0.0;
   	    m_newCoeffs = false; setXFadeSamples(30); m_useNL = true; m_clipVal = 3.f;
        reset(); };
    SOSFilter(T b0, T b1, T b2, T a1, T a2){m_b0 = b0; m_b1 = b1; m_b2 = b2; m_a1 = a1; m_a2 = a2;
        m_b0Old = b0; m_b1Old = b1; m_b2Old = b2; m_a1Old = a1; m_a2Old = a2;
   	    m_newCoeffs = false; setXFadeSamples(30); m_useNL = true; m_clipVal = 3.f;
        reset(); };

    void reset(){
        m_statea1 = 0.0; m_statea2 = 0.0; m_stateb1 = 0.0; m_stateb2 = 0.0;
        m_statea1Old = 0.0; m_statea2Old = 0.0;};

	int setCoeffs(T b0, T b1, T b2, T a1, T a2){
        m_b0Old = m_b0; m_b1Old = m_b1; m_b2Old = m_b2; m_a1Old = m_a1; m_a2Old = m_a2;
        m_statea1Old = m_statea1; m_statea2Old = m_statea2;
        m_b0 = b0; m_b1 = b1; m_b2 = b2; m_a1 = a1; m_a2 = a2;
        m_newCoeffs = true; m_xFadeCounter = 0; m_CrossGain = 0.0; return 0;};
	
	int processData(std::vector<T>& in, std::vector<T>& out)
    {
	    if (in.size() != out.size())
		    return -1;

	    for (unsigned int kk = 0; kk < in.size(); kk++)
	    {
            T curSample = in[kk];
			out[kk] = m_b0*curSample + m_b1*m_stateb1 + m_b2*m_stateb2 - m_a1*m_statea1 - m_a2*m_statea2;
			// non linearities for instable filters
			if (m_useNL)
			{
				
				if (out[kk]>m_clipVal)
					out[kk] = m_clipVal;
				if (out[kk]<-m_clipVal)
					out[kk] = -m_clipVal;
			}
			m_statea2 = m_statea1;
            m_statea1 = out[kk];
            m_stateb2 = m_stateb1;
            m_stateb1 = curSample;
    	}

	    return 0;
    };
	int processDataTV(std::vector<T>& in, std::vector<T>& out)
    {
	    if (in.size() != out.size())
		    return -1;

	    if (m_newCoeffs == false)
	    {
		    processData(in, out);
	    }
	    else // TV cross fade audio
	    {
		    for (unsigned int kk = 0; kk < in.size(); kk++)
		    {
			    T newOut = 0.0;
			    T oldOut = 0.0;
                T curSample = in[kk];
			    newOut = m_b0*curSample + m_b1*m_stateb1 + m_b2*m_stateb2 ;
                oldOut = m_b0Old*curSample + m_b1Old*m_stateb1 + m_b2Old*m_stateb2 ;
    			newOut -= (m_a1*m_statea1 + m_a2*m_statea2);
    			oldOut -= (m_a1Old*m_statea1Old + m_a2Old*m_statea2Old);

				// non linearities for instable filters
				if (m_useNL)
				{

					if (newOut>m_clipVal)
						newOut = m_clipVal;
					if (newOut<-m_clipVal)
						newOut = -m_clipVal;

					if (oldOut>m_clipVal)
						oldOut = m_clipVal;
					if (oldOut<-m_clipVal)
						oldOut = -m_clipVal;

				}

                m_statea2 = m_statea1;
                m_statea1 = newOut;
                m_statea2Old = m_statea1Old;
                m_statea1Old = oldOut;
                m_stateb2 = m_stateb1;
                m_stateb1 = curSample;
			    if (m_xFadeCounter < m_xFadeTimeSamples)
			    {
				    out[kk] = (1.0 - m_CrossGain) * oldOut + m_CrossGain * newOut;
				    m_CrossGain += m_StepSize;
				    m_xFadeCounter++;
			    }
			    else
			    {
				    out[kk] = newOut;
				    m_newCoeffs = false;
			    }
		}
		
	}

	return 0;        
    }
	int setXFadeSamples(int nrofsamples)
    {
        m_xFadeTimeSamples = nrofsamples;
	    m_xFadeCounter = 0;
	    m_StepSize = 1.0 / (m_xFadeTimeSamples - 1);
	    m_CrossGain = 0.0;
	    return 0;
    }

	void setClipValue (T clipval){m_clipVal = clipval;};
private:
    T m_b0,m_b1,m_b2;
    T m_a1,m_a2;

    T m_stateb1,m_stateb2;
    T m_statea1,m_statea2;

	bool m_newCoeffs;
	int m_xFadeCounter;
	int m_xFadeTimeSamples;
	T m_CrossGain;
	T m_StepSize;

    T m_b0Old,m_b1Old,m_b2Old;
    T m_a1Old,m_a2Old;
    T m_statea1Old,m_statea2Old;

	bool m_useNL;
	T m_clipVal;
};
