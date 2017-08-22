all: rtaudio_build/librtaudio.so rtmidi_build/.libs/librtmidi.so
	c++ -Ofast -I./rtaudio -I./rtmidi -L. -l:rtaudio_build/librtaudio.so -l:rtmidi_build/.libs/librtmidi.so main.cpp

rtaudio_build/librtaudio.so:
	mkdir rtaudio_build 
	cd rtaudio_build &&	cmake ../rtaudio -DAUDIO_LINUX_PULSE=ON
	cd rtaudio_build &&	cmake --build .

rtmidi_build/.libs/librtmidi.so:
	cp -r rtmidi rtmidi_build
	cd rtmidi_build && autoreconf --install
	cd rtmidi_build && autoconf
	cd rtmidi_build && ./configure
	cd rtmidi_build && make
	