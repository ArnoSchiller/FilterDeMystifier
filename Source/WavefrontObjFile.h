/*
  ==============================================================================

   This file contains parts of the JUCE tutorials. 
   Copyright (c) 2017 - ROLI Ltd.

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

    As mentioned before, some code fragments are taken from the JUCE tutorials
    and were extended by the named authors.

  ==============================================================================
    Authors:    Jannik Hartog (JH), Arno Schiller (AS)
    Version:    v0.0.3
    
    SPDX-License-Identifier: BSD-3-Clause
    
    Version:   (Author) Description:								Date:     \n
    v0.0.1     (JH) ...                                             ??.??.2020\n
    v0.0.2     (JH) Created (mesh) object file to parse.            ??.??.2020\n
    v0.0.3     (AS) Included a parser for points matrix.            04.03.2021\n
*/

/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

 name:             OpenGLAppTutorial
 version:          1.0.0
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


#pragma once

#include <map>
//#include <JuceHeader.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_events/juce_events.h>
#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_opengl/juce_opengl.h>
//==============================================================================
/**
    This is a quick-and-dirty parser for the 3D OBJ file format.

    Just call load() and if there aren't any errors, the 'shapes' array should
    be filled with all the shape objects that were loaded from the file.
*/
class WavefrontObjFile
{
public:

    ColourGradient m_gradient;

    WavefrontObjFile() 
    {
        m_gradient = ColourGradient::vertical(ZeroColour, 0.0, PoleColour, 1.0);
        m_gradient.addColour(0.3, Colours::green);
        m_gradient.addColour(0.5, NeutralColour);
        m_gradient.addColour(0.7, Colours::orange);
    }

    juce::Result load(const juce::String& objFileContent)
    {
        shapes.clear();
        return parseObjFile(juce::StringArray::fromLines(objFileContent));
    }

    juce::Result load(const juce::File& file)
    {
        sourceFile = file;
        return load(file.loadFileAsString());
    }

    /**
     * load the shape by parsing a points matrix (wrote bySchiller).
     * 
     * \param in points matrix
     * \return juce::Result::ok if successfull
     */
    juce::Result load(std::vector<std::vector<float>>& in)
    {
        shapes.clear();
        return parsePointsMarix(in);
    }
    //==============================================================================
    typedef juce::uint32 Index;

    struct Vertex { float x, y, z; };
    struct TextureCoord { float a, b, c, d; };

    struct Mesh
    {
        juce::Array<Vertex> vertices, normals;
        juce::Array<TextureCoord> textureCoords;
        juce::Array<Index> indices;
    };

    struct Material
    {
        Material() noexcept
        {
            juce::zerostruct(ambient);
            juce::zerostruct(diffuse);
            juce::zerostruct(specular);
            juce::zerostruct(transmittance);
            juce::zerostruct(emission);
        }

        juce::String name;

        Vertex ambient, diffuse, specular, transmittance, emission;
        float shininess = 1.0f, refractiveIndex = 0.0f;

        juce::String ambientTextureName, diffuseTextureName,
            specularTextureName, normalTextureName;

        juce::StringPairArray parameters;
    };

    struct Shape
    {
        juce::String name;
        Mesh mesh;
        Material material;
    };

    juce::OwnedArray<Shape> shapes;

private:
    //==============================================================================
    juce::File sourceFile;
    float m_gridStep = 0.5;
    float m_numGridlines=jmax(paramPoleReal.maxValue - paramPoleReal.minValue,paramPoleImag.maxValue - paramPoleImag.minValue) / m_gridStep;
    float m_thickness= 0.004f;
    float m_radius=0.5 / jmax(paramPoleReal.maxValue,paramPoleImag.maxValue);

    struct TripleIndex
    {
        TripleIndex() noexcept {}

        bool operator< (const TripleIndex& other) const noexcept
        {
            if (this == &other)
                return false;

            if (vertexIndex != other.vertexIndex)
                return vertexIndex < other.vertexIndex;

            if (textureIndex != other.textureIndex)
                return textureIndex < other.textureIndex;

            return normalIndex < other.normalIndex;
        }

        int vertexIndex = -1, textureIndex = -1, normalIndex = -1;
    };

    struct IndexMap
    {
        std::map<TripleIndex, Index> map;

        Index getIndexFor(TripleIndex i, Mesh& newMesh, const Mesh& srcMesh)
        {
            const std::map<TripleIndex, Index>::iterator it(map.find(i));

            if (it != map.end())
                return it->second;

            auto index = (Index)newMesh.vertices.size();

            if (juce::isPositiveAndBelow(i.vertexIndex, srcMesh.vertices.size()))
                newMesh.vertices.add(srcMesh.vertices.getReference(i.vertexIndex));

            if (juce::isPositiveAndBelow(i.normalIndex, srcMesh.normals.size()))
                newMesh.normals.add(srcMesh.normals.getReference(i.normalIndex));

            if (juce::isPositiveAndBelow(i.textureIndex, srcMesh.textureCoords.size()))
                newMesh.textureCoords.add(srcMesh.textureCoords.getReference(i.textureIndex));

            map[i] = index;
            return index;
        }
    };

    static float parseFloat(juce::String::CharPointerType& t)
    {
        t = t.findEndOfWhitespace();
        return (float)juce::CharacterFunctions::readDoubleValue(t);
    }

    static Vertex parseVertex(juce::String::CharPointerType t)
    {
        Vertex v;
        v.x = parseFloat(t);
        v.y = parseFloat(t);
        v.z = parseFloat(t);
        return v;
    }

    static TextureCoord parseTextureCoord(juce::String::CharPointerType t)
    {
        TextureCoord tc;
        tc.a = parseFloat(t);
        tc.b = parseFloat(t);
        tc.c = parseFloat(t);
        tc.d = parseFloat(t);
        return tc;
    }

    static bool matchToken(juce::String::CharPointerType& t, const char* token)
    {
        auto len = (int)strlen(token);

        if (juce::CharacterFunctions::compareUpTo(juce::CharPointer_ASCII(token), t, len) == 0)
        {
            auto end = t + len;

            if (end.isEmpty() || end.isWhitespace())
            {
                t = end.findEndOfWhitespace();
                return true;
            }
        }

        return false;
    }

    struct Face
    {
        Face(juce::String::CharPointerType t)
        {
            while (!t.isEmpty())
                triples.add(parseTriple(t));
        }

        juce::Array<TripleIndex> triples;

        void addIndices(Mesh& newMesh, const Mesh& srcMesh, IndexMap& indexMap)
        {
            TripleIndex i0(triples[0]), i1, i2(triples[1]);

            for (auto i = 2; i < triples.size(); ++i)
            {
                i1 = i2;
                i2 = triples.getReference(i);

                newMesh.indices.add(indexMap.getIndexFor(i0, newMesh, srcMesh));
                newMesh.indices.add(indexMap.getIndexFor(i1, newMesh, srcMesh));
                newMesh.indices.add(indexMap.getIndexFor(i2, newMesh, srcMesh));
            }
        }

        static TripleIndex parseTriple(juce::String::CharPointerType& t)
        {
            TripleIndex i;

            t = t.findEndOfWhitespace();
            i.vertexIndex = t.getIntValue32() - 1;
            t = findEndOfFaceToken(t);

            if (t.isEmpty() || t.getAndAdvance() != '/')
                return i;

            if (*t == '/')
            {
                ++t;
            }
            else
            {
                i.textureIndex = t.getIntValue32() - 1;
                t = findEndOfFaceToken(t);

                if (t.isEmpty() || t.getAndAdvance() != '/')
                    return i;
            }

            i.normalIndex = t.getIntValue32() - 1;
            t = findEndOfFaceToken(t);
            return i;
        }

        static juce::String::CharPointerType findEndOfFaceToken(juce::String::CharPointerType t) noexcept
        {
            return juce::CharacterFunctions::findEndOfToken(t, juce::CharPointer_ASCII("/ \t"), juce::String().getCharPointer());
        }
    };

    static Shape* parseFaceGroup(const Mesh& srcMesh,
        juce::Array<Face>& faceGroup,
        const Material& material,
        const juce::String& name)
    {
        if (faceGroup.size() == 0)
            return nullptr;

        std::unique_ptr<Shape> shape(new Shape());
        shape->name = name;
        shape->material = material;

        IndexMap indexMap;

        for (auto& f : faceGroup)
            f.addIndices(shape->mesh, srcMesh, indexMap);

        return shape.release();
    }

    juce::Result parseObjFile(const juce::StringArray& lines)
    {
        Mesh mesh;
        juce::Array<Face> faceGroup;

        juce::Array<Material> knownMaterials;
        Material lastMaterial;
        juce::String lastName;

        for (auto lineNum = 0; lineNum < lines.size(); ++lineNum)
        {
            auto l = lines[lineNum].getCharPointer().findEndOfWhitespace();

            if (matchToken(l, "v")) { mesh.vertices.add(parseVertex(l));            continue; }
            if (matchToken(l, "vn")) { mesh.normals.add(parseVertex(l));             continue; }
            if (matchToken(l, "vt")) { mesh.textureCoords.add(parseTextureCoord(l)); continue; }
            if (matchToken(l, "f")) { faceGroup.add(Face(l));                       continue; }

            if (matchToken(l, "usemtl"))
            {
                auto name = juce::String(l).trim();

                for (auto i = knownMaterials.size(); --i >= 0;)
                {
                    if (knownMaterials.getReference(i).name == name)
                    {
                        lastMaterial = knownMaterials.getReference(i);
                        break;
                    }
                }

                continue;
            }

            if (matchToken(l, "mtllib"))
            {
                juce::Result r = parseMaterial(knownMaterials, juce::String(l).trim());
                continue;
            }

            if (matchToken(l, "g") || matchToken(l, "o"))
            {
                if (Shape* shape = parseFaceGroup(mesh, faceGroup, lastMaterial, lastName))
                    shapes.add(shape);

                faceGroup.clear();
                lastName = juce::StringArray::fromTokens(l, " \t", "")[0];
                continue;
            }
        }

        if (auto* shape = parseFaceGroup(mesh, faceGroup, lastMaterial, lastName))
            shapes.add(shape);

        return juce::Result::ok();
    }

    /**
     * parse a points matrix into drawable format (wrote bySchiller)
     * 
     * \param in reference to a points matrix
     * \return juce::Result::ok if successfull
     */
    juce::Result parsePointsMarix(std::vector<std::vector<float>>& in)
    {
        Mesh mesh;
        juce::Array<Face> faceGroup;

        juce::Array<Material> knownMaterials;
        Material lastMaterial;
        juce::String lastName;

        float realStep = 1 / static_cast<float>(in.size() - 1);
        float imagStep = 1 / static_cast<float>(in[0].size() - 1);
        float alpha = 1.0;
        int counter = 1;

        for (auto kk = 0U; kk < in.size(); ++kk) // imag
        {
            for (auto jj = 0U; jj < in[kk].size(); ++jj) // real
            {
                Vertex v;
                v.x = jj * realStep;
                v.y = kk * imagStep;
                v.z = in[kk][jj];
                mesh.vertices.add(v);

                Colour usedColour = m_gradient.getColourAtPosition(in[kk][jj]);
                float redVal = usedColour.getFloatRed();
                float greenVal = usedColour.getFloatGreen();
                float blueVal = usedColour.getFloatBlue();
                float a=sqrt(pow(v.x-0.5,2)+pow(v.y-0.5,2)); // verschiebung des Koordinatenurpsrungs in die Mitte(-0.5)
                for (auto ll=0U; ll<=m_numGridlines;++ll){
                    if(v.x>(ll/m_numGridlines-m_thickness) && v.x<(ll/m_numGridlines+m_thickness) || v.y>(ll/m_numGridlines-m_thickness) && v.y<(ll/m_numGridlines+m_thickness) ){
                    redVal=0.0f;
                    greenVal=0.0f;
                    blueVal=0.0f;
                    break;
                    }
                }
                
                if(a>m_radius-m_thickness && a<m_radius+m_thickness){
                    redVal=0.0f;
                    greenVal=0.0f;
                    blueVal=0.0f;
                } 


                TextureCoord tc;
                tc.a = redVal;
                tc.b = greenVal;
                tc.c = blueVal;
                tc.d = alpha;
                mesh.textureCoords.add(tc);

                int index1 = kk * in[kk].size() + jj + 1;
                int index2 = index1 + in.size();
                int index3 = index1 + 1;
                int index4 = index2 + 1;

                if (!(kk == in.size() - 1 || jj == in[kk].size() - 1))
                {
                    String face;
                    face = String(index1) + "/" + String(index1) + "/" + String(index1) + " " +
                        String(index2) + "/" + String(index2) + "/" + String(index2) + " " +
                        String(index3) + "/" + String(index3) + "/" + String(index3);
                    faceGroup.add(Face(face.getCharPointer()));

                    face = String(index2) + "/" + String(index2) + "/" + String(index2) + " " +
                        String(index3) + "/" + String(index3) + "/" + String(index3) + " " +
                        String(index4) + "/" + String(index4) + "/" + String(index4);
                    faceGroup.add(Face(face.getCharPointer()));
                }
                ++counter;
            }
        }

        if (auto* shape = parseFaceGroup(mesh, faceGroup, lastMaterial, lastName))
            shapes.add(shape);

        return juce::Result::ok();
    }

    juce::Result parseMaterial(juce::Array<Material>& materials, const juce::String& filename)
    {
        jassert(sourceFile.exists());
        auto f = sourceFile.getSiblingFile(filename);

        if (!f.exists())
            return juce::Result::fail("Cannot open file: " + filename);

        auto lines = juce::StringArray::fromLines(f.loadFileAsString());

        materials.clear();
        Material material;

        for (auto line : lines)
        {
            auto l = line.getCharPointer().findEndOfWhitespace();

            if (matchToken(l, "newmtl")) { materials.add(material); material.name = juce::String(l).trim(); continue; }

            if (matchToken(l, "Ka")) { material.ambient = parseVertex(l); continue; }
            if (matchToken(l, "Kd")) { material.diffuse = parseVertex(l); continue; }
            if (matchToken(l, "Ks")) { material.specular = parseVertex(l); continue; }
            if (matchToken(l, "Kt")) { material.transmittance = parseVertex(l); continue; }
            if (matchToken(l, "Ke")) { material.emission = parseVertex(l); continue; }
            if (matchToken(l, "Ni")) { material.refractiveIndex = parseFloat(l);  continue; }
            if (matchToken(l, "Ns")) { material.shininess = parseFloat(l);  continue; }

            if (matchToken(l, "map_Ka")) { material.ambientTextureName = juce::String(l).trim(); continue; }
            if (matchToken(l, "map_Kd")) { material.diffuseTextureName = juce::String(l).trim(); continue; }
            if (matchToken(l, "map_Ks")) { material.specularTextureName = juce::String(l).trim(); continue; }
            if (matchToken(l, "map_Ns")) { material.normalTextureName = juce::String(l).trim(); continue; }

            auto tokens = juce::StringArray::fromTokens(l, " \t", "");

            if (tokens.size() >= 2)
                material.parameters.set(tokens[0].trim(), tokens[1].trim());
        }

        materials.add(material);
        return juce::Result::ok();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WavefrontObjFile)
};
