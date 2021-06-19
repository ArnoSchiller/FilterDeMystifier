/*
  ==============================================================================

    SimpleMeter.cpp

    Authors:    Joerg Bitzer (JB)
    
    SPDX-License-Identifier: BSD-3-Clause
 ==============================================================================
*/
#include "SimpleMeter.h"

SimpleMeter::SimpleMeter()
:m_fs(44100.0),m_blockSize(1024),m_tauAttRMS_ms(10.0),m_tauRelRMS_ms(300.0),m_holdtime_ms(3000.0)
{

    m_rms.resize(m_maxnrofchannels);
    m_peak.resize(m_maxnrofchannels);
    m_peakholdcounter.resize(m_maxnrofchannels);
    reset();
    computeTimeConstants();
}
void SimpleMeter::prepareToPlay (float samplerate, int SamplesPerBlock)
{
    juce::ignoreUnused(SamplesPerBlock);
    m_fs = samplerate;
    m_rms.resize(m_maxnrofchannels);
    std::fill(m_rms.begin(), m_rms.end(), 0.0);
    m_peak.resize(m_maxnrofchannels);
    std::fill(m_peak.begin(), m_peak.end(), 0.0);
    m_peakholdcounter.resize(m_maxnrofchannels);
    std::fill(m_peakholdcounter.begin(), m_peakholdcounter.end(), 0.0);
    reset();
    computeTimeConstants();
}
void SimpleMeter::analyseData (juce::AudioBuffer<float>& data)
{
    size_t totalNrChannels = static_cast<size_t>(data.getNumChannels());

    if (totalNrChannels < m_maxnrofchannels)
    {
        m_rms.resize((totalNrChannels));
        m_peak.resize((totalNrChannels));
        m_peakholdcounter.resize((totalNrChannels));
    }
	for (size_t channel = 0; channel < totalNrChannels; ++channel)
	{
        auto* channelData = data.getWritePointer (channel);

        for (auto idx = 0; idx < data.getNumSamples(); ++idx)
        {
            double in = fabs(channelData[idx]);
            /*if (in < 0.0)
                in = -0.38;
            else
                in = 0.38;// testing only == just above -9 dB*/
            // RMS first
            float alpha;

            if (in > m_rms[channel])
                alpha = m_alphaAttRMS;
            else
                alpha = m_alphaRelRMS;

            m_rms[channel] *= alpha;
            m_rms[channel] += (1.0-alpha)*in*in;

            // peak
            if (in > m_peak[channel])
            {
                m_peak[channel] = in;
                m_peakholdcounter[channel] = m_holdtime_samples;
            }
            else
            {
                m_peakholdcounter[channel]--;
                if (m_peakholdcounter[channel] < 0)
                    m_peak[channel] *= m_alphaRelRMS;

            }
            
        }
	}


}
void SimpleMeter::computeTimeConstants()
{
    m_alphaAttRMS = exp(-1.0/(m_tauAttRMS_ms*0.001*m_fs));
    m_alphaRelRMS = exp(-1.0/(m_tauRelRMS_ms*0.001*m_fs));

    m_holdtime_samples = static_cast<int>(m_holdtime_ms*0.001*m_fs);

}
void SimpleMeter::reset()
{
    std::fill(m_rms.begin(), m_rms.end(), 0.0);
    std::fill(m_peak.begin(), m_peak.end(), 0.0);
    std::fill(m_peakholdcounter.begin(), m_peakholdcounter.end(), 0.0);
}

int SimpleMeter::getAnalyserData(std::vector<float>& rms, std::vector<float>& peak)
{
    size_t nrofchannels = m_rms.size();
    rms.resize(nrofchannels);
    peak.resize(nrofchannels);
    for (size_t kk = 0 ; kk < nrofchannels; ++kk)
    {
        rms[kk] = m_rms[kk];
        peak[kk] = m_peak[kk];
    }
    return nrofchannels;
}


SimpleMeterComponent::SimpleMeterComponent(SimpleMeter &meter)
:m_scaleFactor(1.f),m_meter(meter),m_displaychannels(2)
{
    m_rms.resize(8);
    m_peak.resize(8);
    std::fill(m_rms.begin(), m_rms.end(), 0.0);
    std::fill(m_peak.begin(), m_peak.end(), 0.0);

    startTimer (40);
}
void SimpleMeterComponent::paint(Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId).darker(0.2));

    int w = getWidth();
    int h = getHeight();

    float normSizey = 106.0;
    float normSizex = 42.0;
    float border = 3.0;
    float displaySizeNorm;
    float maxvalPeakOrRMS = 6.0;
    float redfrom = 0.0;
    float yellowfrom = -9.0;
    float greenfrom = -60.0;
    float rednormpixels = 20.0;
    float yellownormpixels = 30.0;
    // float greennormpixels = 50.0;
    float scalechangePixels = 60.0;
    float scale3dbFrom = -12.0;
    float peakdisplaywidth = 3.0;
    
    

  	auto r = getLocalBounds();

    if (w>h) // horizontal meter
    {
        normSizex = 106.0;
        normSizey = 42.0;
        float ScaleFactory = h/normSizey;
        float ScaleFactorx = w/normSizex;
        displaySizeNorm = normSizex- 2.0*border;
        border *= ScaleFactorx;
        r.reduce(border, border);
        float remindingpixels = normSizey*ScaleFactory - (2.0+m_displaychannels-1.0)*border;
        float heightperchn = remindingpixels/m_displaychannels;
        for (size_t kk = 0; kk<m_displaychannels; kk++)
        {
            Colour g1 = juce::Colours::green;
            g.setColour(g1.darker(0.95));
            g.fillRect(float (r.getX()),float(r.getY())+ kk*(heightperchn+border),(w-2*border)*0.5  ,heightperchn);

            Colour y1 = juce::Colours::yellow ;
            g.setColour(y1.darker(0.95));
            g.fillRect(float (r.getX())+ (w-2*border)*0.5,float(r.getY())+ kk*(heightperchn+border),(w-2*border)*0.3  ,heightperchn);

            Colour r1 = juce::Colours::red;
            g.setColour(r1.darker(0.95));
            g.fillRect(float (r.getX())+ (w-2*border)*0.8 ,float(r.getY())+ kk*(heightperchn+border),(w-2*border)*0.2,heightperchn);

            // draw Peak 
            float peaklog = 20*log10(m_peak[kk] + 0.00000000001);
            if (peaklog >= maxvalPeakOrRMS)
                peaklog = maxvalPeakOrRMS;
            
            if (peaklog>greenfrom)
            {
                Colour peakcolor;
                if (peaklog >= redfrom)
                    peakcolor = r1;
                else 
                {
                    if (peaklog < redfrom && peaklog > yellowfrom)
                        peakcolor = y1;
                    else
                        peakcolor = g1;
                }    
                g.setColour(peakcolor.brighter(0.5));
                
                if (peaklog > scale3dbFrom)
                {
                    float normscalepos = scalechangePixels - peakdisplaywidth;
                    float xpos = normscalepos- normscalepos/18.0*(peaklog-6.0)*(-1.0);
                    g.fillRect(float (r.getX())+ (w-2*border)*0.4 + xpos*ScaleFactorx ,float(r.getY())+ kk*(heightperchn+border) ,peakdisplaywidth*ScaleFactorx  ,heightperchn);
                }
                else
                {
                    float normscalepos = displaySizeNorm-scalechangePixels; // 
                    //float ypos = normscalepos/48.0*(peaklog+12.0)*(-1.0)+1.0;
                    float xpos = normscalepos-normscalepos/48.0*(peaklog+12.0)*(-1.0);
                    g.fillRect(float (r.getX())+ xpos*ScaleFactorx ,float(r.getY())+ kk*(heightperchn+border),peakdisplaywidth*ScaleFactorx ,heightperchn);
                }
            }
            
            // draw RMS 
            float rmslog = 10*log10(m_rms[kk] + 0.00000000001); // -110dB
            if (rmslog>maxvalPeakOrRMS)
                rmslog = maxvalPeakOrRMS;
           
            if (rmslog >= greenfrom)
            {
                g.setColour(g1);
                float normscalepos;
                float xpos;
                if (rmslog < yellowfrom)
                {
                    
                    if (rmslog >= scale3dbFrom)
                    {
                        normscalepos = scalechangePixels - yellownormpixels-rednormpixels;
                        xpos = normscalepos-normscalepos/3.0*(rmslog+9.0)*(-1.0);    
                        xpos += (displaySizeNorm-scalechangePixels);         
                    }
                    else
                    {
                        normscalepos = displaySizeNorm-scalechangePixels;
                        xpos = normscalepos-normscalepos/48.0*(rmslog+12.0)*(-1.0) ;                
                    }
                    g.fillRect(float (r.getX()),float(r.getY())+ kk*(heightperchn+border) ,xpos*ScaleFactorx,heightperchn);
                }
                
                else
                {
                    g.fillRect(float (r.getX()),float(r.getY())+ kk*(heightperchn+border),(w-2*border)*0.5  ,heightperchn);

                    g.setColour(y1);
                    if (rmslog < redfrom)
                    {
                        normscalepos = yellownormpixels;
                        xpos = normscalepos-normscalepos/9.0*(rmslog)*(-1.0);                
                        g.fillRect(float (r.getX()) + (w-2*border)*0.5,float(r.getY())+ kk*(heightperchn+border) ,xpos*ScaleFactorx,heightperchn);
                    }
                    else
                    {
                        g.fillRect(float (r.getX())+ (w-2*border)*0.5,float(r.getY())+ kk*(heightperchn+border),(w-2*border)*0.3  ,heightperchn);
                        g.setColour(r1);
                        // red part
                        normscalepos = rednormpixels;
                        xpos = normscalepos/6.0*(rmslog);                
                        g.fillRect(float (r.getX()) + (w-2*border)*0.8,float(r.getY())+ kk*(heightperchn+border) ,xpos*ScaleFactorx,heightperchn);
                    }
                    
                }
            }
        }
    }
    else // vertical meter
    {
        float ScaleFactory = h/normSizey;
        float ScaleFactorx = w/normSizex;
        displaySizeNorm = normSizey- 2.0*border;
        border *= ScaleFactory;
        r.reduce(border, border);
        float remindingpixels = normSizex*ScaleFactorx - (2.0+m_displaychannels-1.0)*border;
        float widthperchn = remindingpixels/m_displaychannels;
        // draw channels background
        for (size_t kk = 0; kk<m_displaychannels; kk++)
        {
            Colour r1 = juce::Colours::red;
            g.setColour(r1.darker(0.95));
            g.fillRect(float (r.getX()) + kk*(widthperchn+border),float(r.getY()),widthperchn,(h-2*border)*0.2);


            Colour y1 = juce::Colours::yellow ;
            g.setColour(y1.darker(0.95));
            g.fillRect(float (r.getX())+ kk*(widthperchn+border),float(r.getY()) + (h-2*border)*0.2 ,widthperchn,(h-2*border)*0.3);

            Colour g1 = juce::Colours::green;
            g.setColour(g1.darker(0.95));
            g.fillRect(float (r.getX())+ kk*(widthperchn+border),float(r.getY()) + (h-2*border)*0.5 ,widthperchn,(h-2*border)*0.5);
        
            // draw Peak 
            float peaklog = 20*log10(m_peak[kk] + 0.00000000001);
            if (peaklog >= maxvalPeakOrRMS)
                peaklog = maxvalPeakOrRMS;

            if (peaklog>greenfrom)
            {
                Colour peakcolor;
                if (peaklog >= redfrom)
                    peakcolor = r1;
                else 
                {
                    if (peaklog < redfrom && peaklog > yellowfrom)
                        peakcolor = y1;
                    else
                        peakcolor = g1;
                }    
                g.setColour(peakcolor.brighter(0.5));
                
                if (peaklog > scale3dbFrom)
                {
                    float normscalepos = scalechangePixels;
                    float ypos = normscalepos/18.0*(peaklog-6.0)*(-1.0);
                    g.fillRect(float (r.getX())+ kk*(widthperchn+border),float(r.getY()) +ypos*ScaleFactory   ,widthperchn, peakdisplaywidth*ScaleFactory);
                }
                else
                {
                    float normscalepos = displaySizeNorm-scalechangePixels- peakdisplaywidth; // 40-3 for the drawing
                    //float ypos = normscalepos/48.0*(peaklog+12.0)*(-1.0)+1.0;
                    float ypos = normscalepos/48.0*(peaklog+12.0)*(-1.0);
                    g.fillRect(float (r.getX())+ kk*(widthperchn+border),float(r.getY()) +60.0*ScaleFactory +ypos*ScaleFactory   ,widthperchn, peakdisplaywidth*ScaleFactory);
                }
            }
            // draw RMS 
            float rmslog = 10*log10(m_rms[kk] + 0.00000000001); // -110dB
            if (rmslog>maxvalPeakOrRMS)
                rmslog = maxvalPeakOrRMS;
           
            if (rmslog >= greenfrom)
            {
                g.setColour(g1);
                float normscalepos;
                float ypos;
                if (rmslog < yellowfrom)
                {
                    //float startvaly = 51;
                    if (rmslog >= scale3dbFrom)
                    {
                        normscalepos = scalechangePixels - yellownormpixels-rednormpixels;
                        ypos = normscalepos/3.0*(rmslog+9.0)*(-1.0);                
                    }
                    else
                    {
                        normscalepos = displaySizeNorm-scalechangePixels;
                        ypos = normscalepos/48.0*(rmslog+12.0)*(-1.0) + 10.0;                
                    }
                    
                
                    g.fillRect(float (r.getX())+ kk*(widthperchn+border),float(r.getY()) + (h-2*border)*0.5 + ypos*ScaleFactory ,widthperchn,(h-2*border)*0.5-ypos*ScaleFactory);
                }
                else
                {
                    g.fillRect(float (r.getX())+ kk*(widthperchn+border),float(r.getY()) + (h-2*border)*0.5 ,widthperchn,(h-2*border)*0.5);
                    g.setColour(y1);
                    if (rmslog < redfrom)
                    {
                        normscalepos = yellownormpixels;
                        ypos = normscalepos/9.0*(rmslog)*(-1.0);                
                        g.fillRect(float (r.getX())+ kk*(widthperchn+border),float(r.getY()) + (h-2*border)*0.2 + ypos*ScaleFactory ,widthperchn,(h-2*border)*0.3-ypos*ScaleFactory);

                    }
                    else
                    {
                        g.fillRect(float (r.getX())+ kk*(widthperchn+border),float(r.getY()) + (h-2*border)*0.2 ,widthperchn,(h-2*border)*0.3);
                        g.setColour(r1);
                        // red part
                        normscalepos = rednormpixels;
                        ypos = normscalepos-normscalepos/6.0*(rmslog);                
                        g.fillRect(float (r.getX())+ kk*(widthperchn+border),float(r.getY()) + ypos*ScaleFactory ,widthperchn,(h-2*border)*0.2-ypos*ScaleFactory);

                    }
                    
                }
            }


        }
    }
}    

void SimpleMeterComponent::resized()
{
    
}


