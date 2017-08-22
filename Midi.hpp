#include <RtMidi.h>
#include <stdexcept>
#include <iostream>

#ifndef MIDI_HPP
#define MIDI_HPP


class MidiListener 
{
private:
public:

    virtual void noteOn(unsigned char note, unsigned char velocity) = 0;
    virtual void noteOff(unsigned char note) = 0;
};


void process_midi(double deltatime, std::vector<unsigned char> * message_ptr, void * userData)
{
    if (userData == nullptr)
    {
        return;
    }
    MidiListener& listener = *(MidiListener *) userData;
    
    std::vector<unsigned char>& message = *message_ptr;
    if (message.size() != 3) {
        std::cout << "WARNING: Encountered malformed MIDI message." << std::endl;
        return;
    }

    unsigned char note = message[1];
    unsigned char velocity = message[2];

    if ((message[0] & 0xF0) == 0x80)
    {
        // Note Off
        listener.noteOff(note);
    }

    if ((message[0] & 0xF0) == 0x90)
    {
        // Note On
        listener.noteOn(note, velocity);
    }

}


class MidiIn 
{
private: 

    RtMidiIn midi_in;

    MidiListener * midi_listener = nullptr;
    
public: 

    void connect(unsigned int port_nr)
    {
        midi_in.openPort(port_nr);
        midi_in.ignoreTypes(true, true, true);
    }

    void connect_interactive()
    {
        unsigned int portc = midi_in.getPortCount();
        if (portc == 0)
        {
            throw std::runtime_error("No midi devices found.");
        }
        else if (portc == 1)
        {
            connect(0);
        }
        else 
        {
            std::cout << "Please select a midi device: " << std::endl;
            for (int i=0; i < portc; ++i)
            {
                std::cout << " (" << i << ") " << midi_in.getPortName(i) << std::endl;
            }
            int port=-1;
            while(port < 0 || port >= portc) 
            {
                std::cin >> port;
                std::cin.get();			
            }
            connect(port);          
        }
    }

    void setMidiListener(MidiListener * listener)
    {
        midi_listener = listener;
        midi_in.setCallback(&process_midi, midi_listener);
    }

    MidiListener * getMidiListener() const 
    {
        return midi_listener;
    }



};

#endif
