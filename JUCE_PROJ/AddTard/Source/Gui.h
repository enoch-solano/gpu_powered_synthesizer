/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 5.4.7

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2017 - ROLI Ltd.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include <JuceHeader.h>
#include "Display.h"
#include "engine.h"
//#include <juce_Drawable.h>
//#include <juce_DrawableButton.h>
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class Gui  : public Component,
             public Slider::Listener,
             public Button::Listener
{
public:
    //==============================================================================
    Gui ();
    ~Gui() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void sliderValueChanged (Slider* sliderThatWasMoved) override;
    void buttonClicked (Button* buttonThatWasClicked) override;
      Engine* synth = NULL;

    enum voiceColors {v1On = 0xff42a2c8, v2On = 0xffc87c42, v3On = 0xff02c86a, v4On = 0xffc453b1,
                      v1Off = 0xff36677a, v2Off = 0xff7e6447, v3Off = 0xff346d52, v4Off = 0xff6f3966};


private:
    //[UserVariables]   -- You can add your own custom variables in this section.

    std::unique_ptr<DrawableButton> voice1Select;
    std::unique_ptr<DrawableButton> voice2Select;
    std::unique_ptr<DrawableButton> voice3Select;
    std::unique_ptr<DrawableButton> voice4Select;
    std::unique_ptr<DrawableButton> voice1Mute;
    std::unique_ptr<DrawableButton> voice2Mute;
    std::unique_ptr<DrawableButton> voice3Mute;
    std::unique_ptr<DrawableButton> voice4Mute;
    std::vector<DrawableButton*> voiceButtons {};

    std::unique_ptr<Display> display;






    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<GroupComponent> groupComponent5;
    std::unique_ptr<GroupComponent> groupComponent6;
    std::unique_ptr<GroupComponent> groupComponent;
    std::unique_ptr<Slider> voice1lvl;
    std::unique_ptr<Slider> voice1freq;
    std::unique_ptr<GroupComponent> groupComponent2;
    std::unique_ptr<Slider> voice2lvl;
    std::unique_ptr<Slider> voice2freq;
    std::unique_ptr<GroupComponent> groupComponent3;
    std::unique_ptr<Slider> voice3lvl;
    std::unique_ptr<Slider> voice3freq;
    std::unique_ptr<GroupComponent> groupComponent4;
    std::unique_ptr<Slider> voice4lvl;
    std::unique_ptr<Slider> voice4freq;
    std::unique_ptr<Slider> atk;
    std::unique_ptr<Slider> dec;
    std::unique_ptr<Slider> sus;
    std::unique_ptr<Slider> rel;
    std::unique_ptr<Slider> lfoRate;
    std::unique_ptr<Slider> lfoAmt;
    std::unique_ptr<Slider> lfoShape;
    std::unique_ptr<Slider> fltrType;
    std::unique_ptr<Slider> fltrFreq;
    std::unique_ptr<Slider> fltrPeak;
    std::unique_ptr<Slider> fmSelect;
    std::unique_ptr<Slider> masterLvl;
    std::unique_ptr<TextButton> textButton;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Gui)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

