

# The top-level CMakeLists.txt file for a project must contain a literal, direct call to the
# `project()` command. `project()` sets up some helpful variables that describe source/binary
# directories, and the current project version. This is a standard CMake command.

project(TRANSFERFUNCPLOTTER VERSION 0.0.1)

# `VST3_CATEGORIES`
#  - Should be one or more, separated by spaces, of the following: `Fx`, `Instrument`, `Analyzer`,
#    `Delay`, `Distortion`, `Drum`, `Dynamics`, `EQ`, `External`, `Filter`, `Generator`, `Mastering`,
#    `Modulation`, `Mono`, `Network`, `NoOfflineProcess`, `OnlyOfflineProcess`, `OnlyRT`,
#    `Pitch Shift`, `Restoration`, `Reverb`, `Sampler`, `Spatial`, `Stereo`, `Surround`, `Synth`,
#    `Tools`, `Up-Downmix`

#- `AU_MAIN_TYPE`
#  - Should be one of: `kAudioUnitType_Effect`, `kAudioUnitType_FormatConverter`,
#    `kAudioUnitType_Generator`, `kAudioUnitType_MIDIProcessor`, `kAudioUnitType_Mixer`,
#    `kAudioUnitType_MusicDevice`, `kAudioUnitType_MusicEffect`, `kAudioUnitType_OfflineEffect`,
#    `kAudioUnitType_Output`, `kAudioUnitType_Panner`

# add_compile_definitions(FACTORY_PRESETS) # use this if you have finally some presets to add (see binary files below)
# add_compile_definitions(WITH_MIDIKEYBOARD)

juce_add_plugin(AudioPluginTransferFunc
    # VERSION ...                               # Set this if the plugin version is different to the project version
    # ICON_BIG ...                              # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
    # ICON_SMALL ...
    # COMPANY_NAME ...                          # Specify the name of the plugin's author
    COMPANY_NAME  "Jade Hochschule"             # Specify the name of the plugin's author
    IS_SYNTH FALSE                       # Is this a synth or an effect?
    NEEDS_MIDI_INPUT FALSE               # Does the plugin need midi input?
    NEEDS_MIDI_OUTPUT FALSE              # Does the plugin need midi output?
    # IS_MIDI_EFFECT TRUE/FALSE                 # Is this plugin a MIDI effect?
    # EDITOR_WANTS_KEYBOARD_FOCUS TRUE/FALSE    # Does the editor need keyboard focus?
    VST3_CATEGORIES "Fx" "Analyzer"                        
    AU_MAIN_TYPE "kAudioUnitType_Effect"
    COPY_PLUGIN_AFTER_BUILD TRUE               # Should the plugin be installed to a default location after building?
    PLUGIN_MANUFACTURER_CODE IHAJ               # A four-character manufacturer id with at least one upper-case character
    PLUGIN_CODE tfp1                            # A unique four-character plugin id with at least one upper-case character
    FORMATS AU VST3 Standalone                  # The formats to build. Other valid formats are: AAX Unity VST AU AUv3
    PRODUCT_NAME "TransferFuncVisualizer")     # The name of the final executable, which can differ from the target name
#
# `juce_generate_juce_header` will create a JuceHeader.h for a given target, which will be generated
# into your build tree. This should be included with `#include <JuceHeader.h>`. The include path for
# this header will be automatically added to the target. The main function of the JuceHeader is to
# include all your JUCE module headers; if you're happy to include module headers directly, you
# probably don't need to call this.

# juce_generate_juce_header(AudioPluginExample)
juce_generate_juce_header(AudioPluginTransferFunc)

# `target_sources` adds source files to a target. We pass the target that needs the sources as the
# first argument, then a visibility parameter for the sources which should normally be PRIVATE.
# Finally, we supply a list of source files that will be built into the target. This is a standard
# CMake command.


target_sources(AudioPluginTransferFunc
    PRIVATE
        BodeMagnitudeComponent.cpp
        BodePhaseComponent.cpp
        MainComponent.cpp
        PNcontrolComponent.cpp
        PNplot.cpp
        PresetHandler.cpp
        OpenGLAppClass.h
        #juce_Draggable3DOrientation.h
        #juce_opengl.cpp
        #juce_OpenGLAppComponent.cpp
        #juce_OpenGLGraphicsContext.cpp
        TransferFunLookAndFeel.cpp
        PluginEditor.cpp
        PluginProcessor.cpp
        BrickwallLimiter.cpp
        SimpleMeter.cpp
        #${TGMLIBCPPS}
        )
  


target_compile_definitions(AudioPluginTransferFunc
    PUBLIC
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_REPORT_APP_USAGE=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

# If your target needs extra binary assets, you can add them here. The first argument is the name of
# a new static library target that will include all the binary resources. There is an optional
# `NAMESPACE` argument that can specify the namespace of the generated binary data class. Finally,
# the SOURCES argument should be followed by a list of source files that should be built into the
# static library. These source files can be of any kind (wav data, images, fonts, icons etc.).
# Conversion to binary-data will happen when your target is built.

# juce_add_binary_data(AudioPluginData SOURCES ...)

# `target_link_libraries` links libraries and JUCE modules to other libraries or executables. Here,
# we're linking our executable target to the `juce::juce_audio_utils` module. Inter-module
# dependencies are resolved automatically, so `juce_core`, `juce_events` and so on will also be
# linked automatically. If we'd generated a binary data target above, we would need to link to it
# here too. This is a standard CMake command.



# use this code for binaries (e.g. presets)
#file(GLOB TEMPLATE_RESOURCES_GLOB
#  res/*.xml
#  )

#juce_add_binary_data( AudioPluginJadeSpectrogram-binary
#  SOURCES ${TEMPLATE_RESOURCES_GLOB}
#)
#if( UNIX AND NOT APPLE )
#set_target_properties(AudioPluginJadeSpectrogram-binary PROPERTIES
#    POSITION_INDEPENDENT_CODE TRUE)
#endif()

juce_add_binary_data(AudioPluginTransferFunc-binary 
    SOURCES 
        ../Resources/images/LogoJadeHochschuleTrans.png
        ../Resources/images/AboutBox.jpg)


if( UNIX AND NOT APPLE )
set_target_properties(AudioPluginTransferFunc-binary PROPERTIES
    POSITION_INDEPENDENT_CODE TRUE)
endif()

target_link_libraries(AudioPluginTransferFunc
    PRIVATE
        # AudioPluginData           # If we'd created a binary data target, we'd link to it here
        AudioPluginTransferFunc-binary
        juce::juce_audio_utils
        juce::juce_opengl        
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
