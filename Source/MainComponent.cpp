/*
  ==============================================================================

	This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{


	volume.setSliderStyle(Slider::SliderStyle::Rotary);
	volume.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 20);
	volume.setRange(0, 0.2);
	vol_label.setText("Volume", dontSendNotification);
	vol_label.attachToComponent(&volume, true);

	frequency.setRange(50, 5000.0);
	frequency.setSkewFactorFromMidPoint(500.0);
	frequency.setTextValueSuffix(" Hz");
	frequency.onValueChange = [this] { volume.setValue(1.0 / frequency.getValue(), dontSendNotification); };
	freq_label.setText("Frequency", dontSendNotification);
	freq_label.attachToComponent(&frequency, true);

	addAndMakeVisible(vol_label);
	addAndMakeVisible(freq_label);
	addAndMakeVisible(volume);
	addAndMakeVisible(frequency);

	// Make sure you set the size of the component after
	// you add any child components.
	setSize(800, 600);

	// Some platforms require permissions to open input channels so request that here
	if (RuntimePermissions::isRequired(RuntimePermissions::recordAudio)
		&& !RuntimePermissions::isGranted(RuntimePermissions::recordAudio))
	{
		RuntimePermissions::request(RuntimePermissions::recordAudio,
			[&](bool granted) { if (granted)  setAudioChannels(2, 2); });
	}
	else
	{
		// Specify the number of input and output channels that we want to open
		setAudioChannels(0, 2);
	}
}

MainComponent::~MainComponent()
{
	// This shuts down the audio device and clears the audio source.
	shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
	// This function will be called when the audio device is started, or when
	// its settings (i.e. sample rate, block size, etc) are changed.

	// You can use this function to initialise any resources you might need,
	// but be careful - it will be called on the audio thread, not the GUI thread.

	// For more details, see the help for AudioProcessor::prepareToPlay()
	String message;
	message << "Preparing to play audio..." << newLine;
	message << " samplesPerBlockExpected = " << samplesPerBlockExpected << newLine;
	message << " sampleRate = " << sampleRate;
	Logger::getCurrentLogger()->writeToLog(message);

	frequency.onValueChange = [this]
	{
		if (currentSampleRate > 0.0)
			updateAngleDelta();
	};

	currentSampleRate = sampleRate;
	updateAngleDelta();
}

void MainComponent::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
	// Your audio-processing code goes here!

	// For more details, see the help for AudioProcessor::getNextAudioBlock()

	// Right now we are not producing any data, in which case we need to clear the buffer
	// (to prevent the output of random noise)
	auto level = (float)volume.getValue();

	auto* leftBuffer = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
	auto* rightBuffer = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);

	for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
	{
		auto currentSample = (float)std::sin(currentAngle);
		currentAngle += angleDelta;
		leftBuffer[sample] = currentSample * level;
		rightBuffer[sample] = currentSample * level;
	}
}

void MainComponent::releaseResources()
{
	// This will be called when the audio device stops, or when it is being
	// restarted due to a setting change.

	// For more details, see the help for AudioProcessor::releaseResources()
	Logger::getCurrentLogger()->writeToLog("Releasing audio resources");

}

//==============================================================================
void MainComponent::paint(Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

	// You can add your drawing code here!
}

void MainComponent::resized()
{
	// This is called when the MainContentComponent is resized.
	// If you add any child components, this is where you should
	// update their positions.

	Rectangle<int> area = getLocalBounds();
	Rectangle<int> dialArea = area.removeFromTop(area.getHeight() / 2);
	volume.setBounds(dialArea.removeFromLeft(area.getWidth() / 4));
	frequency.setBounds(dialArea.removeFromRight(area.getWidth() / 4));

}
void MainComponent::updateAngleDelta() {

	auto cyclesPerSample = frequency.getValue() / currentSampleRate;
	angleDelta = cyclesPerSample * 2.0 * MathConstants<double>::pi;

}
