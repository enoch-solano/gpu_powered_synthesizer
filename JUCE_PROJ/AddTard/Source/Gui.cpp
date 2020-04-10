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

#include "Gui.h"
#include "Display.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...

//[/MiscUserDefs]

//==============================================================================
Gui::Gui ()
{
    //[Constructor_pre] You can add your own custom stuff here..

    auto midiDevices = MidiInput::getAvailableDevices();
    for (int i = 0; i < midiDevices.size(); i++ )
    {
        std::cout<<midiDevices[i].name<<std::endl;
    }

    display.reset(new Display());
    addAndMakeVisible(display.get());
    display->setCurrVoice(0);
    display->updateSliders();
    //path creation for all drawable buttons
    Path path;
    Rectangle<float> area (47, 48);
    path.addRectangle(area);

    //Instatiate select buttons
    voice1Select.reset (new DrawableButton ("Voice 1 Selection", DrawableButton::ImageRaw));
    addAndMakeVisible(voice1Select.get());
    DrawablePath on1, off1;
    on1.setPath(path);
    on1.setFill(static_cast<Colour>(Gui::v1On));
    off1.setPath(path);
    off1.setFill(static_cast<Colour>(Gui::v1Off));
    voice1Select->setImages(&off1, &off1, &on1, nullptr, &on1, &on1, &off1, nullptr);
    voice1Select->setRadioGroupId(1);
    voice1Select->addListener(this);

    voice2Select.reset (new DrawableButton ("Voice 2 Selection", DrawableButton::ImageRaw));
    addAndMakeVisible(voice2Select.get());
    DrawablePath on2, off2;
    on2.setPath(path);
    on2.setFill(static_cast<Colour>(Gui::v2On));
    off2.setPath(path);
    off2.setFill(static_cast<Colour>(Gui::v2Off));
    voice2Select->setImages(&off2, &off2, &on2, nullptr, &on2, &on2, &off2, nullptr);
    voice2Select->setRadioGroupId(1);
    voice2Select->addListener(this);

    voice3Select.reset (new DrawableButton ("Voice 3 Selection", DrawableButton::ImageRaw));
    addAndMakeVisible(voice3Select.get());
    DrawablePath on3, off3;
    on3.setPath(path);
    on3.setFill(static_cast<Colour>(Gui::v3On));
    off3.setPath(path);
    off3.setFill(static_cast<Colour>(Gui::v3Off));
    voice3Select->setImages(&off3, &off3, &on3, nullptr, &on3, &on3, &off3, nullptr);
    voice3Select->setRadioGroupId(1);
    voice3Select->addListener(this);

    voice4Select.reset (new DrawableButton ("Voice 4 Selection", DrawableButton::ImageRaw));
    addAndMakeVisible(voice4Select.get());
    DrawablePath on4, off4;
    on4.setPath(path);
    on4.setFill(static_cast<Colour>(Gui::v4On));
    off4.setPath(path);
    off4.setFill(static_cast<Colour>(Gui::v4Off));
    voice4Select->setImages(&off4, &off4, &on4, nullptr, &on4, &on4, &off4, nullptr);
    voice4Select->setRadioGroupId(1);
    voice4Select->addListener(this);

    voiceButtons.push_back(voice1Select.get());
    voiceButtons.push_back(voice2Select.get());
    voiceButtons.push_back(voice3Select.get());
    voiceButtons.push_back(voice4Select.get());


    voice1Mute.reset(new DrawableButton ("Voice 1 mute", DrawableButton::ImageRaw));
    addAndMakeVisible(voice1Mute.get());
    DrawablePath onMute, offMute;
    onMute.setPath(path);
    onMute.setFill(static_cast<Colour>(0xffc0bebe));
    offMute.setPath(path);
    offMute.setFill(static_cast<Colour>(0xff6d6d6d));
    voice1Mute->setImages(&offMute, &offMute, &onMute, nullptr, &onMute, &onMute, &offMute, nullptr);
    voice1Mute->addListener(this);

    voice2Mute.reset(new DrawableButton ("Voice 2 mute", DrawableButton::ImageRaw));
    addAndMakeVisible(voice2Mute.get());
    DrawablePath onMute2, offMute2;
    onMute2.setPath(path);
    onMute2.setFill(static_cast<Colour>(0xffc0bebe));
    offMute2.setPath(path);
    offMute2.setFill(static_cast<Colour>(0xff6d6d6d));
    voice2Mute->setImages(&offMute2, &offMute2, &onMute2, nullptr, &onMute2, &onMute2, &offMute2, nullptr);
    voice2Mute->addListener(this);

    voice3Mute.reset(new DrawableButton ("Voice 3 mute", DrawableButton::ImageRaw));
    addAndMakeVisible(voice3Mute.get());
    DrawablePath onMute3, offMute3;
    onMute3.setPath(path);
    onMute3.setFill(static_cast<Colour>(0xffc0bebe));
    offMute3.setPath(path);
    offMute3.setFill(static_cast<Colour>(0xff6d6d6d));
    voice3Mute->setImages(&offMute3, &offMute3, &onMute3, nullptr, &onMute3, &onMute3, &offMute3, nullptr);
    voice3Mute->addListener(this);

    voice4Mute.reset(new DrawableButton ("Voice 4 mute", DrawableButton::ImageRaw));
    addAndMakeVisible(voice4Mute.get());
    DrawablePath onMute4, offMute4;
    onMute4.setPath(path);
    onMute4.setFill(static_cast<Colour>(0xffc0bebe));
    offMute4.setPath(path);
    offMute4.setFill(static_cast<Colour>(0xff6d6d6d));
    voice4Mute->setImages(&offMute4, &offMute4, &onMute4, nullptr, &onMute4, &onMute4, &offMute4, nullptr);
    voice4Mute->addListener(this);







    //[/Constructor_pre]

    groupComponent5.reset (new GroupComponent ("new group",
                                               TRANS("LFO")));
    addAndMakeVisible (groupComponent5.get());
    groupComponent5->setTextLabelPosition (Justification::centred);
    groupComponent5->setColour (GroupComponent::outlineColourId, Colour (0xffcbdc3c));

    groupComponent6.reset (new GroupComponent ("new group",
                                               TRANS("Filter")));
    addAndMakeVisible (groupComponent6.get());
    groupComponent6->setTextLabelPosition (Justification::centred);
    groupComponent6->setColour (GroupComponent::outlineColourId, Colour (0xffcbdc3c));

    groupComponent.reset (new GroupComponent ("Voice 1",
                                              TRANS("Voice 1")));
    addAndMakeVisible (groupComponent.get());
    groupComponent->setExplicitFocusOrder (1);
    groupComponent->setTextLabelPosition (Justification::centred);
    groupComponent->setColour (GroupComponent::outlineColourId, Colour (0xff36677a));

    voice1lvl.reset (new Slider ("Voice 1 Level"));
    addAndMakeVisible (voice1lvl.get());
    voice1lvl->setRange (0, 10, 0);
    voice1lvl->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    voice1lvl->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    voice1lvl->addListener (this);

    voice1freq.reset (new Slider ("Voice 1 Freq"));
    addAndMakeVisible (voice1freq.get());
    voice1freq->setRange (0, 10, 0);
    voice1freq->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    voice1freq->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    voice1freq->addListener (this);

    groupComponent2.reset (new GroupComponent ("Voice 1",
                                               TRANS("Voice 2")));
    addAndMakeVisible (groupComponent2.get());
    groupComponent2->setExplicitFocusOrder (1);
    groupComponent2->setTextLabelPosition (Justification::centred);
    groupComponent2->setColour (GroupComponent::outlineColourId, Colour (0xff7e643b));

    voice2lvl.reset (new Slider ("Voice 2 Level"));
    addAndMakeVisible (voice2lvl.get());
    voice2lvl->setRange (0, 10, 0);
    voice2lvl->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    voice2lvl->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    voice2lvl->setColour (Slider::thumbColourId, Colour (0xffc87c42));
    voice2lvl->addListener (this);

    voice2freq.reset (new Slider ("Voice 2 Freq"));
    addAndMakeVisible (voice2freq.get());
    voice2freq->setRange (0, 10, 0);
    voice2freq->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    voice2freq->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    voice2freq->setColour (Slider::thumbColourId, Colour (0xffc87c42));
    voice2freq->addListener (this);

    groupComponent3.reset (new GroupComponent ("Voice 3",
                                               TRANS("Voice 3")));
    addAndMakeVisible (groupComponent3.get());
    groupComponent3->setExplicitFocusOrder (1);
    groupComponent3->setTextLabelPosition (Justification::centred);
    groupComponent3->setColour (GroupComponent::outlineColourId, Colour (0xff346d52));

    voice3lvl.reset (new Slider ("Voice 3 level"));
    addAndMakeVisible (voice3lvl.get());
    voice3lvl->setRange (0, 10, 0);
    voice3lvl->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    voice3lvl->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    voice3lvl->setColour (Slider::thumbColourId, Colour (0xff02c86a));
    voice3lvl->addListener (this);

    voice3freq.reset (new Slider ("Voice 3 Freq"));
    addAndMakeVisible (voice3freq.get());
    voice3freq->setRange (0, 10, 0);
    voice3freq->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    voice3freq->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    voice3freq->setColour (Slider::thumbColourId, Colour (0xff02c86a));
    voice3freq->addListener (this);

    groupComponent4.reset (new GroupComponent ("Voice 4",
                                               TRANS("Voice 4")));
    addAndMakeVisible (groupComponent4.get());
    groupComponent4->setExplicitFocusOrder (1);
    groupComponent4->setTextLabelPosition (Justification::centred);
    groupComponent4->setColour (GroupComponent::outlineColourId, Colour (0xff6f3966));

    voice4lvl.reset (new Slider ("Voice 4 level"));
    addAndMakeVisible (voice4lvl.get());
    voice4lvl->setRange (0.0, 1, 0);
    voice4lvl->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    voice4lvl->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    voice4lvl->setColour (Slider::thumbColourId, Colour (0xffc453b1));
    voice4lvl->addListener (this);

    voice4freq.reset (new Slider ("Voice 4 Freq"));
    addAndMakeVisible (voice4freq.get());
    voice4freq->setRange (10, 1000, 0);
    voice4freq->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    voice4freq->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    voice4freq->setColour (Slider::thumbColourId, Colour (0xffc453b1));
    voice4freq->addListener (this);

    atk.reset (new Slider ("Attack"));
    addAndMakeVisible (atk.get());
    atk->setRange (0, 10, 0);
    atk->setSliderStyle (Slider::LinearVertical);
    atk->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);
    atk->setColour (Slider::thumbColourId, Colour (0xffcbdc3b));
    atk->addListener (this);

    dec.reset (new Slider ("Decay"));
    addAndMakeVisible (dec.get());
    dec->setRange (0, 10, 0);
    dec->setSliderStyle (Slider::LinearVertical);
    dec->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);
    dec->setColour (Slider::thumbColourId, Colour (0xffcbdc3b));
    dec->addListener (this);

    sus.reset (new Slider ("Sustain"));
    addAndMakeVisible (sus.get());
    sus->setRange (0, 10, 0);
    sus->setSliderStyle (Slider::LinearVertical);
    sus->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);
    sus->setColour (Slider::thumbColourId, Colour (0xffcbdc3b));
    sus->addListener (this);

    rel.reset (new Slider ("Release"));
    addAndMakeVisible (rel.get());
    rel->setRange (0, 10, 0);
    rel->setSliderStyle (Slider::LinearVertical);
    rel->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);
    rel->setColour (Slider::thumbColourId, Colour (0xffcbdc3b));
    rel->addListener (this);

    lfoRate.reset (new Slider ("LFO Rate"));
    addAndMakeVisible (lfoRate.get());
    lfoRate->setRange (0, 10, 0);
    lfoRate->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    lfoRate->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);
    lfoRate->setColour (Slider::thumbColourId, Colour (0xffcbdc3b));
    lfoRate->addListener (this);

    lfoAmt.reset (new Slider ("LFO Amount"));
    addAndMakeVisible (lfoAmt.get());
    lfoAmt->setRange (0, 10, 0);
    lfoAmt->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    lfoAmt->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);
    lfoAmt->setColour (Slider::thumbColourId, Colour (0xffcbdc3b));
    lfoAmt->addListener (this);

    lfoShape.reset (new Slider ("LFO Waveform Type"));
    addAndMakeVisible (lfoShape.get());
    lfoShape->setRange (0, 10, 0);
    lfoShape->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    lfoShape->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);
    lfoShape->setColour (Slider::thumbColourId, Colour (0xffcbdc3b));
    lfoShape->addListener (this);

    fltrType.reset (new Slider ("Filter Type"));
    addAndMakeVisible (fltrType.get());
    fltrType->setRange (0, 10, 0);
    fltrType->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    fltrType->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);
    fltrType->setColour (Slider::thumbColourId, Colour (0xffcbdc3b));
    fltrType->addListener (this);

    fltrFreq.reset (new Slider ("Filter Frequency"));
    addAndMakeVisible (fltrFreq.get());
    fltrFreq->setRange (0, 10, 0);
    fltrFreq->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    fltrFreq->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);
    fltrFreq->setColour (Slider::thumbColourId, Colour (0xffcbdc3b));
    fltrFreq->addListener (this);

    fltrPeak.reset (new Slider ("Filter Peak"));
    addAndMakeVisible (fltrPeak.get());
    fltrPeak->setRange (0, 10, 0);
    fltrPeak->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    fltrPeak->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);
    fltrPeak->setColour (Slider::thumbColourId, Colour (0xffcbdc3b));
    fltrPeak->addListener (this);

    fmSelect.reset (new Slider ("FM Type Selection"));
    addAndMakeVisible (fmSelect.get());
    fmSelect->setRange (0, 10, 0);
    fmSelect->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    fmSelect->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);
    fmSelect->setColour (Slider::thumbColourId, Colour (0xffcbdc3b));
    fmSelect->addListener (this);

    masterLvl.reset (new Slider ("Master Outut Level"));
    addAndMakeVisible (masterLvl.get());
    masterLvl->setRange (0, 10, 0);
    masterLvl->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    masterLvl->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);
    masterLvl->setColour (Slider::thumbColourId, Colour (0xffcbdc3b));
    masterLvl->addListener (this);

    textButton.reset (new TextButton ("new button"));
    addAndMakeVisible (textButton.get());
    textButton->setButtonText (String());
    textButton->addListener (this);
    textButton->setColour (TextButton::buttonColourId, Colour (0x00a45c94));

    textButton->setBounds (696, 48, 150, 24);

    adsrSliders.push_back(atk.get());
    adsrSliders.push_back(dec.get());
    adsrSliders.push_back(sus.get());
    adsrSliders.push_back(rel.get());

    //[UserPreSize]
    //[/UserPreSize]

    setSize (1600, 900);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

Gui::~Gui()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    groupComponent5 = nullptr;
    groupComponent6 = nullptr;
    groupComponent = nullptr;
    voice1lvl = nullptr;
    voice1freq = nullptr;
    groupComponent2 = nullptr;
    voice2lvl = nullptr;
    voice2freq = nullptr;
    groupComponent3 = nullptr;
    voice3lvl = nullptr;
    voice3freq = nullptr;
    groupComponent4 = nullptr;
    voice4lvl = nullptr;
    voice4freq = nullptr;
    atk = nullptr;
    dec = nullptr;
    sus = nullptr;
    rel = nullptr;
    lfoRate = nullptr;
    lfoAmt = nullptr;
    lfoShape = nullptr;
    fltrType = nullptr;
    fltrFreq = nullptr;
    fltrPeak = nullptr;
    fmSelect = nullptr;
    masterLvl = nullptr;
    textButton = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    voice1Select = nullptr;
    voice2Select = nullptr;
    voice3Select = nullptr;
    voice4Select = nullptr;
    voice1Mute = nullptr;
    voice2Mute = nullptr;
    voice3Mute = nullptr;
    voice4Mute = nullptr;

    display = nullptr;

    //[/Destructor]
}

//==============================================================================
void Gui::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xff3c3d3e));

    {
        int x = proportionOfWidth (0.3700f), y = proportionOfHeight (0.6311f), width = proportionOfWidth (0.0494f), height = proportionOfHeight (0.0333f);
        String text (TRANS("ATTACK"));
        Colour fillColour = Colour (0xffcbdc3b);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular").withExtraKerningFactor (0.147f));
        g.drawText (text, x, y, width, height,
                    Justification::centred, true);
    }

    {
        int x = proportionOfWidth (0.4500f), y = proportionOfHeight (0.6311f), width = proportionOfWidth (0.0494f), height = proportionOfHeight (0.0333f);
        String text (TRANS("DECAY"));
        Colour fillColour = Colour (0xffcbdc3b);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular").withExtraKerningFactor (0.164f));
        g.drawText (text, x, y, width, height,
                    Justification::centred, true);
    }

    {
        int x = proportionOfWidth (0.5250f), y = proportionOfHeight (0.6311f), width = proportionOfWidth (0.0494f), height = proportionOfHeight (0.0333f);
        String text (TRANS("SUSTAIN"));
        Colour fillColour = Colour (0xffcbdc3b);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular").withExtraKerningFactor (0.130f));
        g.drawText (text, x, y, width, height,
                    Justification::centred, true);
    }

    {
        int x = proportionOfWidth (0.6000f), y = proportionOfHeight (0.6311f), width = proportionOfWidth (0.0494f), height = proportionOfHeight (0.0333f);
        String text (TRANS("RELEASE"));
        Colour fillColour = Colour (0xffcbdc3b);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular").withExtraKerningFactor (0.113f));
        g.drawText (text, x, y, width, height,
                    Justification::centred, true);
    }

    {
        int x = proportionOfWidth (0.7150f), y = proportionOfHeight (0.4089f), width = proportionOfWidth (0.0494f), height = proportionOfHeight (0.0289f);
        String text (TRANS("FM"));
        Colour fillColour = Colour (0xffcbdc3b);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular").withExtraKerningFactor (0.366f));
        g.drawText (text, x, y, width, height,
                    Justification::centred, true);
    }

    {
        int x = proportionOfWidth (0.7150f), y = proportionOfHeight (0.5067f), width = proportionOfWidth (0.0494f), height = proportionOfHeight (0.0289f);
        String text (TRANS("MASTER"));
        Colour fillColour = Colour (0xffcbdc3b);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular").withExtraKerningFactor (0.172f));
        g.drawText (text, x, y, width, height,
                    Justification::centred, true);
    }

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void Gui::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    groupComponent5->setBounds (proportionOfWidth (0.7013f), proportionOfHeight (0.0656f), proportionOfWidth (0.2275f), proportionOfHeight (0.1322f));
    groupComponent6->setBounds (proportionOfWidth (0.7013f), proportionOfHeight (0.1978f), proportionOfWidth (0.2275f), proportionOfHeight (0.1322f));
    groupComponent->setBounds (proportionOfWidth (0.0250f), proportionOfHeight (0.0656f), proportionOfWidth (0.2969f), proportionOfHeight (0.1033f));
    voice1lvl->setBounds (proportionOfWidth (0.0950f), proportionOfHeight (0.0756f), proportionOfWidth (0.0625f), proportionOfHeight (0.0844f));
    voice1freq->setBounds (proportionOfWidth (0.1862f), proportionOfHeight (0.0756f), proportionOfWidth (0.0625f), proportionOfHeight (0.0844f));
    groupComponent2->setBounds (proportionOfWidth (0.0250f), proportionOfHeight (0.1889f), proportionOfWidth (0.2969f), proportionOfHeight (0.1033f));
    voice2lvl->setBounds (proportionOfWidth (0.0950f), proportionOfHeight (0.2011f), proportionOfWidth (0.0625f), proportionOfHeight (0.0844f));
    voice2freq->setBounds (proportionOfWidth (0.1862f), proportionOfHeight (0.2011f), proportionOfWidth (0.0625f), proportionOfHeight (0.0844f));
    groupComponent3->setBounds (proportionOfWidth (0.0250f), proportionOfHeight (0.3156f), proportionOfWidth (0.2969f), proportionOfHeight (0.1033f));
    voice3lvl->setBounds (proportionOfWidth (0.0950f), proportionOfHeight (0.3289f), proportionOfWidth (0.0625f), proportionOfHeight (0.0844f));
    voice3freq->setBounds (proportionOfWidth (0.1862f), proportionOfHeight (0.3289f), proportionOfWidth (0.0625f), proportionOfHeight (0.0844f));
    groupComponent4->setBounds (proportionOfWidth (0.0250f), proportionOfHeight (0.4456f), proportionOfWidth (0.2969f), proportionOfHeight (0.1033f));
    voice4lvl->setBounds (proportionOfWidth (0.0950f), 416, 79, 72);
    voice4freq->setBounds (proportionOfWidth (0.1862f), proportionOfHeight (0.4589f), proportionOfWidth (0.0625f), proportionOfHeight (0.0844f));
    atk->setBounds (proportionOfWidth (0.3600f), proportionOfHeight (0.5422f), proportionOfWidth (0.0625f), proportionOfHeight (0.0844f));
    dec->setBounds (proportionOfWidth (0.4425f), proportionOfHeight (0.5378f), proportionOfWidth (0.0625f), proportionOfHeight (0.0844f));
    sus->setBounds (proportionOfWidth (0.5181f), proportionOfHeight (0.5378f), proportionOfWidth (0.0625f), proportionOfHeight (0.0844f));
    rel->setBounds (proportionOfWidth (0.5938f), proportionOfHeight (0.5378f), proportionOfWidth (0.0625f), proportionOfHeight (0.0844f));
    lfoRate->setBounds (proportionOfWidth (0.7837f), proportionOfHeight (0.0756f), proportionOfWidth (0.0625f), proportionOfHeight (0.0844f));
    lfoAmt->setBounds (proportionOfWidth (0.8594f), proportionOfHeight (0.0756f), proportionOfWidth (0.0625f), proportionOfHeight (0.0844f));
    lfoShape->setBounds (proportionOfWidth (0.7075f), proportionOfHeight (0.0756f), proportionOfWidth (0.0625f), proportionOfHeight (0.0844f));
    fltrType->setBounds (proportionOfWidth (0.7075f), proportionOfHeight (0.2111f), proportionOfWidth (0.0625f), proportionOfHeight (0.0844f));
    fltrFreq->setBounds (proportionOfWidth (0.7837f), proportionOfHeight (0.2111f), proportionOfWidth (0.0625f), proportionOfHeight (0.0844f));
    fltrPeak->setBounds (proportionOfWidth (0.8594f), proportionOfHeight (0.2111f), proportionOfWidth (0.0625f), proportionOfHeight (0.0844f));
    fmSelect->setBounds (proportionOfWidth (0.7075f), proportionOfHeight (0.3289f), proportionOfWidth (0.0625f), proportionOfHeight (0.0844f));
    masterLvl->setBounds (proportionOfWidth (0.7150f), proportionOfHeight (0.4267f), proportionOfWidth (0.0494f), proportionOfHeight (0.0800f));
    //[UserResized] Add your own custom resize handling here..
    voice1Select->setBounds(proportionOfWidth (0.2656f), proportionOfHeight (0.0844f), 47, 48);
    voice2Select->setBounds(proportionOfWidth (0.2656f), proportionOfHeight (0.2111f), 47, 48);
    voice3Select->setBounds(proportionOfWidth (0.2656f), proportionOfHeight (0.3389f), 47, 48);
    voice4Select->setBounds(proportionOfWidth (0.2656f), proportionOfHeight (0.4678f), 47, 48);
    voice1Mute->setBounds(proportionOfWidth (0.0321f), proportionOfHeight (0.0847f), 47, 48);
    voice2Mute->setBounds(proportionOfWidth (0.0321f), proportionOfHeight (0.2113f), 47, 48);
    voice3Mute->setBounds(proportionOfWidth (0.0321f), proportionOfHeight (0.3388f), 47, 48);
    voice4Mute->setBounds(proportionOfWidth (0.0321f), proportionOfHeight (0.4674f), 47, 48);
    display->setBounds(800-280, 80, 600, 400);
    //[/UserResized]
}

void Gui::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]
    for (int i = 0; i < 4; i++)
    {
        if (sliderThatWasMoved == adsrSliders[i])
        {
            int v_idx = display->getCurrVoice();

        }
    }
    if (sliderThatWasMoved == voice1lvl.get())
    {
        //[UserSliderCode_voice1lvl] -- add your slider handling code here..
       synth->update_voice_gain(0, voice1lvl.get()->getValue());
        //[/UserSliderCode_voice1lvl]
    }
    else if (sliderThatWasMoved == voice1freq.get())
    {
        //[UserSliderCode_voice1freq] -- add your slider handling code here..
       
        //[/UserSliderCode_voice1freq]
    }
    else if (sliderThatWasMoved == voice2lvl.get())
    {
        //[UserSliderCode_voice2lvl] -- add your slider handling code here..
         synth->update_voice_gain(1, voice2lvl.get()->getValue());

        //[/UserSliderCode_voice2lvl]
    }
    else if (sliderThatWasMoved == voice2freq.get())
    {
        //[UserSliderCode_voice2freq] -- add your slider handling code here..
        //[/UserSliderCode_voice2freq]
    }
    else if (sliderThatWasMoved == voice3lvl.get())
    {
        //[UserSliderCode_voice3lvl] -- add your slider handling code here..
         if(synth->get_mute(2))
         {
             synth->update_voice_gain(2, voice3lvl.get()->getValue());
         }
       //[/UserSliderCode_voice3lvl]
    }
    else if (sliderThatWasMoved == voice3freq.get())
    {
        //[UserSliderCode_voice3freq] -- add your slider handling code here..
        //[/UserSliderCode_voice3freq]
    }
    else if (sliderThatWasMoved == voice4lvl.get())
    {
        //[UserSliderCode_voice4lvl] -- add your slider handling code here..
         synth->update_voice_gain(3, voice4lvl.get()->getValue());
        //[/UserSliderCode_voice4lvl]
    }
    else if (sliderThatWasMoved == voice4freq.get())
    {
        //[UserSliderCode_voice4freq] -- add your slider handling code here..
        //[/UserSliderCode_voice4freq]
    }
    // else if (sliderThatWasMoved == atk.get())
    // {
    //     //[UserSliderCode_atk] -- add your slider handling code here..
    //     //[/UserSliderCode_atk]
    // }
    // else if (sliderThatWasMoved == dec.get())
    // {
    //     //[UserSliderCode_dec] -- add your slider handling code here..

    //     //[/UserSliderCode_dec]
    // }
    // else if (sliderThatWasMoved == sus.get())
    // {
    //     //[UserSliderCode_sus] -- add your slider handling code here..
    //     //[/UserSliderCode_sus]
    // }
    // else if (sliderThatWasMoved == rel.get())
    // {
    //     //[UserSliderCode_rel] -- add your slider handling code here..
    //     //[/UserSliderCode_rel]
    // }
    else if (sliderThatWasMoved == lfoRate.get())
    {
        //[UserSliderCode_lfoRate] -- add your slider handling code here..
        synth->set_gain_lfo_rate(lfoRate.get()->getValue() * 2.f);
        //[/UserSliderCode_lfoRate]
    }
    else if (sliderThatWasMoved == lfoAmt.get())
    {
        //[UserSliderCode_lfoAmt] -- add your slider handling code here..
        synth->set_gain_lfo_level(lfoAmt.get()->getValue() / 10.f);
        //[/UserSliderCode_lfoAmt]
    }
    else if (sliderThatWasMoved == lfoShape.get())
    {
        //[UserSliderCode_lfoShape] -- add your slider handling code here..
        synth->set_gain_lfo_type(lfoShape.get()->getValue(), 10.f);
        //[/UserSliderCode_lfoShape]
    }
    else if (sliderThatWasMoved == fltrType.get())
    {
        //[UserSliderCode_fltrType] -- add your slider handling code here..
        //[/UserSliderCode_fltrType]
    }
    else if (sliderThatWasMoved == fltrFreq.get())
    {
        //[UserSliderCode_fltrFreq] -- add your slider handling code here..
        //[/UserSliderCode_fltrFreq]
    }
    else if (sliderThatWasMoved == fltrPeak.get())
    {
        //[UserSliderCode_fltrPeak] -- add your slider handling code here..
        //[/UserSliderCode_fltrPeak]
    }
    else if (sliderThatWasMoved == fmSelect.get())
    {
        //[UserSliderCode_fmSelect] -- add your slider handling code here..
        //[/UserSliderCode_fmSelect]
    }
    else if (sliderThatWasMoved == masterLvl.get())
    {
        //[UserSliderCode_masterLvl] -- add your slider handling code here..
        //[/UserSliderCode_masterLvl]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

void Gui::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
     bool buttonState = buttonThatWasClicked->getToggleState();
    for (int i = 0; i<4; i++) {
        if (buttonThatWasClicked == voiceButtons[i]) {

            buttonState = !buttonState;
            buttonThatWasClicked->setToggleState(buttonState, 0);
            display->setCurrVoice(i);
            display->updateSliders();
        }
    }
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == textButton.get())
    {
        //[UserButtonCode_textButton] -- add your button handler code here..
        auto midiList = MidiInput::getAvailableDevices();
        if (!midiList.isEmpty()) {
            std::cout<<"There is a midi Device Connected"<<std::endl;
        }
        else {
            std::cout<<"no midi devices"<<std::endl;
        }
        //[/UserButtonCode_textButton]
    }

    //[UserbuttonClicked_Post]
    else if (buttonThatWasClicked == voice1Mute.get())
    {
        buttonState = !buttonState;
        buttonThatWasClicked->setToggleState(buttonState, 0);
        synth->toggleMute(1, (float)voice1lvl.get()->getValue());
    }

        else if (buttonThatWasClicked == voice2Mute.get())
    {
        buttonState = !buttonState;
        buttonThatWasClicked->setToggleState(buttonState, 0);
        synth->toggleMute(2, (float)voice2lvl.get()->getValue());
    }

        else if (buttonThatWasClicked == voice3Mute.get())
    {
        buttonState = !buttonState;
        buttonThatWasClicked->setToggleState(buttonState, 0);
        synth->toggleMute(3, (float)voice3lvl.get()->getValue());
    }

        else if (buttonThatWasClicked == voice4Mute.get())
    {
        buttonState = !buttonState;
        buttonThatWasClicked->setToggleState(buttonState, 0);
        synth->toggleMute(4, (float)voice4lvl.get()->getValue());
    }
    // else if (buttonThatWasClicked == trigger.get())
    // {
       
    // }
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
bool Gui::keyPressed(const KeyPress &k) 
{
    if (k == KeyPress::spaceKey)
    {
        //std::cout<<"space pressed"<<std::endl;
    }
}

bool Gui::keyStateChanged(bool isKeyDown)
{
    static auto lastSpaceKeyIsDown = false;

    auto newSpaceKeyIsDown = KeyPress::isKeyCurrentlyDown(KeyPress::spaceKey);

    if (newSpaceKeyIsDown != lastSpaceKeyIsDown)
    {
        lastSpaceKeyIsDown = newSpaceKeyIsDown;
        if(newSpaceKeyIsDown)
        {
            synth->gate_on();
        }
        else 
        {
            synth->gate_off();
        }
    }
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="Gui" componentName="" parentClasses="public Component"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="1600"
                 initialHeight="900">
  <BACKGROUND backgroundColour="ff3c3d3e">
    <TEXT pos="37% 63.111% 4.938% 3.333%" fill="solid: ffcbdc3b" hasStroke="0"
          text="ATTACK" fontname="Default font" fontsize="15.0" kerning="0.147"
          bold="0" italic="0" justification="36"/>
    <TEXT pos="45% 63.111% 4.938% 3.333%" fill="solid: ffcbdc3b" hasStroke="0"
          text="DECAY" fontname="Default font" fontsize="15.0" kerning="0.164"
          bold="0" italic="0" justification="36"/>
    <TEXT pos="52.5% 63.111% 4.938% 3.333%" fill="solid: ffcbdc3b" hasStroke="0"
          text="SUSTAIN" fontname="Default font" fontsize="15.0" kerning="0.13"
          bold="0" italic="0" justification="36"/>
    <TEXT pos="60% 63.111% 4.938% 3.333%" fill="solid: ffcbdc3b" hasStroke="0"
          text="RELEASE" fontname="Default font" fontsize="15.0" kerning="0.113"
          bold="0" italic="0" justification="36"/>
    <TEXT pos="71.5% 40.889% 4.938% 2.889%" fill="solid: ffcbdc3b" hasStroke="0"
          text="FM" fontname="Default font" fontsize="15.0" kerning="0.366"
          bold="0" italic="0" justification="36"/>
    <TEXT pos="71.5% 50.667% 4.938% 2.889%" fill="solid: ffcbdc3b" hasStroke="0"
          text="MASTER" fontname="Default font" fontsize="15.0" kerning="0.172"
          bold="0" italic="0" justification="36"/>
  </BACKGROUND>
  <GROUPCOMPONENT name="new group" id="acdbb82577ef9726" memberName="groupComponent5"
                  virtualName="" explicitFocusOrder="0" pos="70.125% 6.556% 22.75% 13.222%"
                  outlinecol="ffcbdc3c" title="LFO" textpos="36"/>
  <GROUPCOMPONENT name="new group" id="9fbad3fd8ef5dcae" memberName="groupComponent6"
                  virtualName="" explicitFocusOrder="0" pos="70.125% 19.778% 22.75% 13.222%"
                  outlinecol="ffcbdc3c" title="Filter" textpos="36"/>
  <GROUPCOMPONENT name="Voice 1" id="165131b2961da7d8" memberName="groupComponent"
                  virtualName="" explicitFocusOrder="1" pos="2.5% 6.556% 29.688% 10.333%"
                  outlinecol="ff36677a" title="Voice 1" textpos="36"/>
  <SLIDER name="Voice 1 Level" id="b43cd1995731af0f" memberName="voice1lvl"
          virtualName="" explicitFocusOrder="0" pos="9.5% 7.556% 6.25% 8.444%"
          min="0.0" max="10.0" int="0.0" style="RotaryHorizontalVerticalDrag"
          textBoxPos="NoTextBox" textBoxEditable="1" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Voice 1 Freq" id="7ea199d3da532c2d" memberName="voice1freq"
          virtualName="" explicitFocusOrder="0" pos="18.625% 7.556% 6.25% 8.444%"
          min="0.0" max="10.0" int="0.0" style="RotaryHorizontalVerticalDrag"
          textBoxPos="NoTextBox" textBoxEditable="1" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <GROUPCOMPONENT name="Voice 1" id="6bb9e200f5b243e" memberName="groupComponent2"
                  virtualName="" explicitFocusOrder="1" pos="2.5% 18.889% 29.688% 10.333%"
                  outlinecol="ff7e643b" title="Voice 2" textpos="36"/>
  <SLIDER name="Voice 2 Level" id="db2733910a6c1ecd" memberName="voice2lvl"
          virtualName="" explicitFocusOrder="0" pos="9.5% 20.111% 6.25% 8.444%"
          thumbcol="ffc87c42" min="0.0" max="10.0" int="0.0" style="RotaryHorizontalVerticalDrag"
          textBoxPos="NoTextBox" textBoxEditable="1" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Voice 2 Freq" id="82fe31543dbb3b1c" memberName="voice2freq"
          virtualName="" explicitFocusOrder="0" pos="18.625% 20.111% 6.25% 8.444%"
          thumbcol="ffc87c42" min="0.0" max="10.0" int="0.0" style="RotaryHorizontalVerticalDrag"
          textBoxPos="NoTextBox" textBoxEditable="1" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <GROUPCOMPONENT name="Voice 3" id="22f0d4335be64fbf" memberName="groupComponent3"
                  virtualName="" explicitFocusOrder="1" pos="2.5% 31.556% 29.688% 10.333%"
                  outlinecol="ff346d52" title="Voice 3" textpos="36"/>
  <SLIDER name="Voice 3 level" id="93c43e5c541dd5a8" memberName="voice3lvl"
          virtualName="" explicitFocusOrder="0" pos="9.5% 32.889% 6.25% 8.444%"
          thumbcol="ff02c86a" min="0.0" max="10.0" int="0.0" style="RotaryHorizontalVerticalDrag"
          textBoxPos="NoTextBox" textBoxEditable="1" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Voice 3 Freq" id="fa1ae77aef0dbfc7" memberName="voice3freq"
          virtualName="" explicitFocusOrder="0" pos="18.625% 32.889% 6.25% 8.444%"
          thumbcol="ff02c86a" min="0.0" max="10.0" int="0.0" style="RotaryHorizontalVerticalDrag"
          textBoxPos="NoTextBox" textBoxEditable="1" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <GROUPCOMPONENT name="Voice 4" id="93f11183d4431572" memberName="groupComponent4"
                  virtualName="" explicitFocusOrder="1" pos="2.5% 44.556% 29.688% 10.333%"
                  outlinecol="ff6f3966" title="Voice 4" textpos="36"/>
  <SLIDER name="Voice 4 level" id="48cdb98f9cedf299" memberName="voice4lvl"
          virtualName="" explicitFocusOrder="0" pos="9.5% 416 79 72" thumbcol="ffc453b1"
          min="0.5" max="10.0" int="0.5" style="RotaryHorizontalVerticalDrag"
          textBoxPos="NoTextBox" textBoxEditable="1" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Voice 4 Freq" id="f41d97ea893bf662" memberName="voice4freq"
          virtualName="" explicitFocusOrder="0" pos="18.625% 45.889% 6.25% 8.444%"
          thumbcol="ffc453b1" min="10.0" max="1000.0" int="0.0" style="RotaryHorizontalVerticalDrag"
          textBoxPos="NoTextBox" textBoxEditable="1" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Attack" id="44e5957e33056fba" memberName="atk" virtualName=""
          explicitFocusOrder="0" pos="36% 54.222% 6.25% 8.444%" thumbcol="ffcbdc3b"
          min="0.0" max="10.0" int="0.0" style="LinearVertical" textBoxPos="NoTextBox"
          textBoxEditable="0" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="Decay" id="b56b714defb3198b" memberName="dec" virtualName=""
          explicitFocusOrder="0" pos="44.25% 53.778% 6.25% 8.444%" thumbcol="ffcbdc3b"
          min="0.0" max="10.0" int="0.0" style="LinearVertical" textBoxPos="NoTextBox"
          textBoxEditable="0" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="Sustain" id="e1268ef43853b5e1" memberName="sus" virtualName=""
          explicitFocusOrder="0" pos="51.812% 53.778% 6.25% 8.444%" thumbcol="ffcbdc3b"
          min="0.0" max="10.0" int="0.0" style="LinearVertical" textBoxPos="NoTextBox"
          textBoxEditable="0" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="Release" id="1bbe1a247ea5cec7" memberName="rel" virtualName=""
          explicitFocusOrder="0" pos="59.375% 53.778% 6.25% 8.444%" thumbcol="ffcbdc3b"
          min="0.0" max="10.0" int="0.0" style="LinearVertical" textBoxPos="NoTextBox"
          textBoxEditable="0" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="LFO Rate" id="6104695076024b96" memberName="lfoRate" virtualName=""
          explicitFocusOrder="0" pos="78.375% 7.556% 6.25% 8.444%" thumbcol="ffcbdc3b"
          min="0.0" max="10.0" int="0.0" style="RotaryHorizontalVerticalDrag"
          textBoxPos="NoTextBox" textBoxEditable="0" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="LFO Amount" id="cfacf17a3db69272" memberName="lfoAmt" virtualName=""
          explicitFocusOrder="0" pos="85.938% 7.556% 6.25% 8.444%" thumbcol="ffcbdc3b"
          min="0.0" max="10.0" int="0.0" style="RotaryHorizontalVerticalDrag"
          textBoxPos="NoTextBox" textBoxEditable="0" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="LFO Waveform Type" id="fe7cd6f0df117631" memberName="lfoShape"
          virtualName="" explicitFocusOrder="0" pos="70.75% 7.556% 6.25% 8.444%"
          thumbcol="ffcbdc3b" min="0.0" max="10.0" int="0.0" style="RotaryHorizontalVerticalDrag"
          textBoxPos="NoTextBox" textBoxEditable="0" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Filter Type" id="42b4fc77a271cd6c" memberName="fltrType"
          virtualName="" explicitFocusOrder="0" pos="70.75% 21.111% 6.25% 8.444%"
          thumbcol="ffcbdc3b" min="0.0" max="10.0" int="0.0" style="RotaryHorizontalVerticalDrag"
          textBoxPos="NoTextBox" textBoxEditable="0" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Filter Frequency" id="cb4bec1c570a1966" memberName="fltrFreq"
          virtualName="" explicitFocusOrder="0" pos="78.375% 21.111% 6.25% 8.444%"
          thumbcol="ffcbdc3b" min="0.0" max="10.0" int="0.0" style="RotaryHorizontalVerticalDrag"
          textBoxPos="NoTextBox" textBoxEditable="0" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Filter Peak" id="d15d1531566afd48" memberName="fltrPeak"
          virtualName="" explicitFocusOrder="0" pos="85.938% 21.111% 6.25% 8.444%"
          thumbcol="ffcbdc3b" min="0.0" max="10.0" int="0.0" style="RotaryHorizontalVerticalDrag"
          textBoxPos="NoTextBox" textBoxEditable="0" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="FM Type Selection" id="2def14842c0bc4ed" memberName="fmSelect"
          virtualName="" explicitFocusOrder="0" pos="70.75% 32.889% 6.25% 8.444%"
          thumbcol="ffcbdc3b" min="0.0" max="10.0" int="0.0" style="RotaryHorizontalVerticalDrag"
          textBoxPos="NoTextBox" textBoxEditable="0" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Master Outut Level" id="7ddaee2363f872d9" memberName="masterLvl"
          virtualName="" explicitFocusOrder="0" pos="71.5% 42.667% 4.938% 8%"
          thumbcol="ffcbdc3b" min="0.0" max="10.0" int="0.0" style="RotaryHorizontalVerticalDrag"
          textBoxPos="NoTextBox" textBoxEditable="0" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <TEXTBUTTON name="new button" id="201043ba634662c0" memberName="textButton"
              virtualName="" explicitFocusOrder="0" pos="696 48 150 24" bgColOff="a45c94"
              buttonText="" connectedEdges="0" needsCallback="1" radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

