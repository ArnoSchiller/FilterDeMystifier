/*
  ==============================================================================

    TransferFun3DComponent.h

    This class contains the 3D plot of the transfer function in the z plane. 

    Authors:    Jannik Hartog (JH), Arno Schiller (AS)
    Version:    v0.0.3

    SPDX-License-Identifier: BSD-3-Clause

    Version history:
    Version:   (Author) Description:								Date:     \n
    v0.0.1     (AS) First initialize. Added function to calculate   25.07.2020\n
                    the transfer function.                                    \n
    v0.0.2     (JH) Included OpenGLAppComponent to create 3D plot.  25.07.2020\n
    v0.0.3     (AS) Added function to write calculated transfer     25.07.2020\n
                    function to txt file.                                     \n
    v0.0.4     (AS) Added axis to txt file which describes the      26.07.2020\n
                    3D plot shape.                                            \n
    v0.1.0     (AS) Included OpenGLAppClass with points matrix.     04.03.2021\n
                    Parsing matrix into a shape is possible                   \n
 ==============================================================================
*/

#pragma once
#include <iostream>
#include <complex>
#include <JuceHeader.h>

#include "OpenGLAppClass.h"
#include "TransferFunLookAndFeel.h"
#include "PNParameter.h"


//==============================================================================
/*
*/
class TransferFun3DComponent : public Component
{
public:
    TransferFun3DComponent(AudioProcessorValueTreeState& vts)
        : m_maxValueZ(50), m_minValueZ(-50), m_valueStepZ(20),
        m_vts(vts), m_dirName("Resources"), m_fileName("TransferFun3DShape.txt"),
        m_numberOfSamples(150), m_appClass(-50,50, m_valuesToPlot)
    {
        // In your constructor, you should add any child components, and
        // initialise any special settings that your component needs.
        m_maxReal = 1.5; // std::min(paramPoleReal.maxValue, paramZeroReal.maxValue);
        m_minReal = -1.5; //std::max(paramPoleReal.minValue, paramZeroReal.minValue);
        m_maxImag = 1.5; // std::min(paramPoleImag.maxValue, paramZeroImag.maxValue);
        m_minImag = -1.5; // std::max(paramPoleImag.minValue, paramZeroImag.minValue);

        m_appClass.updateData = [this]() {updateData();};
        m_appClass.updateData();

        addAndMakeVisible(m_appClass);
    }

    ~TransferFun3DComponent()
    {
        /*
        auto dir = juce::File::getCurrentWorkingDirectory();
        int numTries = 0;
        while (!dir.getChildFile("Resources").exists() && numTries++ < 15)
            dir = dir.getParentDirectory();
        if (dir.getChildFile("Resources").getChildFile("TransferFun3DShape.txt").exists())
            dir.getChildFile("Resources").getChildFile("TransferFun3DShape.txt").deleteFile();
        */
    }

    void paint(Graphics& g) override
    {
        g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));   // clear the background
    }

    void resized() override
    {
        // This method is where you should set the bounds of any child
        // components that your component contains..
        m_appClass.setBounds(getLocalBounds());
    }

    void setSomethingChanged()
    {
        m_appClass.setSomethingChanged();
    }

    void updateData()
    {
        updateDataFromPoleZero(m_valuesToPlot);
        normalizeValues(m_valuesToPlot, m_valuesToPlot);
        // update values from OpenGLAppCLass
        m_appClass.m_pointsMatrix = m_valuesToPlot;
        // writeDataToFile(m_valuesToPlot);
        
    }

private:

    OpenGLAppClass m_appClass;

    AudioProcessorValueTreeState& m_vts;

    PNParameter m_PNparam;
    int m_numOfPole = m_PNparam.maxNrOfPole;
    int m_numOfZero = m_PNparam.maxNrOfZero;

    int m_numberOfSamples;

    std::vector<std::vector<float>> m_valuesToPlot;

    float m_minValueZ;
    float m_maxValueZ;
    float m_valueStepZ;

    float m_minImag;
    float m_maxImag;
    float m_minReal;
    float m_maxReal;

    std::vector<std::complex<float>> m_poles;
    std::vector<std::complex<float>> m_zeros;

    std::string m_dirName;
    std::string m_fileName;

    void updateDataFromPoleZero(std::vector<std::vector<float>>& h)
    {
        float b0 = *(m_vts.getRawParameterValue(paramb0.ID));
        b0 = pow(10, (b0/20));       // from dB to linear
        if (b0 == 0)
        {
            h.resize(m_numberOfSamples);

            for (auto kk = 0; kk < m_numberOfSamples; ++kk)
            {
                h[kk].resize(m_numberOfSamples);
                for (auto jj = 0; jj < m_numberOfSamples; ++jj)
                    h[kk][jj] = 0;
            }
        }
        else
        {
            updatePoleAndZero();

            float realStep = (m_maxReal - m_minReal) / (m_numberOfSamples - 1);
            float imagStep = (m_maxImag - m_minImag) / (m_numberOfSamples - 1);

            h.resize(m_numberOfSamples);

            for (auto kk = 0; kk < m_numberOfSamples; ++kk)
            {
                h[kk].resize(m_numberOfSamples);

                for (auto jj = 0; jj < m_numberOfSamples; ++jj)
                {
                    std::complex<float> z(jj * realStep + m_minReal, kk * imagStep + m_minImag);

                    std::complex<float> h_z;
                    h_z = b0;
                    for (auto ll = 0U; ll < m_zeros.size(); ++ll)
                        h_z *= (z - m_zeros[ll]);
                    for (auto ll = 0U; ll < m_poles.size(); ++ll)
                        h_z /= (z - m_poles[ll]);

                    h[kk][jj] = 20 * log10(std::abs(h_z));
                }
            }
        }
    }

    void updatePoleAndZero()
    {
        m_poles.clear();
        m_zeros.clear();

        for (auto kk = 0; kk < m_numOfPole; ++kk)
        {
            if (m_vts.getParameter(paramPoleBool.ID[kk])->getValue())
            {
                float real = *(m_vts.getRawParameterValue(paramPoleReal.ID[kk]));
                float imag = *(m_vts.getRawParameterValue(paramPoleImag.ID[kk]));
                m_poles.push_back(std::complex<float>(real, imag));

                if (m_vts.getParameter(paramPoleConjugated.ID[kk])->getValue())
                    m_poles.push_back(std::complex<float>(real, -imag));
            }
        }
        for (auto kk = 0; kk < m_numOfPole; ++kk)
        {
            if (m_vts.getParameter(paramZeroBool.ID[kk])->getValue())
            {
                float real = *(m_vts.getRawParameterValue(paramZeroReal.ID[kk]));
                float imag = *(m_vts.getRawParameterValue(paramZeroImag.ID[kk]));
                m_zeros.push_back(std::complex<float>(real, imag));

                if (m_vts.getParameter(paramZeroConjugated.ID[kk])->getValue())
                    m_zeros.push_back(std::complex<float>(real, -imag));
            }
        }
    }

    void normalizeValues(std::vector<std::vector<float>>& in,
        std::vector<std::vector<float>>& out)
    {
        if (in.size() != out.size())
            return;
        float valueIn;
        for (auto kk = 0U; kk < m_numberOfSamples; ++kk)
            for (auto jj = 0U; jj < m_numberOfSamples; ++jj)
            {
                valueIn = in[kk][jj];
                out[kk][jj] = (valueIn - m_minValueZ) / (m_maxValueZ - m_minValueZ);
            }
    }

    void writeDataToFile(std::vector<std::vector<float>>& in)
    {

        auto dir = juce::File::getCurrentWorkingDirectory();
        int numTries = 0;
        while (!dir.getChildFile("Resources").exists() && numTries++ < 15)
            dir = dir.getParentDirectory();
        if (dir.getChildFile("Resources").getChildFile("TransferFun3DShape.txt").exists())
            dir.getChildFile("Resources").getChildFile("TransferFun3DShape.txt").deleteFile();
        FileOutputStream outputFile(dir.getChildFile("Resources").getChildFile("TransferFun3DShape.txt"));

        float realStep = 1 / static_cast<float>(in.size() - 1);
        float imagStep = 1 / static_cast<float>(in[0].size() - 1);
        float alpha = 1.0;

        Colour poleCol = PoleColour; //
        Colour zeroCol = ZeroColour; //
        ColourGradient gradient;
        gradient = ColourGradient::vertical(ZeroColour,0.0, PoleColour,1.0);
        gradient.addColour(0.7, PoleLightColour);
        gradient.addColour(0.5, NeutralColour);
        gradient.addColour(0.3, ZeroLightColour);
        int counter = 1;
        for (auto kk = 0U; kk < in.size(); ++kk) // imag
        {
            for (auto jj = 0U; jj < in[kk].size(); ++jj) // real
            {
                outputFile << "v " << jj * realStep << " " << kk * imagStep
                    << " " << in[kk][jj] << "\n";

                Colour usedColour = gradient.getColourAtPosition(in[kk][jj]);
                float redVal = usedColour.getFloatRed();
                float greenVal = usedColour.getFloatGreen();
                float blueVal = usedColour.getFloatBlue();
                /*
                float redVal = (1 - in[kk][jj]) * zeroCol.getFloatRed() + in[kk][jj] * poleCol.getFloatRed();
                float greenVal = (1 - in[kk][jj]) * zeroCol.getFloatGreen() + in[kk][jj] * poleCol.getFloatGreen();
                float blueVal = (1 - in[kk][jj]) * zeroCol.getFloatBlue() + in[kk][jj] * poleCol.getFloatBlue();
                // */

                outputFile << "vt " << redVal << " " << greenVal
                    << " " << blueVal << " " << alpha << "\n";

                int index1 = kk * in[kk].size() + jj + 1;
                int index2 = index1 + in.size();
                int index3 = index1 + 1;
                int index4 = index2 + 1;

                if (!(kk == in.size() - 1 || jj == in[kk].size() - 1))
                {
                    outputFile << "f " << index1 << "/" << index1 << "/" << index1;
                    outputFile << " " << index2 << "/" << index2 << "/" << index2;
                    outputFile << " " << index3 << "/" << index3 << "/" << index3;
                    outputFile << "\n";

                    outputFile << "f " << index2 << "/" << index2 << "/" << index2;
                    outputFile << " " << index3 << "/" << index3 << "/" << index3;
                    outputFile << " " << index4 << "/" << index4 << "/" << index4;
                    outputFile << "\n";
                }
                ++counter;
            }
        }
        addAxis(outputFile, counter);
        //*/
    }
    void addAxis(FileOutputStream& outputFile, int counter)
    {
        float alpha = 1.0f;
        float zPos = (0 - m_minValueZ) / (m_maxValueZ - m_minValueZ);
        // x axis
        outputFile << "v " << 0.0 << " " << 0.5 << " " << zPos << "\n";
        outputFile << "v " << 0.0 << " " << 0.51 << " " << zPos << "\n";
        outputFile << "v " << 1.0 << " " << 0.5 << " " << zPos << "\n";
        outputFile << "v " << 1.0 << " " << 0.51 << " " << zPos << "\n";

        outputFile << "v " << 0.0 << " " << 0.5 << " " << zPos << "\n";
        outputFile << "v " << 0.0 << " " << 0.5 << " " << zPos + 0.01 << "\n";
        outputFile << "v " << 1.0 << " " << 0.5 << " " << zPos << "\n";
        outputFile << "v " << 1.0 << " " << 0.5 << " " << zPos + 0.01 << "\n";

        for (int kk = 0; kk < 8; kk++)
            outputFile << "vt " << XAxisColour.getFloatRed() << " "
            << XAxisColour.getFloatGreen() << " "
            << XAxisColour.getFloatBlue() << " " << alpha << "\n";

        // y axis
        outputFile << "v " << 0.5  << " " << 0.0 << " " << zPos << "\n";
        outputFile << "v " << 0.51 << " " << 0.0 << " " << zPos << "\n";
        outputFile << "v " << 0.5  << " " << 1.0 << " " << zPos << "\n";
        outputFile << "v " << 0.51 << " " << 1.0 << " " << zPos << "\n";

        outputFile << "v " << 0.5 << " " << 0.0 << " " << zPos << "\n";
        outputFile << "v " << 0.5 << " " << 0.0 << " " << zPos + 0.01 << "\n";
        outputFile << "v " << 0.5 << " " << 1.0 << " " << zPos << "\n";
        outputFile << "v " << 0.5 << " " << 1.0 << " " << zPos + 0.01 << "\n";

        for (int kk = 0; kk < 8; kk++)
            outputFile << "vt " << YAxisColour.getFloatRed() << " "
            << YAxisColour.getFloatGreen() << " "
            << YAxisColour.getFloatBlue() << " " << alpha << "\n";

        // z axis
        outputFile << "v " << 0.5 << " " << 0.5 << " " << 0.0 << "\n";
        outputFile << "v " << 0.51 << " " << 0.5 << " " << 0.0 << "\n";
        outputFile << "v " << 0.5 << " " << 0.5 << " " << 1.0 << "\n";
        outputFile << "v " << 0.51 << " " << 0.5 << " " << 1.0 << "\n";

        outputFile << "v " << 0.5 << " " << 0.5 << " " << 0.0 << "\n";
        outputFile << "v " << 0.5 << " " << 0.51 << " " << 0.0 << "\n";
        outputFile << "v " << 0.5 << " " << 0.5 << " " << 1.0 << "\n";
        outputFile << "v " << 0.5 << " " << 0.51 << " " << 1.0 << "\n";

        for (int kk = 0; kk < 8; kk++)
            outputFile << "vt " << ZAxisColour.getFloatRed() << " "
            << ZAxisColour.getFloatGreen() << " "
            << ZAxisColour.getFloatBlue() << " " << alpha << "\n";

        for (int kk = 0; kk < 6; kk++)
        {
            outputFile << "f " << counter << "/" << counter << "/" << counter;
            outputFile << " " << counter + 1 << "/" << counter + 1 << "/" << counter + 1;
            outputFile << " " << counter + 2 << "/" << counter + 2 << "/" << counter + 2;
            outputFile << "\n";

            outputFile << "f " << counter + 1 << "/" << counter + 1 << "/" << counter + 1;
            outputFile << " " << counter + 2 << "/" << counter + 2 << "/" << counter + 2;
            outputFile << " " << counter + 3 << "/" << counter + 3 << "/" << counter + 3;
            outputFile << "\n";
            counter += 4;
        }

    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TransferFun3DComponent)
};