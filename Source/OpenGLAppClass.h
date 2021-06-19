/**
  ==============================================================================

    This class contains the OpenGL element and rules the visualization (Scaling, Drawing, Rotation, ...)                                                             

    Authors:    Jannik Hartog (JH), Arno Schiller (AS)
    Version:    v0.0.5
    
    SPDX-License-Identifier: BSD-3-Clause

    Version history:
    Version:   (Author) Description:								Date:     \n
    v0.0.1     (JH) First initialize. Included code from tutorial.  23.07.2020\n
    v0.0.2     (JH) ...                                             24.07.2020\n
    v0.0.3     (JH) ...                                             25.07.2020\n
    v0.0.4     (JH) ...                                             26.07.2020\n
    v0.0.5     (AS) Added legend and scale to paint function.       26.07.2020\n
    v0.1.0     (AS) Updated Shape constructor to load from points   04.03.2021\n
                    matrix, also included variable to reference 
                    the points matrix to display.


   This file is part of the JUCE tutorials.
   Copyright (c) 2017 - ROLI Ltd.

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
   WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
   PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

 name:             OpenGLAppTutorial
 version:          1.0.1
 vendor:           JUCE
 website:          http://juce.com
 description:      Explores the OpenGL features. 

 dependencies:     juce_core, juce_data_structures, juce_events, juce_graphics,
                   juce_gui_basics, juce_gui_extra, juce_opengl
 exporters:        xcode_mac, vs2017, xcode_iphone

 type:             Component
 mainClass:        MainContentComponent

 useLocalCopy:     1

 END_JUCE_PIP_METADATA

*******************************************************************************/

#include "WavefrontObjFile.h"
#include <juce_opengl/juce_opengl.h>
#include <math.h>
//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content
*/


#define LOGO_SCALE_X 0.15

class OpenGLAppClass : public juce::OpenGLAppComponent
{
public:
    
    std::vector<std::vector<float>>& m_pointsMatrix;
    //==============================================================================
    Draggable3DOrientation m_draggableOrientation;
    float m_scale = 0.5;

    std::function<void()> logoPressed;

    OpenGLAppClass(float minZ, float maxZ, std::vector<std::vector<float>>& pointsMatrix)
        : m_maxValueZ(maxZ), m_minValueZ(minZ), m_pointsMatrix(pointsMatrix)
    {

        m_refreshButton.setButtonText("center");
        m_refreshButton.onClick = [this]() {refreshView();};
        addAndMakeVisible(m_refreshButton);

        setOpaque(true);

        // Tell the context to repaint on a loop.
        openGLContext.setContinuousRepainting(true);

        // Finally - we attach the context to this Component.
        openGLContext.attachTo(*this);

        
        m_JadeLogo = ImageFileFormat::loadFrom(BinaryData::LogoJadeHochschuleTrans_png, BinaryData::LogoJadeHochschuleTrans_pngSize);

    }

    ~OpenGLAppClass() override
    {
        shutdownOpenGL();
    }

    void initialise() override
    {
        createShaders();

    }

    void shutdown() override
    {
        shader    .reset();
        shape     .reset();
        attributes.reset();
        uniforms  .reset();
    }

    void refreshView()
    {
        m_viewMatrix= getViewMatrix();
    }


    void setSomethingChanged()
    {
        m_somethingChanged = true;
    }

    juce::Matrix3D<float> getProjectionMatrix() const
    {
        auto w = 1.0f / (0.5f +m_scale+ 0.1f);                                          // [1]
        auto h = w * getLocalBounds().toFloat().getAspectRatio (false);         // [2]

        return juce::Matrix3D<float>::fromFrustum (-w, w, -h, h, 4.0f, 00.0f);  // [3]
    }

    juce::Matrix3D<float> getViewMatrix() const
    {
                Matrix3D<float> viewMatrix ({ 0.0f, 0.0f, -10.0f });
                Matrix3D<float> rotationMatrix = viewMatrix.rotation ({ 4.25f,  0.0f , 0.0f });

        return rotationMatrix * viewMatrix;
        
    }
    void mouseDown(const MouseEvent& e)
    {
        //m_draggableOrientation.mouseDown(e.getPosition());
        auto bounds = getLocalBounds();

        int x = e.getMouseDownX();
        int y = e.getMouseDownY();

        auto img_ratio = m_JadeLogo.getWidth() / m_JadeLogo.getHeight();
        int img_h = int(getHeight() * LOGO_SCALE_X);
        int img_w = int(img_h * img_ratio);  

        int logo_start_x = bounds.getWidth() - img_w;
        int logo_end_x = bounds.getWidth();
        int logo_start_y = 0;
        int logo_end_y = img_h;

        if( x > logo_start_x && x < logo_end_x && y > logo_start_y && y < logo_end_y)
        {
            if (logoPressed != nullptr)
                logoPressed();
        }
    
    } 

    void mouseDrag(const MouseEvent& e)
    {
      //  m_draggableOrientation.mouseDrag(e.getPosition());
        Matrix3D<float> viewMatrix ({ 0.0f, 0.0f, -10.0f });
        Matrix3D<float> rotationMatrix = viewMatrix.rotation ({ ((float) e.getDistanceFromDragStartY() * 0.03f),  ((float) e.getDistanceFromDragStartX() * 0.03f), 0.0f });
 
        m_viewMatrix= rotationMatrix * viewMatrix;
    }

    void mouseWheelMove(const MouseEvent&, const MouseWheelDetails& d) override
    {
         m_scale = m_scale + d.deltaY;
         if (m_scale <= -0.6)
             m_scale = -0.5;
    }

    

    void render() override
    {
        juce::Thread::sleep( 40 );
        jassert (juce::OpenGLHelpers::isContextActive());

        auto desktopScale = (float) openGLContext.getRenderingScale();          // [1]
        juce::OpenGLHelpers::clear (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId)); // [2]
 

glEnable(GL_DEPTH_TEST);
/*         GLbyte graph[256][256];
        glDepthFunc (GL_LESS);
        glDepthMask(GL_TRUE);
        glEnable (GL_DEPTH_TEST);
        glEnable (GL_BLEND);                                                    // [3]
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        openGLContext.extensions.glActiveTexture (GL_TEXTURE0);
        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_ALWAYS, 0x1, 0xffffffff);
        glStencilOp(GL_KEEP, GL_REPLACE, GL_KEEP);
        glEnable (GL_TEXTURE_2D); 
        glBindTexture(GL_TEXTURE_2D,0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 256, 256, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,graph);
 */


        glViewport (0,
                    0,
                    juce::roundToInt (desktopScale * (float) getWidth()),
                    juce::roundToInt (desktopScale * (float) getHeight()));     // [4]

        shader->use();                                                          // [5]

        if (uniforms->projectionMatrix.get() != nullptr)                        // [6]
            uniforms->projectionMatrix->setMatrix4 (getProjectionMatrix().mat, 1, false);

        if (uniforms->viewMatrix.get() != nullptr)                              // [7]
            uniforms->viewMatrix->setMatrix4 (m_viewMatrix.mat, 1, false);
        
        if (m_somethingChanged)
        {
            m_somethingChanged = false;
            if (updateData != nullptr)
                updateData();
            shape.reset(new Shape(openGLContext, m_pointsMatrix));
        }
        
        shape->draw (openGLContext, *attributes);                               // [8]

        // Reset the element buffers so child Components draw correctly
        openGLContext.extensions.glBindBuffer (GL_ARRAY_BUFFER, 0);             // [9]
        openGLContext.extensions.glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
    }

#define AXIS_LENGTH 30
#define AXIS_LENGTH_ROTATED 20
#define TEXT_HEIGHT 20
#define TEXT_WIDTH 35

    void paint(Graphics& g) override
    {
        auto img_ratio = m_JadeLogo.getWidth() / m_JadeLogo.getHeight();
        int img_h = int(getHeight() * LOGO_SCALE_X);
        int img_w = int(img_h * img_ratio);       
        g.drawImage(m_JadeLogo, getWidth()-img_w, 0, img_w, img_h, 0, 0, m_JadeLogo.getWidth(), m_JadeLogo.getHeight());


        g.setColour(BorderColour);
        g.drawRect(getLocalBounds(), LINEWIDH_MEDIUM);

        auto bounds = getLocalBounds();
        auto help = bounds.removeFromBottom(100);
        help.reduce(10, 0);
        auto legendBounds = help.removeFromLeft(100);
        legendBounds.reduce(35, 20);
        auto  scaleDescriptionBounds = help.removeFromRight(60);
        scaleDescriptionBounds.reduce(0, 10);
        auto scaleBounds = scaleDescriptionBounds.removeFromLeft(15);
        scaleDescriptionBounds.reduce(0, 2);

        // axis legend
        Point<int> center(legendBounds.getX(), 
            legendBounds.getY() + legendBounds.getHeight());
        Point<int> xAxisEnd(center.getX() + AXIS_LENGTH, center.getY());
        Point<int> yAxisEnd(center.getX(), center.getY() - AXIS_LENGTH);
        Point<int> zAxisEnd(center.getX() - AXIS_LENGTH_ROTATED, center.getY() - AXIS_LENGTH_ROTATED);

        g.setColour(XAxisColour);
        Line<float> xAxis(center.getX(), center.getY(), xAxisEnd.getX(), xAxisEnd.getY());
        g.drawArrow(xAxis, LINEWIDH_MEDIUM, 4, 4);

        g.setColour(YAxisColour);
        Line<float> yAxis(center.getX(), center.getY(), yAxisEnd.getX(), yAxisEnd.getY());
        g.drawArrow(yAxis, LINEWIDH_MEDIUM, 4, 4);

        g.setColour(ZAxisColour);
        Line<float> zAxis(center.getX(), center.getY(), zAxisEnd.getX(), zAxisEnd.getY());
        g.drawArrow(zAxis, LINEWIDH_MEDIUM, 4, 4);

        g.setColour(AxisTextColour);
        g.setFont(FontSize_medium);
        g.drawText("Re", xAxisEnd.getX() - TEXT_WIDTH * 0.25, xAxisEnd.getY() - TEXT_HEIGHT * 0.5,
            TEXT_WIDTH, TEXT_HEIGHT, Justification::centred);
        g.drawText("Im", yAxisEnd.getX() - TEXT_WIDTH * 0.5, yAxisEnd.getY() - TEXT_HEIGHT,
            TEXT_WIDTH, TEXT_HEIGHT, Justification::centred);
        g.drawText("|H|", zAxisEnd.getX() - TEXT_WIDTH *0.8, zAxisEnd.getY() - TEXT_HEIGHT,
            TEXT_WIDTH, TEXT_HEIGHT, Justification::centred);

        // scale
        ColourGradient gradient;
        gradient = ColourGradient::vertical(PoleColour, ZeroColour, scaleBounds);
        gradient.addColour(0.7, ZeroLightColour);
        gradient.addColour(0.5, NeutralColour);
        gradient.addColour(0.3, PoleLightColour);
        g.setGradientFill(gradient);
        g.fillRect(scaleBounds);
        
        g.setColour(AxisTextColour);
        g.drawRect(scaleBounds);
        int startX = scaleDescriptionBounds.getX();
        int startY = scaleDescriptionBounds.getY();
        int centerY = startY + scaleDescriptionBounds.getHeight() / 2;
        int endY = startY + scaleDescriptionBounds.getHeight();
        g.drawText(String(m_maxValueZ) + String(" dB"), startX, startY - TEXT_HEIGHT / 2,
            TEXT_WIDTH, TEXT_HEIGHT, Justification::centred);

        g.drawText(String((m_maxValueZ-m_minValueZ)/2 + m_minValueZ) + String(" dB"), startX,
            centerY - TEXT_HEIGHT / 2, TEXT_WIDTH, TEXT_HEIGHT, Justification::centred);

        g.drawText(String(m_minValueZ)+String(" dB"), startX, endY - TEXT_HEIGHT / 2, 
            TEXT_WIDTH, TEXT_HEIGHT, Justification::centred);
    }

    void resized() override
    {
        // This is called when the MainContentComponent is resized.
        // If you add any child components, this is where you should
        // update their positions.

        auto bounds = getLocalBounds().removeFromLeft(70).removeFromTop(30);
        bounds.reduce(10,5);
        m_refreshButton.setBounds(bounds);

        m_draggableOrientation.setViewport(getLocalBounds());
    }

    void createShaders()
    {
        vertexShader = R"(
            attribute vec4 position;
            attribute vec4 sourceColour;
            attribute vec4 textureCoordIn;

            uniform mat4 projectionMatrix;
            uniform mat4 viewMatrix;

            varying vec4 destinationColour;
            varying vec4 textureCoordOut;

            void main()
            {
                destinationColour = sourceColour;
                textureCoordOut = textureCoordIn;
                gl_Position = projectionMatrix * viewMatrix * position;
            })";

        fragmentShader =
           #if JUCE_OPENGL_ES
            R"(varying lowp vec4 destinationColour;
               varying lowp vec2 textureCoordOut;)"
           #else

            R"(varying vec4 destinationColour;
               varying vec4 textureCoordOut;)"
           #endif
            R"(
               void main()
               {)"
           #if JUCE_OPENGL_ES
            R"(    lowp vec4 colour = vec4(0.95, 0.57, 0.03, 0.7);)"
           #else
            R"(    vec4 colour = textureCoordOut;)"
           #endif
            R"(    gl_FragColor = colour;
               })";

        std::unique_ptr<juce::OpenGLShaderProgram> newShader (new juce::OpenGLShaderProgram (openGLContext));   // [1]
        juce::String statusText;

        if (newShader->addVertexShader (juce::OpenGLHelpers::translateVertexShaderToV3 (vertexShader))          // [2]
              && newShader->addFragmentShader (juce::OpenGLHelpers::translateFragmentShaderToV3 (fragmentShader))
              && newShader->link())
        {
            shape     .reset();
            attributes.reset();
            uniforms  .reset();

            shader.reset (newShader.release());                                                                 // [3]
            shader->use();
            
            shape     .reset (new Shape (openGLContext, m_pointsMatrix));
            attributes.reset (new Attributes (openGLContext, *shader));
            uniforms  .reset (new Uniforms (openGLContext, *shader));

            statusText = "GLSL: v" + juce::String (juce::OpenGLShaderProgram::getLanguageVersion(), 2);
        }
        else
        {
            statusText = newShader->getLastError();                                                             // [4]
        }
    }

    std::function<void()> updateData;

private:

    Matrix3D<float> m_viewMatrix = getViewMatrix();
    bool m_somethingChanged;
    float m_minValueZ;
    float m_maxValueZ;
    float m_valueStepZ;

    Image m_JadeLogo;

    TextButton m_refreshButton;

    //==============================================================================
    struct Vertex
    {
        float position[3];
        float normal[3];
        float colour[4];
        float texCoord[4];
    };

    //==============================================================================
    // This class just manages the attributes that the shaders use.
    struct Attributes
    {
        Attributes (juce::OpenGLContext& context, juce::OpenGLShaderProgram& shaderProgram)
        {
            position      .reset (createAttribute (context, shaderProgram, "position"));
            normal        .reset (createAttribute (context, shaderProgram, "normal"));
            sourceColour  .reset (createAttribute (context, shaderProgram, "sourceColour"));
            textureCoordIn.reset (createAttribute (context, shaderProgram, "textureCoordIn"));
        }

        void enable (juce::OpenGLContext& context)
        {
            if (position.get() != nullptr)
            {
                context.extensions.glVertexAttribPointer (position->attributeID, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex), nullptr);
                context.extensions.glEnableVertexAttribArray (position->attributeID);
            }

            if (normal.get() != nullptr)
            {
                context.extensions.glVertexAttribPointer (normal->attributeID, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*) (sizeof (float) * 3));
                context.extensions.glEnableVertexAttribArray (normal->attributeID);
            }

            if (sourceColour.get() != nullptr)
            {
                context.extensions.glVertexAttribPointer (sourceColour->attributeID, 4, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*) (sizeof (float) * 6));
                context.extensions.glEnableVertexAttribArray (sourceColour->attributeID);
            }

            if (textureCoordIn.get() != nullptr)
            {
                context.extensions.glVertexAttribPointer (textureCoordIn->attributeID, 4, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*) (sizeof (float) * 10));
                context.extensions.glEnableVertexAttribArray (textureCoordIn->attributeID);
            }
        }

        void disable (juce::OpenGLContext& context)
        {
            if (position.get() != nullptr)       context.extensions.glDisableVertexAttribArray (position->attributeID);
            if (normal.get() != nullptr)         context.extensions.glDisableVertexAttribArray (normal->attributeID);
            if (sourceColour.get() != nullptr)   context.extensions.glDisableVertexAttribArray (sourceColour->attributeID);
            if (textureCoordIn.get() != nullptr) context.extensions.glDisableVertexAttribArray (textureCoordIn->attributeID);
        }

        std::unique_ptr<juce::OpenGLShaderProgram::Attribute> position, normal, sourceColour, textureCoordIn;

    private:
        static juce::OpenGLShaderProgram::Attribute* createAttribute (juce::OpenGLContext& context,
                                                                      juce::OpenGLShaderProgram& shader,
                                                                      const juce::String& attributeName)
        {
            if (context.extensions.glGetAttribLocation (shader.getProgramID(), attributeName.toRawUTF8()) < 0)
                return nullptr;

            return new juce::OpenGLShaderProgram::Attribute (shader, attributeName.toRawUTF8());
        }
    };

    //==============================================================================
    // This class just manages the uniform values that the demo shaders use.
    struct Uniforms
    {
        Uniforms (juce::OpenGLContext& context, juce::OpenGLShaderProgram& shaderProgram)
        {
            projectionMatrix.reset (createUniform (context, shaderProgram, "projectionMatrix"));
            viewMatrix      .reset (createUniform (context, shaderProgram, "viewMatrix"));
        }

        std::unique_ptr<juce::OpenGLShaderProgram::Uniform> projectionMatrix, viewMatrix;

    private:
        static juce::OpenGLShaderProgram::Uniform* createUniform (juce::OpenGLContext& context,
                                                                  juce::OpenGLShaderProgram& shaderProgram,
                                                                  const juce::String& uniformName)
        {
            if (context.extensions.glGetUniformLocation (shaderProgram.getProgramID(), uniformName.toRawUTF8()) < 0)
                return nullptr;

            return new juce::OpenGLShaderProgram::Uniform (shaderProgram, uniformName.toRawUTF8());
        }
    };

    //==============================================================================
    /** This loads a 3D model from an OBJ file and converts it into some vertex buffers
        that we can draw.
    */
    struct Shape
    {
        Shape(juce::OpenGLContext& context)
        {
            auto dir = juce::File::getCurrentWorkingDirectory();

            int numTries = 0;

            while (!dir.getChildFile("Resources").exists() && numTries++ < 15)
                dir = dir.getParentDirectory();

            if (shapeFile.load(dir.getChildFile("Resources").getChildFile("TransferFun3DShape.txt")).wasOk())
                for (auto* s : shapeFile.shapes)
                    vertexBuffers.add(new VertexBuffer(context, *s));
        }
        Shape(juce::OpenGLContext& context, std::vector<std::vector<float>>& values)
        {
            if (shapeFile.load(values).wasOk())
                for (auto* s : shapeFile.shapes)
                    vertexBuffers.add(new VertexBuffer(context, *s));
        }

        void draw(juce::OpenGLContext& context, Attributes& glAttributes)
        {
            for (auto* vertexBuffer : vertexBuffers)
            {
                vertexBuffer->bind();
                glAttributes.enable(context);
                glDrawElements(GL_TRIANGLES, vertexBuffer->numIndices, GL_UNSIGNED_INT, nullptr);
                glAttributes.disable(context);
            }
        }


    private:
        struct VertexBuffer
        {
            VertexBuffer (juce::OpenGLContext& context, WavefrontObjFile::Shape& aShape) : openGLContext (context)
            {
                numIndices = aShape.mesh.indices.size();                                    // [1]

                openGLContext.extensions.glGenBuffers (1, &vertexBuffer);                   // [2]
                openGLContext.extensions.glBindBuffer (GL_ARRAY_BUFFER, vertexBuffer);

                juce::Array<Vertex> vertices;
                createVertexListFromMesh(aShape.mesh, vertices, juce::Colour::fromFloatRGBA(1.f,1.f,1.f,1.f));     // [3]

                openGLContext.extensions.glBufferData (GL_ARRAY_BUFFER,                     // [4]
                                                       static_cast<GLsizeiptr> (static_cast<size_t> (vertices.size()) * sizeof (Vertex)),
                                                       vertices.getRawDataPointer(), GL_STATIC_DRAW);

                openGLContext.extensions.glGenBuffers (1, &indexBuffer);                    // [5]
                openGLContext.extensions.glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
                openGLContext.extensions.glBufferData (GL_ELEMENT_ARRAY_BUFFER,
                                                       static_cast<GLsizeiptr> (static_cast<size_t> (numIndices) * sizeof (juce::uint32)),
                                                       aShape.mesh.indices.getRawDataPointer(), GL_STATIC_DRAW);
            }

            ~VertexBuffer()
            {
                openGLContext.extensions.glDeleteBuffers (1, &vertexBuffer);
                openGLContext.extensions.glDeleteBuffers (1, &indexBuffer);
            }

            void bind()
            {
                openGLContext.extensions.glBindBuffer (GL_ARRAY_BUFFER, vertexBuffer);
                openGLContext.extensions.glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
            }

            GLuint vertexBuffer, indexBuffer;
            int numIndices;
            juce::OpenGLContext& openGLContext;

            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VertexBuffer)
        };

        WavefrontObjFile shapeFile;
        juce::OwnedArray<VertexBuffer> vertexBuffers;

        static void createVertexListFromMesh (const WavefrontObjFile::Mesh& mesh, juce::Array<Vertex>& list, juce::Colour colour)
        {

            auto scale = 2.0f;                                              // [6]
            WavefrontObjFile::TextureCoord defaultTexCoord { 0.5f, 0.5f, 0.5f, 1.0f};
            WavefrontObjFile::Vertex defaultNormal { 0.5f, 0.5f, 0.5f };
            for (auto i = 0; i < mesh.vertices.size(); ++i)                     // [7]
            {
                const auto& v = mesh.vertices.getReference (i);
                const auto& n = i < mesh.normals.size() ? mesh.normals.getReference (i) : defaultNormal;
                const auto& tc = i < mesh.textureCoords.size() ? mesh.textureCoords.getReference (i) : defaultTexCoord;

                list.add ({ { scale * (v.x-0.5f), scale * (v.y-0.5f), scale * (v.z-0.5f), },
                            { scale * n.x, scale * n.y, scale * n.z, },
                            { colour.getFloatRed(), colour.getFloatGreen(), colour.getFloatBlue(), colour.getFloatAlpha() },
                            { tc.a, tc.b, tc.c,tc.d } });                                  // [8]
            }
        }
    };

    juce::String vertexShader;
    juce::String fragmentShader;

    std::unique_ptr<juce::OpenGLShaderProgram> shader;
    std::unique_ptr<Shape> shape;
    std::unique_ptr<Attributes> attributes;
    std::unique_ptr<Uniforms> uniforms;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OpenGLAppClass)
};
