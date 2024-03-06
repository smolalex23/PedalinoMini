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

#include <MIDI.h>                       // https://github.com/FortySevenEffects/arduino_midi_library

#include "ESPSerialMIDI.h"

#define MIDI_BAUD_RATE                  31250
#define HIGH_SPEED_SERIAL_BAUD_RATE     1000000

struct Serial2MIDISettings : public midi::DefaultSettings
{
  static const long BaudRate = MIDI_BAUD_RATE;
  static const int8_t RxPin  = DIN_MIDI_IN_PIN;
  static const int8_t TxPin  = DIN_MIDI_OUT_PIN;
};

#define SERIAL_MIDI_DIN   Serial2

MIDI_CREATE_CUSTOM_INSTANCE_ESP(HardwareSerial, SERIAL_MIDI_DIN, DIN_MIDI, Serial2MIDISettings);


void DIN_MIDI_SendRealTimeMessage(byte type)
{
  if (!interfaces[PED_DINMIDI].midiOut) return;

  switch (type) {

      case midi::TuneRequest:
        DIN_MIDI.sendTuneRequest();
        break;

      case midi::Clock:
        DIN_MIDI.sendClock();
        break;

      case midi::Start:
        DIN_MIDI.sendStart();
        break;

      case midi::Continue:
        DIN_MIDI.sendContinue();
        break;

      case midi::Stop:
        DIN_MIDI.sendStop();
        break;

      case midi::ActiveSensing:
        DIN_MIDI.sendActiveSensing();
        break;

      case midi::SystemReset:
        DIN_MIDI.sendSystemReset();
        break;
    }
}