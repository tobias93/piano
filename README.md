# What is this?

This is a tiny toy project that I implemented just for fun. It is a simple software synthesizer that is controlled via MIDI.

# Compiling

Make sure that you also downloaded the git submodules. You will also need cmake for rtaudio and rtmidi. Then run `make`. That's it.

# How to use?

Connect a MIDI device (for example, your e-piano) to your computer.

Then, start the program:
```bash
LD_LIBRARY_PATH="./rtaudio_build;./rtmidi_build/.libs" ./a.out
```

If multiple MIDI devices are found, then you are prompted to select the correct one.

Now you can play some music on your device. You should hear the notes that you play from your computer. Have fun!
