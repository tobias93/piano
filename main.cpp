#include "Audio.hpp"
#include "Midi.hpp"
#include "Synth.hpp"
#include <iostream>

using namespace std;

int main()
{
	MidiIn midi;
	SimpleSynth synth;
	AudioPlayer player;

	AudioSettings audioSettings;
	audioSettings.bufferFrames = 64;
	player.setAudioSettings(audioSettings);

	Envelope envelope;
	envelope.amplitude = 1;
	envelope.sustain = .5;
	envelope.attack = 0.1;
	envelope.decay = 0.6;
	envelope.release = 2;
	synth.setEnvelope(envelope);
	
	player.setAudioSource(&synth);
	midi.setMidiListener(&synth);
	
	midi.connect_interactive();
	player.play();

	cout << "Have fun playing some music!." << endl;
	cout << "Press [enter] to exit." << endl;

	cin.get();

	return 0;
}