/*
  ==============================================================================

    PluginGUISettings.h

    Authors:    Joerg Bitzer (JB)
    
    SPDX-License-Identifier: BSD-3-Clause
 ==============================================================================
*/

#pragma once

#define PRESETHANDLER_HEIGHT 30
#define MAINCOMPONENT_WIDTH 800
#define MAINCOMPONENT_HEIGHT 500

#define WINDOW_WIDTH MAINCOMPONENT_WIDTH
#define WINDOW_HEIGHT MAINCOMPONENT_HEIGHT + PRESETHANDLER_HEIGHT

const int g_maxGuiSize_x(2*WINDOW_WIDTH);
const int g_minGuiSize_y(WINDOW_HEIGHT);
const int g_minGuiSize_x(WINDOW_WIDTH);
const float g_guiratio = float(g_minGuiSize_y)/g_minGuiSize_x;

