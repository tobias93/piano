#include <RtAudio.h>
#include <stdexcept>
#include <memory>
#include <iostream>

#ifndef AUDIO_HPP
#define AUDIO_HPP


struct AudioSettings 
{
    unsigned int sampleRate = 22050;
    unsigned int bufferFrames = 256;
    unsigned int nrChannels = 1;
};

class AudioSource 
{
private:
    
protected: 

    const AudioSettings * audio_settings = nullptr;

public:

    void injectAudioSettings(const AudioSettings * settings) {
        audio_settings = settings;
    }

    virtual int fillBuffer(double * buffer, unsigned int nBufferFrames) = 0;
};

int playFromSource(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *userData)
{
    double * buffer = (double *) outputBuffer;
	AudioSource& source = *(AudioSource*)userData;

	if (status)
	{
		std::cerr << "Stream underflow detected!" << std::endl;
    }
    
    return source.fillBuffer(buffer, nBufferFrames);
}

class AudioPlayer 
{
private:

    RtAudio dac;
    AudioSettings settins;
    AudioSource * source = nullptr;

public:

    AudioPlayer()
    {
        if ( dac.getDeviceCount() < 1 ) {
            throw std::runtime_error("No audio devices found.");
        }

        RtAudio::StreamParameters parameters;
        parameters.deviceId = dac.getDefaultOutputDevice();
        parameters.nChannels = 1;
        parameters.firstChannel = 0;


    }

    void setAudioSettings(const AudioSettings& audioSettings)
    {
        if (dac.isStreamOpen()) 
        {
            dac.closeStream();
        }
        settins = audioSettings;
    }

    const AudioSettings& getAudioSettings() const 
    {
        return settins;
    }

    void setAudioSource(AudioSource * source)
    {
        if (dac.isStreamOpen())
        {
            dac.closeStream();
        }
        this->source = source;
    }

    AudioSource * getAudioSource() const 
    {
        return source;
    }

    void play()
    {
        if (source == nullptr) {
            throw std::logic_error("There is no source to play from.");
        }
        source->injectAudioSettings(&settins);

        if (!dac.isStreamOpen()) 
        {
            RtAudio::StreamParameters parameters;
            parameters.deviceId = dac.getDefaultOutputDevice();
            parameters.nChannels = settins.nrChannels;
            parameters.firstChannel = 0;
            dac.openStream(&parameters, NULL, RTAUDIO_FLOAT64, settins.sampleRate, &(settins.bufferFrames), &playFromSource, (void *)source );
        }

        dac.startStream();

    }

    void stop() 
    {
        dac.stopStream();
    }

    ~AudioPlayer() 
    {
        if (dac.isStreamOpen())
        {
            dac.closeStream();
        }
    }

};

#endif
