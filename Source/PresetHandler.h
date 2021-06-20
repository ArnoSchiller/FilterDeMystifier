/*
  ==============================================================================

    PresetHandler.h
    Created: 3 Jun 2020 11:35:05am
    
    Authors:    Joerg Bitzer (JB)

    SPDX-License-Identifier: BSD-3-Clause

	// Version 1.0.1 18.06.20 JB: color in save button changed to jadeGrey and JadeRed
								  new dependency from JadeLookAndFeel
	// Version 1.1.1 11.01.20 JB: Added Handling of Factory presets, 
								  changed how to create categories (more flexibel)
								  automation of category combobox, if no categories are set
	// Version 1.1.2 14.01.20 JB: improved category handling (categories can be set and 
								  save (no unknown categories-> set to Unknown))

	// Version 1.2.0 18.01.20 JB: added submenus for categories (if provided)

  ==============================================================================
*/
/*
	Ideas: delete a preset by a special menu entry on right clock button on a preset entry (savety measure)

	Todo: (if necessary): How to update Presets from different versions: Possible Solution would be a special converter for 
	each version of a plugin. Is there a general pattern, to do it hiere in the presetHAndler.

*/

#pragma once

#include <JuceHeader.h>

/*#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_events/juce_events.h>
#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
//*/
#include <list>
#include <vector>

const int g_maxNumberOfCategories = 20;

class PresetHandler
{
public:
//	const std::vector<std::string> Categories;

	PresetHandler();
	int setAudioValueTreeState(AudioProcessorValueTreeState* vts);
	int addPreset(ValueTree& newpreset);
	int addOrChangeCurrentPreset(String name, String category = "Unknown", String bank = "User");
	ValueTree getPreset(String name);
	int changePreset(ValueTree& newpreset);
	int changePresetCategory(String name, String category);

	int deletePreset(ValueTree& newpreset);
	File getUserPresetsFolder(bool & wasCreated);
	File getFactoryPresetsFolder();
	int savePreset(String name, String category = "Unknown", String bank = "User");
	int savePreset(ValueTree& vt);
	ValueTree loadPreset(String name);
	int loadPresetAndActivate(String name);
	int deletePresetFile(String name);
	int loadfromFileAllUserPresets();
	int getNrOfPresets() { return m_presetList.size(); };
	int getAllKeys(std::vector<String>& keys, std::vector<String>& presetcats);

	bool isAlreadyAPreset(String name)
	{
		return m_presetList.count(name);
	}
	bool isAValidCategory(String category, int &position)
	{
		
		auto it = std::find(m_categoryList.begin(), m_categoryList.end(), category);
 
    	// If element was found
    	if (it != m_categoryList.end()) 
    	{
        	// calculating the index of K
        	position = it - m_categoryList.begin();
			return true;
	    }
    	else 
		{
			position = -1;
			return false;
	    }		
	}

// new methods for categories
	void addCategory(String newCat);
	void addCategory(StringArray newCat);
	bool gethasCategories(){return hasCategories;};
	std::vector<String> m_categoryList;

// Factory Presets 
#ifdef FACTORY_PRESETS
	void DeployFactoryPresets();
#endif
private:
	AudioProcessorValueTreeState* m_vts;
	std::map <String, ValueTree> m_presetList;

	bool hasCategories;

	void repairCategory(ValueTree& vt)
	{
		String category = vt.getProperty("category");
		int pos;
		bool isCat = isAValidCategory(category,pos);
		if (isCat)
		{
			vt.setProperty("category", category, nullptr);
		}
		else
		{
			vt.setProperty("category", "Unknown", nullptr);
		}

	}

};

class PresetComponent : public Component
{
public:
	PresetComponent(PresetHandler&);
	void paint(Graphics& g) override;
	void resized() override;
	void setSomethingChanged() {
		m_somethingchanged = true; repaint();
	};
	void setNoCategory();
	
    std::function<void()> somethingChanged;

private:
	ComboBox m_presetCombo;
	PopupMenu m_popupCat[g_maxNumberOfCategories];
	TextButton m_nextButton;
	TextButton m_prevButton;
	TextButton m_saveButton;
	ComboBox m_categoriesCombo;

	PresetHandler& m_presetHandler;

	void nextButtonClick();
	void prevButtonClick();
	void itemchanged();
	void categorychanged();
	void savePreset();

	String m_oldcatname;
	bool m_somethingchanged;
	bool m_hidecategory;

	void buildPresetCombo();
};