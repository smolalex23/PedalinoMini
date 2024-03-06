<<<<<<< HEAD
/*
__________           .___      .__  .__                 _____  .__       .__     ___ ________________    ___
\______   \ ____   __| _/____  |  | |__| ____   ____   /     \ |__| ____ |__|   /  / \__    ___/     \   \  \
 |     ___// __ \ / __ |\__  \ |  | |  |/    \ /  _ \ /  \ /  \|  |/    \|  |  /  /    |    | /  \ /  \   \  \
 |    |   \  ___// /_/ | / __ \|  |_|  |   |  (  <_> )    Y    \  |   |  \  | (  (     |    |/    Y    \   )  )
 |____|    \___  >____ |(____  /____/__|___|  /\____/\____|__  /__|___|  /__|  \  \    |____|\____|__  /  /  /
               \/     \/     \/             \/               \/        \/       \__\                 \/  /__/
                                                                                   (c) 2018-2023 alf45star
                                                                       https://github.com/alf45tar/PedalinoMini
 */

#ifdef ARDUINO_BPI_LEAF_S33

//#include "USBDeviceMIDI.h"

//USBDEVICEMIDI_CREATE_INSTANCE(USB_DEVICE_MIDI, "PedalinoMini", 1);

//#include <MIDIUSB.h>

//TinyUSBMIDI_CREATE_DEFAULT_INSTANCE();

#include <Adafruit_TinyUSB.h>

#include <MIDI.h>

// USB MIDI object
Adafruit_USBD_MIDI usb_midi;

// Create a new instance of the Arduino MIDI Library,
// and attach usb_midi as the transport.
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, USB_DEVICE_MIDI);



void OnUsbDeviceMidiNoteOn(byte channel, byte note, byte velocity)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendNoteOn(note, velocity, channel);
  BLESendNoteOn(note, velocity, channel);
  ipMIDISendNoteOn(note, velocity, channel);
  AppleMidiSendNoteOn(note, velocity, channel);
  OSCSendNoteOn(note, velocity, channel);
  leds_update(midi::NoteOn, channel, note, velocity);
  if (IS_SHOW_ENABLED(interfaces[PED_USBMIDI].midiIn)) screen_info(midi::NoteOn, note, velocity, channel);
}

void OnUsbDeviceMidiNoteOff(byte channel, byte note, byte velocity)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendNoteOff(note, velocity, channel);
  BLESendNoteOff(note, velocity, channel);
  ipMIDISendNoteOff(note, velocity, channel);
  AppleMidiSendNoteOff(note, velocity, channel);
  OSCSendNoteOff(note, velocity, channel);
  leds_update(midi::NoteOff, channel, note, velocity);
  if (IS_SHOW_ENABLED(interfaces[PED_USBMIDI].midiIn)) screen_info(midi::NoteOff, note, velocity, channel);
}

void OnUsbDeviceMidiAfterTouchPoly(byte channel, byte note, byte pressure)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendAfterTouch(note, pressure, channel);
  BLESendAfterTouchPoly(note, pressure, channel);
  ipMIDISendAfterTouchPoly(note, pressure, channel);
  AppleMidiSendAfterTouchPoly(note, pressure, channel);
  OSCSendAfterTouchPoly(note, pressure, channel);
  if (IS_SHOW_ENABLED(interfaces[PED_USBMIDI].midiIn)) screen_info(midi::AfterTouchPoly, note, pressure, channel);
}

void OnUsbDeviceMidiControlChange(byte channel, byte number, byte value)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendControlChange(number, value, channel);
  BLESendControlChange(number, value, channel);
  ipMIDISendControlChange(number, value, channel);
  AppleMidiSendControlChange(number, value, channel);
  OSCSendControlChange(number, value, channel);
  leds_update(midi::ControlChange, channel, number, value);
  if (IS_SHOW_ENABLED(interfaces[PED_USBMIDI].midiIn)) screen_info(midi::ControlChange, number, value, channel);
  switch_profile_or_bank(channel, number, value);
}

void OnUsbDeviceMidiProgramChange(byte channel, byte number)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendProgramChange(number, channel);
  BLESendProgramChange(number, channel);
  ipMIDISendProgramChange(number, channel);
  AppleMidiSendProgramChange(number, channel);
  OSCSendProgramChange(number, channel);
  leds_update(midi::ProgramChange, channel, number, 0);
  if (IS_SHOW_ENABLED(interfaces[PED_USBMIDI].midiIn)) screen_info(midi::ProgramChange, number, 0, channel);
}

void OnUsbDeviceMidiAfterTouchChannel(byte channel, byte pressure)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendAfterTouch(pressure, channel);
  BLESendAfterTouch(pressure, channel);
  ipMIDISendAfterTouch(pressure, channel);
  AppleMidiSendAfterTouch(pressure, channel);
  OSCSendAfterTouch(pressure, channel);
  if (IS_SHOW_ENABLED(interfaces[PED_USBMIDI].midiIn)) screen_info(midi::AfterTouchChannel, pressure, 0, channel);
}

void OnUsbDeviceMidiPitchBend(byte channel, int bend)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendPitchBend(bend, channel);
  BLESendPitchBend(bend, channel);
  ipMIDISendPitchBend(bend, channel);
  AppleMidiSendPitchBend(bend, channel);
  OSCSendPitchBend(bend, channel);
  if (IS_SHOW_ENABLED(interfaces[PED_USBMIDI].midiIn)) screen_info(midi::PitchBend, bend, 0, channel);
}

void OnUsbDeviceMidiSystemExclusive(byte* array, unsigned size)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendSysEx(size, array);
  BLESendSystemExclusive(array, size);
  ipMIDISendSystemExclusive(array, size);
  AppleMidiSendSystemExclusive(array, size);
  OSCSendSystemExclusive(array, size);
}

void OnUsbDeviceMidiTimeCodeQuarterFrame(byte data)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendTimeCodeQuarterFrame(data);
  BLESendTimeCodeQuarterFrame(data);
  ipMIDISendTimeCodeQuarterFrame(data);
  AppleMidiSendTimeCodeQuarterFrame(data);
  OSCSendTimeCodeQuarterFrame(data);
}

void OnUsbDeviceMidiSongPosition(unsigned int beats)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendSongPosition(beats);
  BLESendSongPosition(beats);
  ipMIDISendSongPosition(beats);
  AppleMidiSendSongPosition(beats);
  OSCSendSongPosition(beats);
}

void OnUsbDeviceMidiSongSelect(byte songnumber)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendSongSelect(songnumber);
  BLESendSongSelect(songnumber);
  ipMIDISendSongSelect(songnumber);
  AppleMidiSendSongSelect(songnumber);
  OSCSendSongSelect(songnumber);
}

void OnUsbDeviceMidiTuneRequest(void)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendTuneRequest();
  BLESendTuneRequest();
  ipMIDISendTuneRequest();
  AppleMidiSendTuneRequest();
  OSCSendTuneRequest();
}

void OnUsbDeviceMidiClock(void)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::Clock);
  BLESendClock();
  ipMIDISendClock();
  AppleMidiSendClock();
  OSCSendClock();
}

void OnUsbDeviceMidiStart(void)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::Start);
  BLESendStart();
  ipMIDISendStart();
  AppleMidiSendStart();
  OSCSendStart();
}

void OnUsbDeviceMidiContinue(void)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::Continue);
  BLESendContinue();
  ipMIDISendContinue();
  AppleMidiSendContinue();
  OSCSendContinue();
}

void OnUsbDeviceMidiStop(void)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::Stop);
  BLESendStop();
  ipMIDISendStop();
  AppleMidiSendStop();
  OSCSendStop();
}

void OnUsbDeviceMidiActiveSensing(void)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::ActiveSensing);
  BLESendActiveSensing();
  ipMIDISendActiveSensing();
  AppleMidiSendActiveSensing();
  OSCSendActiveSensing();
}

void OnUsbDeviceMidiSystemReset(void)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::SystemReset);
  BLESendSystemReset();
  ipMIDISendSystemReset();
  AppleMidiSendSystemReset();
  OSCSendSystemReset();
}

void usb_device_midi_connect()
{
  // Connect the handle function called upon reception of a MIDI message from USB MIDI interface
  USB_DEVICE_MIDI.setHandleNoteOn(OnUsbDeviceMidiNoteOn);
  USB_DEVICE_MIDI.setHandleNoteOff(OnUsbDeviceMidiNoteOff);
  USB_DEVICE_MIDI.setHandleAfterTouchPoly(OnUsbDeviceMidiAfterTouchPoly);
  USB_DEVICE_MIDI.setHandleControlChange(OnUsbDeviceMidiControlChange);
  USB_DEVICE_MIDI.setHandleProgramChange(OnUsbDeviceMidiProgramChange);
  USB_DEVICE_MIDI.setHandleAfterTouchChannel(OnUsbDeviceMidiAfterTouchChannel);
  USB_DEVICE_MIDI.setHandlePitchBend(OnUsbDeviceMidiPitchBend);
  USB_DEVICE_MIDI.setHandleSystemExclusive(OnUsbDeviceMidiSystemExclusive);
  USB_DEVICE_MIDI.setHandleTimeCodeQuarterFrame(OnUsbDeviceMidiTimeCodeQuarterFrame);
  USB_DEVICE_MIDI.setHandleSongPosition(OnUsbDeviceMidiSongPosition);
  USB_DEVICE_MIDI.setHandleSongSelect(OnUsbDeviceMidiSongSelect);
  USB_DEVICE_MIDI.setHandleTuneRequest(OnUsbDeviceMidiTuneRequest);
  USB_DEVICE_MIDI.setHandleClock(OnUsbDeviceMidiClock);
  USB_DEVICE_MIDI.setHandleStart(OnUsbDeviceMidiStart);
  USB_DEVICE_MIDI.setHandleContinue(OnUsbDeviceMidiContinue);
  USB_DEVICE_MIDI.setHandleStop(OnUsbDeviceMidiStop);
  USB_DEVICE_MIDI.setHandleActiveSensing(OnUsbDeviceMidiActiveSensing);
  USB_DEVICE_MIDI.setHandleSystemReset(OnUsbDeviceMidiSystemReset);

  // Initiate USB MIDI communications, listen to all channels
  USB_DEVICE_MIDI.begin(MIDI_CHANNEL_OMNI);
  // Enable/disable MIDI Thru
  interfaces[PED_USBDEVICEMIDI].midiThru ? USB_DEVICE_MIDI.turnThruOn() : USB_DEVICE_MIDI.turnThruOff();
}

#else

void usb_device_midi_connect();

#endif
=======
/*
__________           .___      .__  .__                 _____  .__       .__     ___ ________________    ___
\______   \ ____   __| _/____  |  | |__| ____   ____   /     \ |__| ____ |__|   /  / \__    ___/     \   \  \
 |     ___// __ \ / __ |\__  \ |  | |  |/    \ /  _ \ /  \ /  \|  |/    \|  |  /  /    |    | /  \ /  \   \  \
 |    |   \  ___// /_/ | / __ \|  |_|  |   |  (  <_> )    Y    \  |   |  \  | (  (     |    |/    Y    \   )  )
 |____|    \___  >____ |(____  /____/__|___|  /\____/\____|__  /__|___|  /__|  \  \    |____|\____|__  /  /  /
               \/     \/     \/             \/               \/        \/       \__\                 \/  /__/
                                                                                   (c) 2018-2024 alf45star
                                                                       https://github.com/alf45tar/PedalinoMini
 */


void OnUSBMidiNoteOn(byte channel, byte note, byte velocity)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendNoteOn(note, velocity, channel);
  BLESendNoteOn(note, velocity, channel);
  ipMIDISendNoteOn(note, velocity, channel);
  AppleMidiSendNoteOn(note, velocity, channel);
  OSCSendNoteOn(note, velocity, channel);
  leds_update(midi::NoteOn, channel, note, velocity);
  if (IS_SHOW_ENABLED(interfaces[PED_USBMIDI].midiIn)) screen_info(midi::NoteOn, note, velocity, channel);
}

void OnUSBMidiNoteOff(byte channel, byte note, byte velocity)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendNoteOff(note, velocity, channel);
  BLESendNoteOff(note, velocity, channel);
  ipMIDISendNoteOff(note, velocity, channel);
  AppleMidiSendNoteOff(note, velocity, channel);
  OSCSendNoteOff(note, velocity, channel);
  leds_update(midi::NoteOff, channel, note, velocity);
  if (IS_SHOW_ENABLED(interfaces[PED_USBMIDI].midiIn)) screen_info(midi::NoteOff, note, velocity, channel);
}

void OnUSBMidiAfterTouchPoly(byte channel, byte note, byte pressure)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendAfterTouch(note, pressure, channel);
  BLESendAfterTouchPoly(note, pressure, channel);
  ipMIDISendAfterTouchPoly(note, pressure, channel);
  AppleMidiSendAfterTouchPoly(note, pressure, channel);
  OSCSendAfterTouchPoly(note, pressure, channel);
  if (IS_SHOW_ENABLED(interfaces[PED_USBMIDI].midiIn)) screen_info(midi::AfterTouchPoly, note, pressure, channel);
}

void OnUSBMidiControlChange(byte channel, byte number, byte value)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendControlChange(number, value, channel);
  BLESendControlChange(number, value, channel);
  ipMIDISendControlChange(number, value, channel);
  AppleMidiSendControlChange(number, value, channel);
  OSCSendControlChange(number, value, channel);
  leds_update(midi::ControlChange, channel, number, value);
  if (IS_SHOW_ENABLED(interfaces[PED_USBMIDI].midiIn)) screen_info(midi::ControlChange, number, value, channel);
  switch_profile_or_bank(channel, number, value);
}

void OnUSBMidiProgramChange(byte channel, byte number)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendProgramChange(number, channel);
  BLESendProgramChange(number, channel);
  ipMIDISendProgramChange(number, channel);
  AppleMidiSendProgramChange(number, channel);
  OSCSendProgramChange(number, channel);
  leds_update(midi::ProgramChange, channel, number, 0);
  if (IS_SHOW_ENABLED(interfaces[PED_USBMIDI].midiIn)) screen_info(midi::ProgramChange, number, 0, channel);
}

void OnUSBMidiAfterTouchChannel(byte channel, byte pressure)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendAfterTouch(pressure, channel);
  BLESendAfterTouch(pressure, channel);
  ipMIDISendAfterTouch(pressure, channel);
  AppleMidiSendAfterTouch(pressure, channel);
  OSCSendAfterTouch(pressure, channel);
  if (IS_SHOW_ENABLED(interfaces[PED_USBMIDI].midiIn)) screen_info(midi::AfterTouchChannel, pressure, 0, channel);
}

void OnUSBMidiPitchBend(byte channel, int bend)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendPitchBend(bend, channel);
  BLESendPitchBend(bend, channel);
  ipMIDISendPitchBend(bend, channel);
  AppleMidiSendPitchBend(bend, channel);
  OSCSendPitchBend(bend, channel);
  if (IS_SHOW_ENABLED(interfaces[PED_USBMIDI].midiIn)) screen_info(midi::PitchBend, bend, 0, channel);
}

void OnUSBMidiSystemExclusive(byte* array, unsigned size)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendSysEx(size, array);
  BLESendSystemExclusive(array, size);
  ipMIDISendSystemExclusive(array, size);
  AppleMidiSendSystemExclusive(array, size);
  OSCSendSystemExclusive(array, size);
  MTC.decodeMTCFullFrame(size, array);
}

void OnUSBMidiTimeCodeQuarterFrame(byte data)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendTimeCodeQuarterFrame(data);
  BLESendTimeCodeQuarterFrame(data);
  ipMIDISendTimeCodeQuarterFrame(data);
  AppleMidiSendTimeCodeQuarterFrame(data);
  OSCSendTimeCodeQuarterFrame(data);
  MTC.decodMTCQuarterFrame(data);
}

void OnUSBMidiSongPosition(unsigned int beats)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendSongPosition(beats);
  BLESendSongPosition(beats);
  ipMIDISendSongPosition(beats);
  AppleMidiSendSongPosition(beats);
  OSCSendSongPosition(beats);
}

void OnUSBMidiSongSelect(byte songnumber)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendSongSelect(songnumber);
  BLESendSongSelect(songnumber);
  ipMIDISendSongSelect(songnumber);
  AppleMidiSendSongSelect(songnumber);
  OSCSendSongSelect(songnumber);
}

void OnUSBMidiTuneRequest(void)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendTuneRequest();
  BLESendTuneRequest();
  ipMIDISendTuneRequest();
  AppleMidiSendTuneRequest();
  OSCSendTuneRequest();
}

void OnUSBMidiClock(void)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::Clock);
  BLESendClock();
  ipMIDISendClock();
  AppleMidiSendClock();
  OSCSendClock();
  if (MTC.getMode() == MidiTimeCode::SynchroClockSlave) bpm = MTC.tapTempo();
}

void OnUSBMidiStart(void)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::Start);
  BLESendStart();
  ipMIDISendStart();
  AppleMidiSendStart();
  OSCSendStart();
  if (MTC.getMode() == MidiTimeCode::SynchroClockSlave) MTC.sendPlay();
}

void OnUSBMidiContinue(void)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::Continue);
  BLESendContinue();
  ipMIDISendContinue();
  AppleMidiSendContinue();
  OSCSendContinue();
  if (MTC.getMode() == MidiTimeCode::SynchroClockSlave) MTC.sendContinue();
}

void OnUSBMidiStop(void)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::Stop);
  BLESendStop();
  ipMIDISendStop();
  AppleMidiSendStop();
  OSCSendStop();
  if (MTC.getMode() == MidiTimeCode::SynchroClockSlave) MTC.sendStop();
}

void OnUSBMidiActiveSensing(void)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::ActiveSensing);
  BLESendActiveSensing();
  ipMIDISendActiveSensing();
  AppleMidiSendActiveSensing();
  OSCSendActiveSensing();
}

void OnUSBMidiSystemReset(void)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::SystemReset);
  BLESendSystemReset();
  ipMIDISendSystemReset();
  AppleMidiSendSystemReset();
  OSCSendSystemReset();
}


void usb_midi_connect()
{
  // Connect the handle function called upon reception of a MIDI message from USB MIDI interface
  USB_MIDI.setHandleNoteOn(OnUSBMidiNoteOn);
  USB_MIDI.setHandleNoteOff(OnUSBMidiNoteOff);
  USB_MIDI.setHandleAfterTouchPoly(OnUSBMidiAfterTouchPoly);
  USB_MIDI.setHandleControlChange(OnUSBMidiControlChange);
  USB_MIDI.setHandleProgramChange(OnUSBMidiProgramChange);
  USB_MIDI.setHandleAfterTouchChannel(OnUSBMidiAfterTouchChannel);
  USB_MIDI.setHandlePitchBend(OnUSBMidiPitchBend);
  USB_MIDI.setHandleSystemExclusive(OnUSBMidiSystemExclusive);
  USB_MIDI.setHandleTimeCodeQuarterFrame(OnUSBMidiTimeCodeQuarterFrame);
  USB_MIDI.setHandleSongPosition(OnUSBMidiSongPosition);
  USB_MIDI.setHandleSongSelect(OnUSBMidiSongSelect);
  USB_MIDI.setHandleTuneRequest(OnUSBMidiTuneRequest);
  USB_MIDI.setHandleClock(OnUSBMidiClock);
  USB_MIDI.setHandleStart(OnUSBMidiStart);
  USB_MIDI.setHandleContinue(OnUSBMidiContinue);
  USB_MIDI.setHandleStop(OnUSBMidiStop);
  USB_MIDI.setHandleActiveSensing(OnUSBMidiActiveSensing);
  USB_MIDI.setHandleSystemReset(OnUSBMidiSystemReset);

#if defined(ARDUINO_BPI_LEAF_S3) || defined(ARDUINO_LILYGO_T_DISPLAY_S3)
  //usb_midi.setStringDescriptor("PedalinoMini USB MIDI");
  TinyUSBDevice.setManufacturerDescriptor("Pedalino");
  TinyUSBDevice.setProductDescriptor("PedalinoMini USB MIDI");
  usb_midi.setCableName(1, "PedalinoMini");
#endif

  // Initiate USB MIDI communications, listen to all channels
  USB_MIDI.begin(MIDI_CHANNEL_OMNI);
  // Enable/disable MIDI Thru
  interfaces[PED_USBMIDI].midiThru ? USB_MIDI.turnThruOn() : USB_MIDI.turnThruOff();

#if defined(ARDUINO_BPI_LEAF_S3) || defined(ARDUINO_LILYGO_T_DISPLAY_S3)
  // Wait until device mounted
  // while(!TinyUSBDevice.mounted()) delay(1);
#endif
}
>>>>>>> e4f1e97206ff5a34f78e3a8f04bd44979efc91f0
