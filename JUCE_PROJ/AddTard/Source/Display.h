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
#include "engine.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class Display  : public Component,
                  public Slider::Listener
                  public Button::Listener
{
public:
    //==============================================================================
    Display ();
    ~Display() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void sliderValueChanged (Slider* sliderThatWasMoved) override;
    void buttonClicked (Button* buttonThatWasClicked) override;
    void updateSliders();
    void setCurrVoice(int newVoice);
    int getCurrVoice();

private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    // displayLookAndFeel dispLAF;
    Engine* synth;
    int currVoice;
    std::vector<Slider*> harmonics;
    std::unique_ptr<TextButton> gainLFO;
    //[/UserVariables]

    //==============================================================================


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Display)
};

// class displayLookAndFeel : public LookAndFeel_V4
// {
//   public:
//     displayLookAndFeel()
//     {
//         void drawLinearSlider(Graphics&, int x, int y, int width, int height, 
//                               float sliderPos, float minSliderPos,
//                               float maxSliderpos, const Slider::SliderStyle,
//                               Slider& slider) override
//         {

//         }

//     }
// }

//[EndFile] You can add extra defines here...
//[/EndFile]

