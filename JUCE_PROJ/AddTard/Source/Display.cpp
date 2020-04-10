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

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "Display.h"
#include "Gui.h"
#include "engine.h"

//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
Display::Display ()
{
    currVoice = 0.0;
    for (int i = 0; i < 32; i++)
    {
        std::string name = "Harmonic ";
        name+= std::to_string(i);
        harmonics.push_back(new Slider (name));
        addAndMakeVisible (harmonics[i]);
        harmonics[i]->setRange (0, 1, 0);
        harmonics[i]->setSliderStyle (Slider::LinearVertical);
        harmonics[i]->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);
        harmonics[i]->setColour (Slider::thumbColourId, Colour (0xffcbdc3b));
        harmonics[i]->addListener (this);
    }

    synth = Engine::getInstance();
    //[/Constructor_pre]


    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

Display::~Display()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    for (int i =0; i < 32; i++)
    {
        harmonics[i] = nullptr;
    }
    //[/Destructor_pre]



    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void Display::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..

    //[/UserPrePaint]

    g.fillAll (Colours::black);

    //[UserPaint] Add your own custom painting code here..
    Path outline;
    Rectangle<float> rect (600, 400);
    g.setColour(static_cast<Colour> (0xffcbdc3b));
    g.strokePath(outline, PathStrokeType(2));
    //[/UserPaint]
}

void Display::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    for (int i = 0; i < 32; i++) {
        harmonics[i]->setBounds((44+i*16), 20, 16, 200);
    }
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void Display::sliderValueChanged (Slider* sliderThatWasMoved)
{
    for (int i = 0; i < 32; i++)
    {
        if (sliderThatWasMoved == harmonics[i])
        {
            synth->update_harmonics(currVoice, i, sliderThatWasMoved->getValue());
        }
    }
}

void Display::updateSliders()
{
    for (int i = 0; i < 32; i++)
    {
        if(currVoice == 0)
        {
            harmonics[i]->setColour(Slider::thumbColourId, static_cast<Colour>(Gui::v1On));
        
        }
        else if (currVoice == 1)
        {
            harmonics[i]->setColour(Slider::thumbColourId, static_cast<Colour>(Gui::v2On));
        }
        else if (currVoice == 2)
        {
            harmonics[i]->setColour(Slider::thumbColourId, static_cast<Colour>(Gui::v3On));
        }
        else if (currVoice == 3)
        {
            harmonics[i]->setColour(Slider::thumbColourId, static_cast<Colour>(Gui::v4On));
        }
        harmonics[i]->setValue(synth->get_gain(currVoice, i));
    }
}

void Display::setCurrVoice(int newVoice)
{
    currVoice = newVoice;
}

int Display::getCurrVoice()
{
    return currVoice;
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="Display" componentName=""
                 parentClasses="public Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="ff323e44"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

