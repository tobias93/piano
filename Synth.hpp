#include "Audio.hpp"
#include "Midi.hpp"
#include <cmath>
#include <array>


#ifndef SYNTH_HPP
#define SYNTH_HPP 

struct Envelope 
{
    double amplitude = 1;
    double attack = 0;
    double decay = 0;
    double sustain = 1;
    double release = 0;
};

class SimpleSynth: public AudioSource, public MidiListener
{
private: 

    struct Note 
    {
        bool pressed = false;
        unsigned int last_event_time_code = 0;
        unsigned char note = 42;
        unsigned char velocity = 0;
    };

    unsigned int time_code = 0;
    constexpr static unsigned char max_concurrency = 10;
    std::array<Note, max_concurrency> notes;
    Envelope synth;

    double getEnvelopeVolume(const Note& note, const Envelope& env, unsigned int t)
    {
        unsigned int time_since_last_event = t - note.last_event_time_code;
        if (note.pressed) 
        {
            unsigned int attack_time = env.attack * audio_settings->sampleRate;
            unsigned int decay_time = env.decay * audio_settings->sampleRate;
            
            if (time_since_last_event <= attack_time)
            {
                // attack
                double pos = (double)time_since_last_event / attack_time;
                return pos * env.amplitude;
            }

            time_since_last_event -= attack_time;
            if (time_since_last_event <= decay_time)
            {
                // decay
                double pos = (double)time_since_last_event / decay_time;
                return (1 - pos) * env.amplitude + pos * env.sustain;
            }

            // sustain
            return env.sustain;
            
        }
        else 
        {
            unsigned int release_time = env.release * audio_settings->sampleRate;
            if (time_since_last_event <= release_time) 
            {
                // release
                double pos = (double)time_since_last_event / release_time;
                return (double)(1 - pos) * env.sustain;
            }
            
            // off
            return 0;
            
        }
    }

public: 
    
        int fillBuffer(double * buffer, unsigned int nBufferFrames)
        {
            for (int i=0; i < nBufferFrames; ++i) 
            {
                buffer[i] = 0;

                unsigned int t = time_code + i;

                for (int n=0; n<max_concurrency; ++n)
                {
                    Note& note = notes[n];
                    double volume = (double)note.velocity / 256 * getEnvelopeVolume(note, synth, t);
                    double frequency = pow(2, (double)(note.note - 69) / 12) * 441;
                    double a = volume * sin((double)t / audio_settings->sampleRate * frequency * 2 * M_PI);
                    buffer[i] += a;
                }

            }
    
            time_code += nBufferFrames;
            return 0;
        }

        void noteOn(unsigned char note, unsigned char velocity)
        {
            std::size_t oldest_note = 0;
            for (std::size_t i=1; i<max_concurrency; ++i) 
            {
                if (notes[oldest_note].pressed != notes[i].pressed) 
                {
                    if (!notes[i].pressed) {
                        oldest_note = i;
                    }
                }
                else if(notes[oldest_note].last_event_time_code > notes[i].last_event_time_code) 
                {
                    oldest_note = i;
                }
            }
            
            notes[oldest_note].last_event_time_code = time_code;
            notes[oldest_note].note = note;
            notes[oldest_note].velocity = velocity;
            notes[oldest_note].pressed = true;

        }

        void noteOff(unsigned char note) 
        {
            for (std::size_t i=0; i<max_concurrency; ++i) 
            {
                if (notes[i].note == note && notes[i].pressed)
                {
                    notes[i].last_event_time_code = time_code;
                    notes[i].pressed = false;
                }
            }	
        }

        void setEnvelope(const Envelope& envelope)
        {
            synth = envelope;
        }

        Envelope getEnvelope() const 
        {
            return synth;
        }
};

#endif
