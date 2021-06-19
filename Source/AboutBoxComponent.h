/*
  ==============================================================================
    AboutBoxComponent.h

    This components include the image drawing of the about box. 

    Authors:    Arno Schiller (AS)
    Version:    v0.0.1

    SPDX-License-Identifier: BSD-3-Clause

    Version history:
    Version:   (Author) Description:								Date:     \n
    v0.0.1     (AS) First initialize.                               19.06.2021\n
 ===============================================================================
*/


#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class AboutBoxComponent    : public Component
{
public:
    AboutBoxComponent()
    {
        m_aboutBoxImg = ImageFileFormat::loadFrom(BinaryData::AboutBox_jpg, BinaryData::AboutBox_jpgSize);
    }

    ~AboutBoxComponent()
    {
    }

    void paint (Graphics& g) override
    {
        g.fillAll (getLookAndFeel().findColour(ResizableWindow::backgroundColourId).withAlpha(m_backgroundAlpha));   

        int img_w = m_aboutBoxImg.getWidth();
        int img_h = m_aboutBoxImg.getHeight();
        g.drawImage(m_aboutBoxImg, getWidth()/2-img_w/2, getHeight()/2-img_h/2, img_w, img_h, 0, 0, img_w, img_h);

    }

    void mouseDown(const MouseEvent& e)
    {
        if(componentPressed!=nullptr) componentPressed();
    }  
        
    std::function<void()> componentPressed;

private:

    Image m_aboutBoxImg;
    float m_backgroundAlpha = 0.6f;

};

       