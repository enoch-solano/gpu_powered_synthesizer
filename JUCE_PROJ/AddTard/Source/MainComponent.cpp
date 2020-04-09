/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"
#include "Gui.h"


//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    // guiWin.reset(new TopLevelWindow("Gui Window", true));
    // guiWin->addAndMakeVisible(new Gui());
    // guiWin->addToDesktop();
    // guiWin->setSize(1600, 900);
    // addAndMakeVisible(guiWin.get());

  
    Path path;
    Rectangle<float> area(47, 48); 
    path.addRectangle(area);

    trigger.reset(new DrawableButton("Trigger button", DrawableButton::ImageRaw));
    addAndMakeVisible(trigger.get());
    DrawablePath on, off;
    on.setPath(path);
    on.setFill(Colours::grey);
    off.setPath(path);
    off.setFill(Colours::red);
    trigger->setImages(&on, &on, &off);
    trigger->addListener(this);

    setOpaque(true);

    // Some platforms require permissions to open input channels so request that here
    if (RuntimePermissions::isRequired (RuntimePermissions::recordAudio)
        && ! RuntimePermissions::isGranted (RuntimePermissions::recordAudio))
    {
        RuntimePermissions::request (RuntimePermissions::recordAudio,
                                     [&] (bool granted) { if (granted)  setAudioChannels (2, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }
     audioSetupComp.reset (new AudioDeviceSelectorComponent (audioDeviceManager,
                                                                0, 256, 0, 256, true, true, true, false));
        addAndMakeVisible (audioSetupComp.get());

        addAndMakeVisible (diagnosticsBox);
        diagnosticsBox.setMultiLine (true);
        diagnosticsBox.setReturnKeyStartsNewLine (true);
        diagnosticsBox.setReadOnly (true);
        diagnosticsBox.setScrollbarsShown (true);
        diagnosticsBox.setCaretVisible (false);
        diagnosticsBox.setPopupMenuEnabled (true);

        audioDeviceManager.addChangeListener (this);

        logMessage ("Audio device diagnostics:\n");
        dumpDeviceInfo();

        addAndMakeVisible( new Gui());
        setSize (500, 600);
}



MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
    audioDeviceManager.removeChangeListener (this);
    
    guiWin = nullptr;
}
void MainComponent::dumpDeviceInfo()
    {
        logMessage ("--------------------------------------");
        logMessage ("Current audio device type: " + (audioDeviceManager.getCurrentDeviceTypeObject() != nullptr
                                                     ? audioDeviceManager.getCurrentDeviceTypeObject()->getTypeName()
                                                     : "<none>"));

        if (AudioIODevice* device = audioDeviceManager.getCurrentAudioDevice())
        {
            logMessage ("Current audio device: "   + device->getName().quoted());
            logMessage ("Sample rate: "    + String (device->getCurrentSampleRate()) + " Hz");
            logMessage ("Block size: "     + String (device->getCurrentBufferSizeSamples()) + " samples");
            logMessage ("Output Latency: " + String (device->getOutputLatencyInSamples())   + " samples");
            logMessage ("Input Latency: "  + String (device->getInputLatencyInSamples())    + " samples");
            logMessage ("Bit depth: "      + String (device->getCurrentBitDepth()));
            logMessage ("Input channel names: "    + device->getInputChannelNames().joinIntoString (", "));
            logMessage ("Active input channels: "  + getListOfActiveBits (device->getActiveInputChannels()));
            logMessage ("Output channel names: "   + device->getOutputChannelNames().joinIntoString (", "));
            logMessage ("Active output channels: " + getListOfActiveBits (device->getActiveOutputChannels()));
        }
        else
        {
            logMessage ("No audio device open");
        }
    }

    void MainComponent::logMessage (const String& m)
    {
        diagnosticsBox.moveCaretToEnd();
        diagnosticsBox.insertTextAtCaret (m + newLine);
    }
//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
    /* 
    * Change buffer size 
    */ 
    AudioDeviceManager::AudioDeviceSetup currentAudioSetup;
    deviceManager.getAudioDeviceSetup(currentAudioSetup);
    currentAudioSetup.bufferSize = 128;
    deviceManager.setAudioDeviceSetup(currentAudioSetup, true);

    sampleRate = sampleRate;
    std::cout << sampleRate << std::endl;
    expectedSamplesPerBlock = currentAudioSetup.bufferSize;
    std::cout << "init..." <<std::endl;
    synth = Engine::getInstance(expectedSamplesPerBlock);
    std::cout << "initialised!" <<std::endl;
}

void printArray(int n, float *a, bool abridged) {
    printf("    [ ");
    for (int i = 0; i < n; i++) {
        if (abridged && i + 2 == 15 && n > 16) {
            i = n - 2;
            printf("... ");
        }
        printf("%3f, ", a[i]);
    } 
    printf("]\n");
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    
    bufferToFill.clearActiveBufferRegion();
    auto originalPhase = phase;
    //for (auto chan = 0; chan < bufferToFill.buffer->getNumChannels(); ++chan){
        if(bufferToFill.buffer->getNumChannels() == 2){
          //  std::cout << 0 << std::endl;
            auto* channelData = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
            synth->simple_tick(channelData, bufferToFill.numSamples);
            //printArray(bufferToFill.numSamples, (float*)channelData, 1);
             //std::cout << 1<< std::endl;
            auto* chanman = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);
    
            memcpy(chanman, channelData, sizeof(float) * bufferToFill.numSamples);// bufferToFill.buffer->getReadPointer(1, bufferToFill.startSample) );
             //printArray(bufferToFill.numSamples, (float*)chanman, 1);

       // }

        /*phase = originalPhase;
        auto* channelData = bufferToFill.buffer->getWritePointer(chan, bufferToFill.startSample);
        for (auto i = 0; i < bufferToFill.numSamples; ++i){
            channelData[i] = amplitude * std::sin(phase);
            phase = std::fmod(phase + phaseDelta, MathConstants<float>::twoPi);
        }*/

    }
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
inline Colour getUIColourIfAvailable (LookAndFeel_V4::ColourScheme::UIColour uiColour, Colour fallback = Colour (0xff4d4d4d)) noexcept
{
    if (auto* v4 = dynamic_cast<LookAndFeel_V4*> (&LookAndFeel::getDefaultLookAndFeel()))
        return v4->getCurrentColourScheme().getUIColour (uiColour);

    return fallback;
}

void MainComponent::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
 g.fillAll (getUIColourIfAvailable (LookAndFeel_V4::ColourScheme::UIColour::windowBackground));
    // You can add your drawing code here!
}



void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    trigger->setBounds(250, 300, 47, 48);
    auto r =  getLocalBounds().reduced (4);
        audioSetupComp->setBounds (r.removeFromTop (proportionOfHeight (0.65f)));
        diagnosticsBox.setBounds (r);
    //guiWin->setBounds()
}

void MainComponent::buttonClicked(Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == trigger.get())
    {
        std::cout<<"the button was clicked"<<std::endl;
         //trigger event here
    }
}

