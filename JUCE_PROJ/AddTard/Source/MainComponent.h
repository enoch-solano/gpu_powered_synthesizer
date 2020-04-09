/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "engine.h"
#include "Gui.h"


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public AudioAppComponent, public ChangeListener, public Button::Listener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
 void logMessage (const String& m);
 void dumpDeviceInfo();
    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;
    void buttonClicked(Button* buttonThatWasClicked) override;


private:
    //==============================================================================
    // Your private member variables go here...
    float phase = 0.0f;
    float phaseDelta = (float) (MathConstants<double>::twoPi * 5000.f / 44100.f);
    float frequency= 5000.f;
    float amplitude = 0.8f;
    std::unique_ptr<DrawableButton> trigger;
    std::unique_ptr<TopLevelWindow> guiWin;

    AudioDeviceManager audioDeviceManager;
    TextEditor diagnosticsBox;
        std::unique_ptr<AudioDeviceSelectorComponent> audioSetupComp;
    Engine* synth = NULL;
    double sampleRate = 0.0;
    int expectedSamplesPerBlock = 0;

     void changeListenerCallback (ChangeBroadcaster*) override
    {
        dumpDeviceInfo();
    }

    void lookAndFeelChanged() override
    {
        diagnosticsBox.applyFontToAllText (diagnosticsBox.getFont());
    }

    static String getListOfActiveBits (const BigInteger& b)
    {
        StringArray bits;

        for (int i = 0; i <= b.getHighestBit(); ++i)
            if (b[i])
                bits.add (String (i));

        return bits.joinIntoString (", ");
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

