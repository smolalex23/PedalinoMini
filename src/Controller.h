/*
__________           .___      .__  .__                 _____  .__       .__     ___ ________________    ___
\______   \ ____   __| _/____  |  | |__| ____   ____   /     \ |__| ____ |__|   /  / \__    ___/     \   \  \
 |     ___// __ \ / __ |\__  \ |  | |  |/    \ /  _ \ /  \ /  \|  |/    \|  |  /  /    |    | /  \ /  \   \  \
 |    |   \  ___// /_/ | / __ \|  |_|  |   |  (  <_> )    Y    \  |   |  \  | (  (     |    |/    Y    \   )  )
 |____|    \___  >____ |(____  /____/__|___|  /\____/\____|__  /__|___|  /__|  \  \    |____|\____|__  /  /  /
               \/     \/     \/             \/               \/        \/       \__\                 \/  /__/
                                                                                   (c) 2018-2022 alf45star
                                                                       https://github.com/alf45tar/PedalinoMini
 */

#include <algorithm>

void delete_actions()
{
  for (byte b = 0; b < BANKS; b++) {
    //memset(banknames[b], 0, MAXBANKNAME+1);
    action *act = actions[b];
    actions[b] = nullptr;
    while (act != nullptr) {
      action *a = act;
      act = act->next;
      free(a);
    }
  }
}

void sort_actions()
{
  for (byte b = 0; b < BANKS; b++) {
    action *act = actions[b];
    while (act != nullptr) {
      action *idx = act->next;
      while (idx != nullptr) {
        if ((act->pedal > idx->pedal)                                                                       ||
           ((act->pedal == idx->pedal) && (act->button > idx->button))                                      ||
           ((act->pedal == idx->pedal) && (act->event != PED_EVENT_NONE) && (idx->event == PED_EVENT_NONE)) ||
           ((act->pedal == idx->pedal) && (act->event != PED_EVENT_NONE) && (act->button == idx->button) && (act->event > idx->event))) {
          action t;
          strncpy(t.oscAddress,    idx->oscAddress, sizeof(t.oscAddress));
          strncpy(t.tag0,          idx->tag0, MAXACTIONNAME + 1);
          strncpy(t.tag1,          idx->tag1, MAXACTIONNAME + 1);
          t.pedal                = idx->pedal;
          t.button               = idx->button;
          t.event                = idx->event;
          t.midiMessage          = idx->midiMessage;
          t.midiChannel          = idx->midiChannel;
          t.midiCode             = idx->midiCode;
          t.midiValue1           = idx->midiValue1;
          t.midiValue2           = idx->midiValue2;
          t.led                  = idx->led;
          t.color0               = idx->color0;
          t.color1               = idx->color1;
          strncpy(idx->oscAddress, act->oscAddress, sizeof(idx->oscAddress));
          strncpy(idx->tag0,       act->tag0, MAXACTIONNAME + 1);
          strncpy(idx->tag1,       act->tag1, MAXACTIONNAME + 1);
          idx->pedal             = act->pedal;
          idx->button            = act->button;
          idx->event             = act->event;
          idx->midiMessage       = act->midiMessage;
          idx->midiChannel       = act->midiChannel;
          idx->midiCode          = act->midiCode;
          idx->midiValue1        = act->midiValue1;
          idx->midiValue2        = act->midiValue2;
          idx->led               = act->led;
          idx->color0            = act->color0;
          idx->color1            = act->color1;
          strncpy(act->oscAddress, t.oscAddress, sizeof(t.oscAddress));
          strncpy(act->tag0,       t.tag0, MAXACTIONNAME + 1);
          strncpy(act->tag1,       t.tag1, MAXACTIONNAME + 1);
          act->pedal             = t.pedal;
          act->button            = t.button;
          act->event             = t.event;
          act->midiMessage       = t.midiMessage;
          act->midiChannel       = t.midiChannel;
          act->midiCode          = t.midiCode;
          act->midiValue1        = t.midiValue1;
          act->midiValue2        = t.midiValue2;
          act->led               = t.led;
          act->color0            = t.color0;
          act->color1            = t.color1;
        }
        idx = idx->next;
      }
      act = act->next;
    }
  }
}

void create_banks()
{
  for (byte b = 0; b < BANKS; b++) {
    for (byte p = 0; p < PEDALS; p++) {
      action *act = actions[b];
      while ((act != nullptr) && (act->pedal != p)) act = act->next;
      if ((act == nullptr)) {
        memset(banks[b][p].pedalName, 0, MAXACTIONNAME + 1);
        banks[b][p].midiMessage = PED_EMPTY;
        banks[b][p].midiChannel = 1;
        banks[b][p].midiCode    = 0;
        banks[b][p].midiValue1  = 0;
        banks[b][p].midiValue2  = 127;
      } else {
        strncpy(banks[b][p].pedalName, act->name, MAXACTIONNAME + 1);
        banks[b][p].midiMessage = act->midiMessage;
        banks[b][p].midiChannel = act->midiChannel;
        banks[b][p].midiCode    = act->midiCode;
        banks[b][p].midiValue1  = act->midiValue1;
        banks[b][p].midiValue2  = act->midiValue2;
      }
    }
  }

  // Looking for tap action in the current bank
  tapLed    = 0;
  tapColor0 = 0;
  tapColor1 = 0;
  action *act = actions[currentBank];
  while (act != nullptr) {
    if (act->midiMessage == PED_ACTION_TAP) {
      tapLed = led_button(act->pedal, act->button, act->led);
      tapColor0 = act->color0;
      tapColor1 = act->color1;
      act = nullptr;
    }
    else
      act = act->next;
  }
}

void leds_off()
{
  // Set all leds off
  FastLED.clear(true);
}

void leds_refresh()
{
  // Set the last leds color of the current bank after a bank switch or led update
  for (byte l = 0; l < LEDS; l++)
    fastleds[l] = lastLedColor[currentBank][l];
  FastLED.show();
}

void leds_refresh(byte l)
{
  fastleds[l] = lastLedColor[currentBank][l];
  FastLED.show();
}

void leds_update(byte type, byte channel, byte data1, byte data2, byte bank)
{
    action *act = actions[bank];
    while (act != nullptr) {
      if (act->midiChannel == channel) {
        switch (act->midiMessage) {

          case PED_PROGRAM_CHANGE:
          case PED_PROGRAM_CHANGE_INC:
          case PED_PROGRAM_CHANGE_DEC:
            if (type == midi::ProgramChange) {
              if (act->midiCode == data1) {
                lastLedColor[bank][led_button(act->pedal, act->button, act->led)] = act->color1;
                lastLedColor[bank][led_button(act->pedal, act->button, act->led)].nscale8(ledsOnBrightness);
                leds_refresh(led_button(act->pedal, act->button, act->led));
              }
              else {
                lastLedColor[bank][led_button(act->pedal, act->button, act->led)] = act->color0;
                lastLedColor[bank][led_button(act->pedal, act->button, act->led)].nscale8(ledsOffBrightness);
                leds_refresh(led_button(act->pedal, act->button, act->led));
              }
            }
            break;

          case PED_CONTROL_CHANGE:
            if (type == midi::ControlChange && act->midiCode == data1) {
              switch (pedals[act->pedal].mode) {
                case PED_ANALOG:
                case PED_JOG_WHEEL:
                  lastLedColor[bank][led_button(act->pedal, act->button, act->led)] = act->color0;
                  lastLedColor[bank][led_button(act->pedal, act->button, act->led)] = lastLedColor[bank][led_button(act->pedal, act->button, act->led)].lerp8(act->color1, 255 * data2 / (MIDI_RESOLUTION - 1));
                  lastLedColor[bank][led_button(act->pedal, act->button, act->led)].nscale8(ledsOffBrightness + (ledsOnBrightness - ledsOffBrightness) * data2 / (MIDI_RESOLUTION - 1));
                  leds_refresh(act->led);
                  break;

                default:
                  if (data2 == act->midiValue1) {
                    // Led off
                    lastLedColor[bank][led_button(act->pedal, act->button, act->led)] = act->color0;
                    lastLedColor[bank][led_button(act->pedal, act->button, act->led)].nscale8(ledsOffBrightness);
                    leds_refresh(act->led);
                  } else if (data2 == act->midiValue2) {
                    // Led on
                    lastLedColor[bank][led_button(act->pedal, act->button, act->led)] = act->color1;
                    lastLedColor[bank][led_button(act->pedal, act->button, act->led)].nscale8(ledsOnBrightness);
                    leds_refresh(act->led);
                  }
                  break;
              }
            }
            break;

          case PED_NOTE_ON:
            // Invert the status only on NoteOn
            if (type == midi::NoteOn && act->midiCode == data1) {
              CRGB off = act->color0;
              CRGB on  = act->color1;
              off.nscale8(ledsOffBrightness);
              on.nscale8(ledsOnBrightness);
              lastLedColor[bank][led_button(act->pedal, act->button, act->led)] = (fastleds[led_button(act->pedal, act->button, act->led)] == off) ? on : off;
              leds_refresh(act->led);
            }
            break;
        }
      }
      act = act->next;
    }
}

void leds_update(byte type, byte channel, byte data1, byte data2)
{
  for (byte b = 0; b < BANKS; b++)
    leds_update(type, channel, data1, data2, b);
}


//
//
//
void mtc_midi_real_time_message_send(byte b)
{
  static byte click = 0;

  if (interfaces[PED_RTPMIDI].midiClock) AppleMidiSendRealTimeMessage(b);
  if (interfaces[PED_IPMIDI].midiClock)  ipMIDISendRealTimeMessage(b);

  if (!tapLed && !tapColor0 && !tapColor1) return;

  if (click == 0) {
    fastleds[tapLed] = tapColor1;
    fastleds[tapLed].nscale8(ledsOnBrightness);
    fastleds[tapLed] = swap_rgb_order(fastleds[tapLed], rgbOrder);
    FastLED.show();
  }
  else if (click == 8) {
    fastleds[tapLed] = tapColor0;
    fastleds[tapLed].nscale8(ledsOffBrightness);
    fastleds[tapLed] = swap_rgb_order(fastleds[tapLed], rgbOrder);
    FastLED.show();
  }
  click = (click + 1) % 24;
}

void mtc_midi_time_code_quarter_frame_send(byte b)
{
  if (interfaces[PED_RTPMIDI].midiClock) AppleMidiSendTimeCodeQuarterFrame(b);
  if (interfaces[PED_IPMIDI].midiClock)  ipMIDISendTimeCodeQuarterFrame(b);
}

//
// MIDI Time Code/MIDI Clock setup
//
void mtc_setup() {

  MTC.setup(mtc_midi_real_time_message_send, mtc_midi_time_code_quarter_frame_send);

  switch (currentMidiTimeCode) {

    case PED_MTC_NONE:
      DPRINTLN("MTC None");
      MTC.setMode(MidiTimeCode::SynchroNone);
      break;

    case PED_MTC_SLAVE:
      DPRINTLN("MTC Slave");
      MTC.setMode(MidiTimeCode::SynchroMTCSlave);
      break;

    case PED_MTC_MASTER_24:
    case PED_MTC_MASTER_25:
    case PED_MTC_MASTER_30DF:
    case PED_MTC_MASTER_30:
      DPRINTLN("MTC Master");
      MTC.setMode(MidiTimeCode::SynchroMTCMaster);
      MTC.sendPosition(0, 0, 0, 0);
      break;

    case PED_MIDI_CLOCK_SLAVE:
      DPRINTLN("MIDI Clock Slave");
      MTC.setMode(MidiTimeCode::SynchroClockSlave);
      bpm = 0;
      break;

    case PED_MIDI_CLOCK_MASTER:
      DPRINTLN("MIDI Clock Master");
      MTC.setMode(MidiTimeCode::SynchroClockMaster);
      MTC.setBpm(bpm);
      break;
  }
}

void mtc_start()
{
  if (MTC.isPlaying())
    MTC.sendStop();
  else {
    MTC.sendPosition(0, 0, 0, 0);
    MTC.sendPlay();
  }
}

void mtc_stop()
{
  if (MTC.isPlaying())
    MTC.sendStop();
  else
    MTC.sendPosition(0, 0, 0, 0);
}

void mtc_continue()
{
  if (MTC.isPlaying())
    MTC.sendStop();
  else if (MTC.getFrames()  == 0 &&
           MTC.getSeconds() == 0 &&
           MTC.getMinutes() == 0 &&
           MTC.getHours()   == 0)
    MTC.sendPlay();
  else
    MTC.sendContinue();
}

void mtc_tap()
{
  bpm = MTC.tapTempo();
  if      (bpm <  40) MTC.setBpm(40);
  else if (bpm > 300) MTC.setBpm(300);
  else                MTC.setBpm(bpm);
}

void mtc_tap_continue()
{
  switch (MTC.getMode()) {
    case MidiTimeCode::SynchroClockMaster:
      mtc_tap();
      break;
    case MidiTimeCode::SynchroMTCMaster:
      mtc_continue();
      break;
    case MidiTimeCode::SynchroNone:
    case MidiTimeCode::SynchroClockSlave:
    case MidiTimeCode::SynchroMTCSlave:
      break;
  }
}

//
//  Autosensing setup
//
void autosensing_setup()
{
  int tip;    // tip connected to an input digital pin with internal pull-up resistor
  int ring;   // ring connected to an input analog pin
  /*        */// sleeve connected to GND

  return;

  DPRINT("Pedal autosensing...\n");
  analogReadResolution(ADC_RESOLUTION_BITS);
  analogSetAttenuation(ADC_11db);

  for (byte p = 0; p < PEDALS; p++) {
    if (pedals[p].autoSensing && pedals[p].mode != PED_NONE && pedals[p].mode != PED_LADDER) {

      pinMode(PIN_D(p), INPUT_PULLUP);
      pinMode(PIN_A(p), INPUT_PULLUP);
      tip  = analogRead(PIN_D(p));
      ring = analogRead(PIN_A(p));

      DPRINT("Pedal %2d   Tip Pin %2d Value %4d    Ring Pin %2d Value %4d", p + 1, PIN_D(p), tip, PIN_A(p), ring);

      switch (tip) {
        case 0:
          // tip connected to GND
          switch (ring) {
            case 0:
              // tip and ring connected to GND
              // switch between tip and ring normally closed
              pedals[p].mode = PED_MOMENTARY1;
              pedals[p].pressMode = PED_PRESS_1;
              pedals[p].invertPolarity = true;
              DPRINT(" MOMENTARY POLARITY-");
              break;
            case ADC_RESOLUTION-1:
              break;
            default:
              break;
          }
          break;
        case ADC_RESOLUTION-1:
          // tip not connected (pull up resistor)
          switch (ring) {
            case 0:
              break;
            case ADC_RESOLUTION-1:
              // tip and ring not connected (pul up resitor)
              // switch between tip and ring normally open
              pedals[p].mode = PED_MOMENTARY1;
              pedals[p].pressMode = PED_PRESS_1;
              DPRINT(" MOMENTARY");
              break;
            default:
              break;
          }
          break;
        default:
          // tip connected connected to a pot
          pedals[p].mode = PED_ANALOG;
          //pedals[p].invertPolarity = true;
          // inititalize continuos calibration
          pedals[p].expZero = ADC_RESOLUTION - 1;
          pedals[p].expMax = 0;
          DPRINT(" ANALOG POLARITY+");
      }
    }
    else {
      DPRINT("Pedal %2d   autosensing disabled", p + 1);
    }
    DPRINT("\n");
  }
}

void ladder_config()
{
  int input = 0;

  for (byte p = 0; p < PEDALS; p++) {
    if (pedals[p].mode == PED_LADDER) {
      ResponsiveAnalogRead analog(PIN_A(p), true);

      analogReadResolution(ADC_RESOLUTION_BITS);
      analogSetAttenuation(ADC_11db);
      //analogSetClockDiv(255);
      analog.setAnalogResolution(ADC_RESOLUTION);
      analog.enableEdgeSnap();
      pinMode(PIN_D(p), OUTPUT);
      digitalWrite(PIN_D(p), HIGH);

      for (byte i = 0; i < LADDER_STEPS; i++) {
        display_progress_bar_title2("Press and hold", "Switch " + String(i+1));
        for (byte j = 0; j < i; j++)
          display_progress_bar_2_label(j + 1, DISPLAY_WIDTH * ladderLevels[j] / ADC_RESOLUTION);
        unsigned long start = millis();
        while (millis() - start < 3000) {
          display_progress_bar_update(3000 - (millis() - start), 3000);
          input = (9 * input + analogRead(PIN_A(p))) / 10;
          analog.update(input);
          if (analog.hasChanged()) display_progress_bar_2_update(analog.getValue(), ADC_RESOLUTION);
        }
        if (analog.getValue() != ADC_RESOLUTION-1) {
          ladderLevels[i] = analog.getValue();
        }
      }
      display_progress_bar_2_label(LADDER_STEPS, DISPLAY_WIDTH * ladderLevels[LADDER_STEPS-1] / ADC_RESOLUTION);
      delay(1000);
      ladderLevels[LADDER_STEPS] = pedals[p].invertPolarity ? 0 : ADC_RESOLUTION - 1;
      std::sort(ladderLevels, ladderLevels+LADDER_STEPS+1);
      eeprom_update_ladder();
      break;
    }
  }
}

byte map_digital(byte p, byte value)
{
  p = constrain(p, 0, PEDALS - 1);
  return value;
}

unsigned int map_analog(byte p, unsigned int value)
{
  p = constrain(p, 0, PEDALS - 1);
  value = constrain(value, pedals[p].expZero, pedals[p].expMax);                  // make sure that the analog value is between the minimum and maximum value
  value = map2(value, pedals[p].expZero, pedals[p].expMax, 0, ADC_RESOLUTION - 1); // map the value from [minimumValue, maximumValue] to [0, ADC_RESOLUTION-1]
  switch (pedals[p].analogResponse) {
    case PED_LINEAR:
      break;
    case PED_LOG:
      value = round((ADC_RESOLUTION-1)*log(value+1)/log(ADC_RESOLUTION));
      break;
    case PED_ANTILOG:
      value = round((exp(3*value/(double)(ADC_RESOLUTION-1))-1)/(exp(3)-1)*(ADC_RESOLUTION-1));
      break;
  }
  return value;
}


void midi_send(byte message, byte code, byte value, byte channel, bool on_off, byte range_min, byte range_max, byte bank, byte pedal, byte button = 0)
{
  code    = constrain(code,    0, MIDI_RESOLUTION - 1);
  value   = constrain(value,   0, MIDI_RESOLUTION - 1);
  channel = constrain(channel, 1, 16);

  switch (message) {

    case PED_NOTE_ON:
    case PED_NOTE_OFF:

      if (on_off && value > 0) {
        DPRINT("NOTE ON.....Note %3d.....Velocity %3d.....Channel %2d\n", code, value, channel);
        if (interfaces[PED_USBMIDI].midiOut)  USB_MIDI.sendNoteOn(code, value, channel);
        if (interfaces[PED_DINMIDI].midiOut)  DIN_MIDI.sendNoteOn(code, value, channel);
        AppleMidiSendNoteOn(code, value, channel);
        ipMIDISendNoteOn(code, value, channel);
        BLESendNoteOn(code, value, channel);
        OSCSendNoteOn(code, value, channel);
        screen_info(midi::NoteOn, code, value, channel, range_min, range_max);
        currentMIDIValue[bank][pedal][button] = value;
        lastMIDIMessage[currentBank] = {PED_NOTE_ON, code, value, channel};
      }
      else {
        DPRINT("NOTE OFF....Note %3d......Velocity %3d.....Channel %2d\n", code, value, channel);
        if (interfaces[PED_USBMIDI].midiOut)  USB_MIDI.sendNoteOff(code, value, channel);
        if (interfaces[PED_DINMIDI].midiOut)  DIN_MIDI.sendNoteOff(code, value, channel);
        AppleMidiSendNoteOff(code, value, channel);
        ipMIDISendNoteOff(code, value, channel);
        BLESendNoteOff(code, value, channel);
        OSCSendNoteOff(code, value, channel);
        screen_info(midi::NoteOff, code, value, channel, range_min, range_max);
        currentMIDIValue[bank][pedal][button] = value;
        lastMIDIMessage[currentBank] = {PED_NOTE_OFF, code, value, channel};
      }
      break;

    case PED_CONTROL_CHANGE:

      if (on_off) {
        DPRINT("CONTROL CHANGE.....Code %3d......Value %3d.....Channel %2d\n", code, value, channel);
        if (interfaces[PED_USBMIDI].midiOut)  USB_MIDI.sendControlChange(code, value, channel);
        if (interfaces[PED_DINMIDI].midiOut)  DIN_MIDI.sendControlChange(code, value, channel);
        AppleMidiSendControlChange(code, value, channel);
        ipMIDISendControlChange(code, value, channel);
        BLESendControlChange(code, value, channel);
        OSCSendControlChange(code, value, channel);
        screen_info(midi::ControlChange, code, value, channel, range_min, range_max);
        currentMIDIValue[bank][pedal][button] = value;
        lastMIDIMessage[currentBank] = {PED_CONTROL_CHANGE, code, value, channel};
      }
      break;

    case PED_PROGRAM_CHANGE:
    case PED_PROGRAM_CHANGE_INC:
    case PED_PROGRAM_CHANGE_DEC:

      if (on_off) {
        DPRINT("PROGRAM CHANGE.....Program %3d.....Channel %2d\n", code, channel);
        if (interfaces[PED_USBMIDI].midiOut)  USB_MIDI.sendProgramChange(code, channel);
        if (interfaces[PED_DINMIDI].midiOut)  DIN_MIDI.sendProgramChange(code, channel);
        AppleMidiSendProgramChange(code, channel);
        ipMIDISendProgramChange(code, channel);
        BLESendProgramChange(code, channel);
        OSCSendProgramChange(code, channel);
        screen_info(midi::ProgramChange, code, 0, channel, range_min, range_max);
        currentMIDIValue[bank][pedal][button] = value;
        lastMIDIMessage[currentBank] = {PED_PROGRAM_CHANGE, code, 0, channel};
        leds_update(midi::ProgramChange, channel, code, 0);
      }
      break;

    case PED_BANK_SELECT_INC:
    case PED_BANK_SELECT_DEC:

      if (on_off) {
        // MSB
        DPRINT("CONTROL CHANGE.....Code %3d.....Value %3d.....Channel %2d\n", midi::BankSelect, code, channel);
        if (interfaces[PED_USBMIDI].midiOut)  USB_MIDI.sendControlChange(midi::BankSelect, code, channel);
        if (interfaces[PED_DINMIDI].midiOut)  DIN_MIDI.sendControlChange(midi::BankSelect, code, channel);
        AppleMidiSendControlChange(midi::BankSelect, code, channel);
        ipMIDISendControlChange(midi::BankSelect, code, channel);
        BLESendControlChange(midi::BankSelect, code, channel);
        OSCSendControlChange(midi::BankSelect, code, channel);
        // LSB
        DPRINT("CONTROL CHANGE.....Code %3d.....Value %3d.....Channel %2d\n", midi::BankSelect+32, value, channel);
        if (interfaces[PED_USBMIDI].midiOut)  USB_MIDI.sendControlChange(midi::BankSelect+32, value, channel);
        if (interfaces[PED_DINMIDI].midiOut)  DIN_MIDI.sendControlChange(midi::BankSelect+32, value, channel);
        AppleMidiSendControlChange(midi::BankSelect+32, value, channel);
        ipMIDISendControlChange(midi::BankSelect+32, value, channel);
        BLESendControlChange(midi::BankSelect+32, value, channel);
        OSCSendControlChange(midi::BankSelect+32, value, channel);
        screen_info(midi::ControlChange, midi::BankSelect+32, value, channel, range_min, range_max);
        currentMIDIValue[bank][pedal][button] = value;
      }
      break;

    case PED_PITCH_BEND:

      if (on_off) {
        int bend = map2(value, 0, MIDI_RESOLUTION - 1, MIDI_PITCHBEND_MIN, MIDI_PITCHBEND_MAX);
        DPRINT("PITCH BEND.....Value %5d.....Channel %2d\n", bend, channel);
        if (interfaces[PED_USBMIDI].midiOut)  USB_MIDI.sendPitchBend(bend, channel);
        if (interfaces[PED_DINMIDI].midiOut)  DIN_MIDI.sendPitchBend(bend, channel);
        AppleMidiSendPitchBend(bend, channel);
        ipMIDISendPitchBend(bend, channel);
        BLESendPitchBend(bend, channel);
        OSCSendPitchBend(bend, channel);
        const unsigned ubend = unsigned(bend - int(MIDI_PITCHBEND_MIN));
        screen_info(midi::PitchBend, ubend & 0x7f, (ubend >> 7) & 0x7f, channel, range_min, range_max);
        currentMIDIValue[bank][pedal][button] = value;
        lastMIDIMessage[currentBank] = {PED_PITCH_BEND, code, value, channel};
      }
      break;

    case PED_CHANNEL_PRESSURE:

      if (on_off) {
        DPRINT("CHANNEL PRESSURE.....Channel %2d\n", channel);
        if (interfaces[PED_USBMIDI].midiOut)  USB_MIDI.sendAfterTouch(value, channel);
        if (interfaces[PED_DINMIDI].midiOut)  DIN_MIDI.sendAfterTouch(value, channel);
        AppleMidiSendAfterTouch(value, channel);
        ipMIDISendAfterTouch(value, channel);
        BLESendAfterTouch(value, channel);
        OSCSendAfterTouch(value, channel);
        screen_info(midi::AfterTouchChannel, value, 0, channel, range_min, range_max);
        currentMIDIValue[bank][pedal][button] = value;
        lastMIDIMessage[currentBank] = {PED_CHANNEL_PRESSURE, code, value, channel};
      }
      break;

    case PED_MIDI_START:

      if (on_off) {
        DPRINT("START.....\n");
        if (interfaces[PED_USBMIDI].midiOut)  USB_MIDI.sendRealTime(midi::Start);
        if (interfaces[PED_DINMIDI].midiOut)  DIN_MIDI.sendRealTime(midi::Start);
        AppleMidiSendStart();
        ipMIDISendStart();
        BLESendStart();
        OSCSendStart();
        screen_info(midi::Start, 0, 0, 0, range_min, range_max);
        currentMIDIValue[bank][pedal][button] = 0;
      }
      break;

    case PED_MIDI_STOP:

      if (on_off) {
        DPRINT("STOP.....\n");
        if (interfaces[PED_USBMIDI].midiOut)  USB_MIDI.sendRealTime(midi::Stop);
        if (interfaces[PED_DINMIDI].midiOut)  DIN_MIDI.sendRealTime(midi::Stop);
        AppleMidiSendStop();
        ipMIDISendStop();
        BLESendStop();
        OSCSendStop();
        screen_info(midi::Stop, 0, 0, 0, range_min, range_max);
        currentMIDIValue[bank][pedal][button] = 0;
      }
      break;

    case PED_MIDI_CONTINUE:

      if (on_off) {
        DPRINT("CONTINUE.....\n");
        if (interfaces[PED_USBMIDI].midiOut)  USB_MIDI.sendRealTime(midi::Continue);
        if (interfaces[PED_DINMIDI].midiOut)  DIN_MIDI.sendRealTime(midi::Continue);
        AppleMidiSendContinue();
        ipMIDISendContinue();
        BLESendContinue();
        OSCSendContinue();
        screen_info(midi::Continue, 0, 0, 0, range_min, range_max);
        currentMIDIValue[bank][pedal][button] = 0;
      }
      break;

    case PED_SEQUENCE:
      channel = constrain(channel, 1, SEQUENCES);
      DPRINT("=======================================================\n");
      DPRINT("SEQUENCE.....Number %2d\n", channel);
      DPRINT("-------------------------------------------------------\n");
      for (byte s = 0; s < STEPS; s++)
        if (sequences[channel-1][s].midiMessage == PED_ACTION_LED_COLOR) {
          sequences[channel-1][s].led = constrain(sequences[channel-1][s].led, 0, LEDS);
          fastleds[sequences[channel-1][s].led] = sequences[channel-1][s].color;
          fastleds[sequences[channel-1][s].led].nscale8(ledsOnBrightness);
          DPRINT("LED COLOR.....Led %2d......RGB Color #%02x%02x%02x\n", sequences[channel-1][s].led + 1, fastleds[sequences[channel-1][s].led].red, fastleds[sequences[channel-1][s].led].green, fastleds[sequences[channel-1][s].led].blue);
          fastleds[sequences[channel-1][s].led] = swap_rgb_order(fastleds[sequences[channel-1][s].led], rgbOrder);
          FastLED.show();
          lastLedColor[currentBank][sequences[channel-1][s].led] = fastleds[sequences[channel-1][s].led];
        }
        else
          midi_send(sequences[channel-1][s].midiMessage, sequences[channel-1][s].midiCode, sequences[channel-1][s].midiValue, sequences[channel-1][s].midiChannel, on_off, 0, MIDI_RESOLUTION - 1, bank, pedal);
      DPRINT("=======================================================\n");
      currentMIDIValue[bank][pedal][button] = channel;
      lastMIDIMessage[currentBank] = {PED_SEQUENCE, code, value, channel};
      break;
  }
}

//
// Trigger Actions on Analog Events
//
void controller_event_handler_analog(byte pedal, int value)
{
      bool    global = actions[0] != nullptr;
      action *act    = actions[0] == nullptr ? actions[currentBank] : actions[0];
      while (act != nullptr) {
        if (act->pedal == pedal && act->event == PED_EVENT_MOVE) {
          if (act->midiMessage == PED_ACTION_REPEAT) {
            value = map2(value,                                      // map from [0, ADC_RESOLUTION-1] to [0, 127] MIDI value
                      0,
                      ADC_RESOLUTION - 1,
                      pedals[pedal].invertPolarity ? MIDI_RESOLUTION - 1 : 0,
                      pedals[pedal].invertPolarity ? 0 : MIDI_RESOLUTION - 1);
            value = constrain(value, 0, MIDI_RESOLUTION - 1);
          }
          else if (act->midiMessage != PED_OSC_MESSAGE) {
            value = map2(value,                                      // map from [0, ADC_RESOLUTION-1] to [min, max] MIDI value
                      0,
                      ADC_RESOLUTION - 1,
                      pedals[pedal].invertPolarity ? act->midiValue2 : act->midiValue1,
                      pedals[pedal].invertPolarity ? act->midiValue1 : act->midiValue2);
            value = constrain(value, act->midiValue1, act->midiValue2);
          }
          lastUsedPedal = pedal;
          lastUsed = pedal;
          lastSlot = act->slot;
          if (act->midiMessage != PED_EMPTY ||
              act->midiMessage != PED_ACTION_REPEAT ||
              act->midiMessage != PED_ACTION_REPEAT_OVERWRITE) strncpy(lastPedalName, act->name, MAXACTIONNAME+1);
          DPRINT("Action: %s\n", act->name);
          switch (act->midiMessage) {
            case PED_ACTION_REPEAT:
              midi_send(lastMIDIMessage[currentBank].midiMessage, lastMIDIMessage[currentBank].midiCode, value, lastMIDIMessage[currentBank].midiChannel, true, 0, MIDI_RESOLUTION - 1, currentBank, pedal);
              fastleds[led_button(act->pedal, act->button, act->led)] = lastColor0;
              fastleds[led_button(act->pedal, act->button, act->led)] = fastleds[led_button(act->pedal, act->button, act->led)].lerp8(lastColor1, 255 * value / (MIDI_RESOLUTION - 1));
              fastleds[led_button(act->pedal, act->button, act->led)].nscale8(ledsOffBrightness + (ledsOnBrightness - ledsOffBrightness) * value / (MIDI_RESOLUTION - 1));
              fastleds[led_button(act->pedal, act->button, act->led)] = swap_rgb_order(fastleds[led_button(act->pedal, act->button, act->led)], rgbOrder);
              FastLED.show();
              lastLedColor[currentBank][led_button(act->pedal, act->button, act->led)] = fastleds[led_button(act->pedal, act->button, act->led)];
              break;

            case PED_ACTION_REPEAT_OVERWRITE:
              midi_send(lastMIDIMessage[currentBank].midiMessage, lastMIDIMessage[currentBank].midiCode, value, act->midiChannel, true, act->midiValue1, act->midiValue2, currentBank, pedal);
              fastleds[led_button(act->pedal, act->button, act->led)] = act->color0;
              fastleds[led_button(act->pedal, act->button, act->led)] = fastleds[led_button(act->pedal, act->button, act->led)].lerp8(act->color1, 255 * value / (MIDI_RESOLUTION - 1));
              fastleds[led_button(act->pedal, act->button, act->led)].nscale8(ledsOffBrightness + (ledsOnBrightness - ledsOffBrightness) * value / (MIDI_RESOLUTION - 1));
              fastleds[led_button(act->pedal, act->button, act->led)] = swap_rgb_order(fastleds[led_button(act->pedal, act->button, act->led)], rgbOrder);
              FastLED.show();
              lastLedColor[currentBank][led_button(act->pedal, act->button, act->led)] = fastleds[led_button(act->pedal, act->button, act->led)];
              break;

            case PED_CONTROL_CHANGE:
            case PED_NOTE_ON:
            case PED_NOTE_OFF:
            case PED_PITCH_BEND:
            case PED_CHANNEL_PRESSURE:
              midi_send(act->midiMessage, act->midiCode, value, act->midiChannel, true, act->midiValue1, act->midiValue2, currentBank, pedal);
              fastleds[led_button(act->pedal, act->button, act->led)] = act->color0;
              fastleds[led_button(act->pedal, act->button, act->led)] = fastleds[led_button(act->pedal, act->button, act->led)].lerp8(act->color1, 255 * value / (MIDI_RESOLUTION - 1));
              fastleds[led_button(act->pedal, act->button, act->led)].nscale8(ledsOffBrightness + (ledsOnBrightness - ledsOffBrightness) * value / (MIDI_RESOLUTION - 1));
              fastleds[led_button(act->pedal, act->button, act->led)] = swap_rgb_order(fastleds[led_button(act->pedal, act->button, act->led)], rgbOrder);
              FastLED.show();
              lastLedColor[currentBank][led_button(act->pedal, act->button, act->led)] = fastleds[led_button(act->pedal, act->button, act->led)];
              break;

            case PED_SEQUENCE:
              midi_send(act->midiMessage, act->midiCode, value, act->midiChannel, true, act->midiValue1, act->midiValue2, currentBank, pedal);
              fastleds[led_button(act->pedal, act->button, act->led)] = act->color0;
              fastleds[led_button(act->pedal, act->button, act->led)] = fastleds[led_button(act->pedal, act->button, act->led)].lerp8(act->color1, 255 * value / (MIDI_RESOLUTION - 1));
              fastleds[led_button(act->pedal, act->button, act->led)].nscale8(ledsOffBrightness + (ledsOnBrightness - ledsOffBrightness) * value / (MIDI_RESOLUTION - 1));
              fastleds[led_button(act->pedal, act->button, act->led)] = swap_rgb_order(fastleds[led_button(act->pedal, act->button, act->led)], rgbOrder);
              FastLED.show();
              lastLedColor[currentBank][led_button(act->pedal, act->button, act->led)] = fastleds[led_button(act->pedal, act->button, act->led)];
              break;

            case PED_PROGRAM_CHANGE:
              lastProgramChange[act->midiChannel] = act->midiCode;
              midi_send(act->midiMessage, act->midiCode, value, act->midiChannel, true, act->midiValue1, act->midiValue2, currentBank, pedal);
              break;

            case PED_BANK_SELECT_INC:
            case PED_BANK_SELECT_DEC:
              lastBankSelect[act->midiChannel] = (act->midiCode << 7) + constrain(value, act->midiValue1, act->midiValue2);
              midi_send(act->midiMessage, (lastBankSelect[act->midiChannel] & 0b0011111110000000) >> 7, lastBankSelect[act->midiChannel] & 0b0000000001111111, act->midiChannel, true, act->midiValue1, act->midiValue2, currentBank, pedal);
              break;

            case PED_PROGRAM_CHANGE_INC:
            case PED_PROGRAM_CHANGE_DEC:
              lastProgramChange[act->midiChannel] = constrain(value, act->midiValue1, act->midiValue2);
              midi_send(act->midiMessage, lastProgramChange[act->midiChannel], 0, act->midiChannel, true, act->midiValue1, act->midiValue2, currentBank, pedal);
              break;

            case PED_ACTION_BANK_PLUS:
            case PED_ACTION_BANK_MINUS:
              currentBank = map2(value, 0, MIDI_RESOLUTION - 1, constrain(act->midiValue1 - 1, 0, BANKS - 1), constrain(act->midiValue2 - 1, 0, BANKS - 1));
              currentBank = constrain(currentBank, 0, BANKS - 1);
              if (repeatOnBankSwitch)
                midi_send(lastMIDIMessage[currentBank].midiMessage,
                          lastMIDIMessage[currentBank].midiCode,
                          lastMIDIMessage[currentBank].midiValue,
                          lastMIDIMessage[currentBank].midiChannel,
                          true,
                          0, MIDI_RESOLUTION - 1,
                          currentBank, act->pedal);
              leds_refresh();
              break;

            case PED_ACTION_TAP:
            case PED_ACTION_BPM_PLUS:
            case PED_ACTION_BPM_MINUS:
              bpm = map2(value, 0, MIDI_RESOLUTION, 40, 300);
              MTC.setBpm(bpm);
              break;

            case PED_OSC_MESSAGE:
              OSCSendMessage(act->oscAddress, map2(value, 0, ADC_RESOLUTION - 1, act->midiValue1*1000, act->midiValue2*1000) / (float)1000.0);
              fastleds[led_button(act->pedal, act->button, act->led)] = act->color0;
              fastleds[led_button(act->pedal, act->button, act->led)] = fastleds[led_button(act->pedal, act->button, act->led)].lerp8(act->color1, 255 * value / (MIDI_RESOLUTION - 1));
              fastleds[led_button(act->pedal, act->button, act->led)].nscale8(ledsOffBrightness + (ledsOnBrightness - ledsOffBrightness) * value / (MIDI_RESOLUTION - 1));
              fastleds[led_button(act->pedal, act->button, act->led)] = swap_rgb_order(fastleds[led_button(act->pedal, act->button, act->led)], rgbOrder);
              FastLED.show();
              lastLedColor[currentBank][led_button(act->pedal, act->button, act->led)] = fastleds[led_button(act->pedal, act->button, act->led)];
              break;

            case PED_ACTION_LED_COLOR:
              fastleds[led_button(act->pedal, act->button, act->led)] = act->color0;
              fastleds[led_button(act->pedal, act->button, act->led)] = fastleds[led_button(act->pedal, act->button, act->led)].lerp8(act->color1, 255 * value / (MIDI_RESOLUTION - 1));
              fastleds[led_button(act->pedal, act->button, act->led)].nscale8(ledsOffBrightness + (ledsOnBrightness - ledsOffBrightness) * value / (MIDI_RESOLUTION - 1));
              fastleds[led_button(act->pedal, act->button, act->led)] = swap_rgb_order(fastleds[led_button(act->pedal, act->button, act->led)], rgbOrder);
              FastLED.show();
              lastLedColor[currentBank][led_button(act->pedal, act->button, act->led)] = fastleds[led_button(act->pedal, act->button, act->led)];
              break;
          }
        }
        act = act->next;
        if (global && (currentBank != 0) && (act == nullptr)) {
          global = false;
          act = actions[currentBank];
        }
      }
}


void refresh_analog(byte i, bool send)
{
  const int SAFE_ZONE = ADC_RESOLUTION / 20;                // 5% of margin at both end of the scale

  int input;
  int value;

  if (pedals[i].analogPedal == nullptr) return;             // sanity check

  input = analogRead(PIN_A(i));                             // read the raw analog input value
  if (pedals[i].autoSensing) {                              // continuos calibration
    if (pedals[i].expZero > (input + SAFE_ZONE)) {
      DPRINT("Pedal %2d calibration min %d\n", i + 1, input);
    }
    if (pedals[i].expMax < (input - SAFE_ZONE)) {
      DPRINT("Pedal %2d calibration max %d\n", i + 1, input);
    }
    pedals[i].expZero = _min(pedals[i].expZero, input + SAFE_ZONE);
    pedals[i].expMax  = _max(pedals[i].expMax,  input - SAFE_ZONE);
    //DPRINT("%d -> [%d, %d]\n", input, pedals[i].expZero, pedals[i].expMax);
  }
  value = map_analog(i, input);                             // expand to [0, ADC_RESOLUTION-1] and apply the map function
  pedals[i].analogPedal->update(value);                     // update the responsive analog average
  if (pedals[i].analogPedal->hasChanged()) {                // if the value changed since last time
    value = pedals[i].analogPedal->getValue();              // get the responsive analog average value
    double velocity = (1000.0 * (value - pedals[i].pedalValue[0])) / (micros() - pedals[i].lastUpdate[0]);
    DPRINT("Pedal %2d   input %d output %d velocity %.2f\n", i + 1, input, value, velocity);
    if (send) controller_event_handler_analog(i, value);
    pedals[i].pedalValue[0] = value;
    pedals[i].lastUpdate[0] = micros();
  }
 }

void refresh_ultrasonic(bool send)
{
  //const int SAFE_ZONE = ADC_RESOLUTION / 20;                // 5% of margin at both end of the scale

  long input;
  int value;

  if (ultrasonicTrigger) {
    long* distances = HCSR04.measureMicroseconds();
    byte p = 0;
    for (byte i = 0; i < PEDALS && p < ultrasonicEcho.size(); i++) {
      if (pedals[i].mode == PED_ULTRASONIC) {
        input = distances[p];
        if (input <= 0) continue;
        /*
        if (pedals[i].autoSensing) {                              // continuos calibration
          if (pedals[i].expZero > (input + SAFE_ZONE)) {
            DPRINT("Pedal %2d calibration min %d\n", i + 1, input);
          }
          if (pedals[i].expMax < (input - SAFE_ZONE)) {
            DPRINT("Pedal %2d calibration max %d\n", i + 1, input);
          }
          pedals[i].expZero = _min(pedals[i].expZero, input + SAFE_ZONE);
          pedals[i].expMax  = _max(pedals[i].expMax,  input - SAFE_ZONE);
          //DPRINT("%d -> [%d, %d]\n", input, pedals[i].expZero, pedals[i].expMax);
        }
        */

        value = map_analog(i, input);                             // expand to [0, ADC_RESOLUTION-1] and apply the map function
        pedals[i].analogPedal->update(value);                     // update the responsive analog average
        if (pedals[i].analogPedal->hasChanged()) {                // if the value changed since last time
          value = pedals[i].analogPedal->getValue();              // get the responsive analog average value
          double velocity = (1000.0 * (value - pedals[i].pedalValue[0])) / (micros() - pedals[i].lastUpdate[0]);
          DPRINT("Pedal %2d   input %d output %d velocity %.2f\n", i + 1, input, value, velocity);
        if (send) controller_event_handler_analog(i, value);
          pedals[i].pedalValue[0] = value;
          pedals[i].lastUpdate[0] = micros();
        }
        p++;
      }
    }
  }
}


void controller_setup();

//
//  Delete previous setup
//
void controller_delete()
{
  for (byte i = 0; i < PEDALS; i++) {
    if (pedals[i].analogPedal   != nullptr)  {
      delete pedals[i].analogPedal;
      pedals[i].analogPedal = nullptr;
    }
    if (pedals[i].jogwheel      != nullptr)  {
      delete pedals[i].jogwheel;
      pedals[i].jogwheel = nullptr;
    }
    if (pedals[i].buttonConfig  != nullptr)  {
      delete pedals[i].buttonConfig;
      pedals[i].buttonConfig = nullptr;
    }
    for (byte s = 0; s < LADDER_STEPS; s++)
      if (pedals[i].button[s]   != nullptr)  {
        delete pedals[i].button[s];
        pedals[i].button[s] = nullptr;
      }
  }
  ultrasonicTrigger = 0;
  ultrasonicEcho.clear();
}

//
//  Refresh pedals
//
void controller_run(bool send = true)
{
  if (saveProfile && send) {
    saveProfile = false;
    DPRINT("Saving profile ...\n");
    eeprom_update_current_profile(currentProfile);
    return;
  }

  if (reloadProfile && send) {
    reloadProfile = false;
    DPRINT("Loading profile %d ...\n", currentProfile);
    eeprom_read_profile(currentProfile);
    lastColor0 = CRGB::Black;
    lastColor1 = CRGB::Black;
    for (byte b = 0; b < BANKS; b++) {
      lastMIDIMessage[b] = {PED_EMPTY, 0, 0, 1};
      for (byte p = 0; p < PEDALS; p++)
        for (byte i = 0; i < LADDER_STEPS; i++)
          currentMIDIValue[b][p][i] = 0;
      action *act = actions[b];
      while (act != nullptr) {
        if (act->midiMessage == midi::ControlChange) {
          if (act->tag1[0] != 0 && act->tag1[strlen(act->tag1) - 1] == '.')
            currentMIDIValue[b][act->pedal][act->button] = act->midiValue2;
          else
            currentMIDIValue[b][act->pedal][act->button] = act->midiValue1;
        }
        act = act->next;
      }
    }
    for (byte i = 0; i < 16; i++) {
      lastProgramChange[i] = 0;
      lastBankSelect[i]    = 0;
    }
    autosensing_setup();
    controller_setup();
    mtc_setup();
#ifdef WIFI
    //OscControllerUpdate();
#endif
    DPRINT("... profile %d loaded.\n", currentProfile);
    return;
  }

  if (loadConfig && send) {
    loadConfig = false;
    DPRINT("Loading configuration ...\n");
    autosensing_setup();
    controller_setup();
    mtc_setup();
#ifdef WIFI
    OscControllerUpdate();
#endif
    DPRINT("... configuration loaded.\n");
    return;
  }

  for (byte i = 0; i < PEDALS; i++) {
    switch (pedals[i].mode) {

      case PED_MOMENTARY1:
      case PED_LATCH1:
        if (pedals[i].button[0] != nullptr) pedals[i].button[0]->check();
        break;

      case PED_MOMENTARY2:
      case PED_LATCH2:
        if (pedals[i].button[0] != nullptr) pedals[i].button[0]->check();
        if (pedals[i].button[1] != nullptr) pedals[i].button[1]->check();
        break;

      case PED_MOMENTARY3:
        if (pedals[i].button[0] != nullptr) pedals[i].button[0]->check();
        if (pedals[i].button[1] != nullptr) pedals[i].button[1]->check();
        if (pedals[i].button[2] != nullptr) pedals[i].button[2]->check();
        break;

      case PED_ANALOG:
        refresh_analog(i, send);
        break;

      case PED_ANALOG_MOMENTARY:
      case PED_ANALOG_LATCH:
        if (pedals[i].button[0] != nullptr) pedals[i].button[0]->check();
        refresh_analog(i, send);
        break;

      case PED_LADDER:
        for (byte s = 0; s < LADDER_STEPS; s++)
          if (pedals[i].button[s] != nullptr) pedals[i].button[s]->check();
        break;

      case PED_JOG_WHEEL:
        if (pedals[i].jogwheel == nullptr) break;
        uint8_t direction = pedals[i].jogwheel->read();
        switch (direction) {
          case DIR_NONE:
            break;
          case DIR_CW:
          case DIR_CCW:
            bool    global = actions[0] != nullptr;
            action *act    = actions[0] == nullptr ? actions[currentBank] : actions[0];
            while (act != nullptr) {
              if (act->pedal == i && act->event == PED_EVENT_JOG) {
                switch (act->midiMessage) {

                  case PED_EMPTY:
                    break;

                  case PED_PROGRAM_CHANGE:
                  case PED_CONTROL_CHANGE:
                  case PED_NOTE_ON:
                  case PED_NOTE_OFF:
                  case PED_PITCH_BEND:
                  case PED_CHANNEL_PRESSURE:
                  case PED_BANK_SELECT_INC:
                  case PED_BANK_SELECT_DEC:
                  case PED_PROGRAM_CHANGE_INC:
                  case PED_PROGRAM_CHANGE_DEC:
                  case PED_SEQUENCE:
                    currentMIDIValue[currentBank][i][0] = constrain(currentMIDIValue[currentBank][i][0] +
                                                           ((direction == DIR_CW) ? 1 : -1) *
                                                           _max(1, (pedals[i].jogwheel->speed() + 1) * (act->midiValue2 - act->midiValue1) / (MIDI_RESOLUTION - 1)),
                                                           pedals[i].invertPolarity ? act->midiValue2 : act->midiValue1,
                                                           pedals[i].invertPolarity ? act->midiValue1 : act->midiValue2);
                    DPRINT("Pedal %2d   input %d output %d velocity %d\n", i + 1, ((direction == DIR_CW) ? 1 : -1), currentMIDIValue[currentBank][i][0], pedals[i].jogwheel->speed());
                    if (send) {
                      switch (act->midiMessage) {
                        case PED_PROGRAM_CHANGE:
                        case PED_PROGRAM_CHANGE_INC:
                        case PED_PROGRAM_CHANGE_DEC:
                        case PED_BANK_SELECT_INC:
                        case PED_BANK_SELECT_DEC:
                          midi_send(act->midiMessage, currentMIDIValue[currentBank][i][0], 0, act->midiChannel, true, act->midiValue1, act->midiValue2, currentBank, i);
                          midi_send(act->midiMessage, currentMIDIValue[currentBank][i][0], 0, act->midiChannel, false, act->midiValue1, act->midiValue2, currentBank, i);
                          break;
                        default:
                          midi_send(act->midiMessage, act->midiCode, currentMIDIValue[currentBank][i][0], act->midiChannel, true, act->midiValue1, act->midiValue2, currentBank, i);
                          midi_send(act->midiMessage, act->midiCode, currentMIDIValue[currentBank][i][0], act->midiChannel, false, act->midiValue1, act->midiValue2, currentBank, i);
                          break;
                      }
                    }
                    pedals[i].lastUpdate[0] = micros();
                    lastUsedPedal = i;
                    lastUsed = i;
                    lastSlot = act->slot;
                    strncpy(lastPedalName, act->name, MAXACTIONNAME+1);
                    break;

                  case PED_ACTION_BANK_PLUS:
                  case PED_ACTION_BANK_MINUS:
                    {
                      int b = currentBank + ((direction == DIR_CW) ? 1 : -1) * (pedals[i].invertPolarity ? -1 : 1);
                      b = constrain(b, constrain(act->midiValue1, 0, BANKS - 1), constrain(act->midiValue2, 0, BANKS - 1));
                      currentBank = constrain(b, 0, BANKS - 1);
                      leds_refresh();
                      break;
                    }
                  case PED_ACTION_TAP:
                  case PED_ACTION_BPM_PLUS:
                  case PED_ACTION_BPM_MINUS:
                    bpm = constrain(bpm + ((direction == DIR_CW) ? 1 : -1) * (pedals[i].jogwheel->speed() + 1),
                                    pedals[i].invertPolarity ? 300 : 40,
                                    pedals[i].invertPolarity ? 40 : 300);
                    MTC.setBpm(bpm);
                    break;

                  case PED_ACTION_LED_COLOR:
                    fastleds[led_button(act->pedal, act->button, act->led)] = act->color0;
                    fastleds[led_button(act->pedal, act->button, act->led)] = fastleds[led_button(act->pedal, act->button, act->led)].lerp8(act->color1, 255 * 0 / (MIDI_RESOLUTION - 1));
                    fastleds[led_button(act->pedal, act->button, act->led)].nscale8(ledsOffBrightness + (ledsOnBrightness - ledsOffBrightness) * 0 / (MIDI_RESOLUTION - 1));
                    fastleds[led_button(act->pedal, act->button, act->led)] = swap_rgb_order(fastleds[led_button(act->pedal, act->button, act->led)], rgbOrder);
                    FastLED.show();
                    lastLedColor[currentBank][led_button(act->pedal, act->button, act->led)] = fastleds[led_button(act->pedal, act->button, act->led)];
                    break;
                }
                DPRINT("Action: %s\n", act->name);
              }
              act = act->next;
              if (global && (currentBank != 0) && (act == nullptr)) {
                global = false;
                act = actions[currentBank];
              }
            }
            break;
        }
        break;
    }
  }

  refresh_ultrasonic(send);
}

//
// Trigger Actions on Buttons Events
//
void controller_event_handler_button(AceButton* button, uint8_t eventType, uint8_t buttonState)
{
  byte p = constrain(button->getId() / 10 - 1, 0, PEDALS - 1);
  byte i = constrain(button->getId() % 10 - 1, 0, LADDER_STEPS - 1);

  DPRINT("Pedal: %d     Button: %d    EventType: %d     ButtonState: %d\n", p + 1, i + 1, eventType, buttonState);

  if (pedals[p].pressMode == 0) return;

  switch (eventType) {
    case AceButton::kEventClicked:
    case AceButton::kEventPressed:
    case AceButton::kEventReleased:
      if (!IS_SINGLE_PRESS_ENABLED(pedals[p].pressMode)) return;
      break;

    case AceButton::kEventDoubleClicked:
      if (!IS_DOUBLE_PRESS_ENABLED(pedals[p].pressMode)) return;
      break;

    case AceButton::kEventLongPressed:
    case AceButton::kEventLongReleased:
    case AceButton::kEventRepeatPressed:
      if (!IS_LONG_PRESS_ENABLED(pedals[p].pressMode)) return;
      break;
   }

      bool    global = actions[0] != nullptr;
      action *act    = actions[0] == nullptr ? actions[currentBank] : actions[0];
      while (act != nullptr) {
        if ((act->pedal == p) && (act->button == i) && (act->event == eventType)) {
          pedals[p].lastUpdate[0] = micros();
          lastUsedPedal = p;
          lastUsed = p;
          lastSlot = act->slot;
          strncpy(lastPedalName, act->name, MAXACTIONNAME+1);
          DPRINT("Action: %s\n", act->name);
          lastSlot = act->slot;
          switch (act->midiMessage) {
            case PED_EMPTY:
              break;

            case PED_NOTE_ON:
            case PED_NOTE_OFF:
            case PED_PITCH_BEND:
            case PED_CHANNEL_PRESSURE:
            case PED_MIDI_START:
            case PED_MIDI_STOP:
            case PED_MIDI_CONTINUE:
            case PED_SEQUENCE:
              midi_send(act->midiMessage, act->midiCode, act->midiValue1, act->midiChannel, true, act->midiValue1, act->midiValue2, currentBank, p, i);
              break;

            case PED_PROGRAM_CHANGE:
              lastProgramChange[act->midiChannel] = act->midiCode;
              midi_send(act->midiMessage, act->midiCode, act->midiValue1, act->midiChannel, true, act->midiValue1, act->midiValue2, currentBank, p, i);
              break;

            case PED_CONTROL_CHANGE:
              if ((pedals[p].mode == PED_MOMENTARY1 ||
                   pedals[p].mode == PED_MOMENTARY2 ||
                   pedals[p].mode == PED_MOMENTARY3 ||
                   pedals[p].mode == PED_LADDER     ||
                   pedals[p].mode == PED_ANALOG_MOMENTARY) && (currentMIDIValue[currentBank][p][i] == act->midiValue1)) {
                midi_send(act->midiMessage, act->midiCode, act->midiValue2, act->midiChannel, true, act->midiValue1, act->midiValue2, currentBank, p, i);
                strncpy(lastPedalName, act->tag1, MAXACTIONNAME+1);
              }
              else {
                midi_send(act->midiMessage, act->midiCode, act->midiValue1, act->midiChannel, true, act->midiValue1, act->midiValue2, currentBank, p, i);
                strncpy(lastPedalName, act->tag0, MAXACTIONNAME+1);
              }
              break;

            case PED_BANK_SELECT_INC:
              lastBankSelect[act->midiChannel] = (act->midiCode << 7) + constrain((lastBankSelect[act->midiChannel] + 1) & 0b0000000001111111, act->midiValue1, act->midiValue2);
              midi_send(act->midiMessage, (lastBankSelect[act->midiChannel] & 0b0011111110000000) >> 7, lastBankSelect[act->midiChannel] & 0b0000000001111111, act->midiChannel, true, act->midiValue1, act->midiValue2, currentBank, p, i);
              break;

            case PED_BANK_SELECT_DEC:
              lastBankSelect[act->midiChannel] = (act->midiCode << 7) + constrain((lastBankSelect[act->midiChannel] - 1) & 0b0000000001111111, act->midiValue1, act->midiValue2);
              midi_send(act->midiMessage, (lastBankSelect[act->midiChannel] & 0b0011111110000000) >> 7, lastBankSelect[act->midiChannel] & 0b0000000001111111, act->midiChannel, true, act->midiValue1, act->midiValue2, currentBank, p, i);
              break;

            case PED_PROGRAM_CHANGE_INC:
              lastProgramChange[act->midiChannel] = (lastProgramChange[act->midiChannel] == act->midiValue2) ? act->midiValue1 : lastProgramChange[act->midiChannel] + 1;
              lastProgramChange[act->midiChannel] = constrain(lastProgramChange[act->midiChannel], act->midiValue1, act->midiValue2);
              midi_send(act->midiMessage, lastProgramChange[act->midiChannel], 0, act->midiChannel, true, act->midiValue1, act->midiValue2, currentBank, p, i);
              break;

            case PED_PROGRAM_CHANGE_DEC:
              lastProgramChange[act->midiChannel] = (lastProgramChange[act->midiChannel] == act->midiValue1) ? act->midiValue2 : lastProgramChange[act->midiChannel] - 1;
              lastProgramChange[act->midiChannel] = constrain(lastProgramChange[act->midiChannel], act->midiValue1, act->midiValue2);
              midi_send(act->midiMessage, lastProgramChange[act->midiChannel], 0, act->midiChannel, true, act->midiValue1, act->midiValue2, currentBank, p, i);
              break;

            case PED_ACTION_BANK_PLUS:
              currentBank = constrain((currentBank == constrain(act->midiValue2, 0, BANKS - 1)) ? act->midiValue1 : (currentBank + 1), 0, BANKS - 1);
              currentBank = constrain(currentBank, constrain(act->midiValue1, 0, BANKS - 1), constrain(act->midiValue2, 0, BANKS - 1));
              currentBank = constrain(currentBank, 0, BANKS - 1);
              if (repeatOnBankSwitch)
                midi_send(lastMIDIMessage[currentBank].midiMessage,
                          lastMIDIMessage[currentBank].midiCode,
                          lastMIDIMessage[currentBank].midiValue,
                          lastMIDIMessage[currentBank].midiChannel,
                          true,
                          0, MIDI_RESOLUTION - 1,
                          currentBank, act->pedal, act->button);
              leds_refresh();
              break;

            case PED_ACTION_BANK_MINUS:
              currentBank = constrain((currentBank == act->midiValue1) ? act->midiValue2 : (currentBank - 1), 0, BANKS - 1);
              currentBank = constrain(currentBank, constrain(act->midiValue1, 0, BANKS - 1), constrain(act->midiValue2, 0, BANKS - 1));
              currentBank = constrain(currentBank, 0, BANKS - 1);
              if (repeatOnBankSwitch)
                midi_send(lastMIDIMessage[currentBank].midiMessage,
                          lastMIDIMessage[currentBank].midiCode,
                          lastMIDIMessage[currentBank].midiValue,
                          lastMIDIMessage[currentBank].midiChannel,
                          true,
                          0, MIDI_RESOLUTION - 1,
                          currentBank, act->pedal, act->button);
              leds_refresh();
              break;

            case PED_ACTION_START:
              mtc_start();
              break;

            case PED_ACTION_STOP:
              mtc_stop();
              break;

            case PED_ACTION_CONTINUE:
              mtc_continue();
              break;

            case PED_ACTION_TAP:
              mtc_tap();
              break;

            case PED_ACTION_BPM_PLUS:
              bpm = constrain(bpm + 1, 40, 300);
              MTC.setBpm(bpm);
              break;

            case PED_ACTION_BPM_MINUS:
              bpm = constrain(bpm - 1, 40, 300);
              MTC.setBpm(bpm);
              break;

            case PED_OSC_MESSAGE:
              if ((pedals[p].mode == PED_MOMENTARY1 ||
                   pedals[p].mode == PED_MOMENTARY2 ||
                   pedals[p].mode == PED_MOMENTARY3 ||
                   pedals[p].mode == PED_LADDER     ||
                   pedals[p].mode == PED_ANALOG_MOMENTARY) && (currentMIDIValue[currentBank][p][i] == act->midiValue1)) {
                if (act->midiValue1 == act->midiValue2)
                  OSCSendMessage(act->oscAddress);
                else
                  OSCSendMessage(act->oscAddress, act->midiValue2);
                DPRINT("OSC MESSAGE.....%s %d\n", act->oscAddress, act->midiValue2);
                currentMIDIValue[currentBank][p][i] = act->midiValue2;
                strncpy(lastPedalName, act->tag1, MAXACTIONNAME+1);
              }
              else {
                 if (act->midiValue1 == act->midiValue2)
                  OSCSendMessage(act->oscAddress);
                else
                  OSCSendMessage(act->oscAddress, act->midiValue1);
                DPRINT("OSC MESSAGE.....%s %d\n", act->oscAddress, act->midiValue1);
                currentMIDIValue[currentBank][p][i] = act->midiValue1;
                strncpy(lastPedalName, act->tag0, MAXACTIONNAME+1);
              }
              break;

            case PED_ACTION_PROFILE_PLUS:
              if (reloadProfile) return;
              eeprom_update_current_bank();
              currentProfile = (currentProfile == (PROFILES - 1) ? 0 : currentProfile + 1);
              reloadProfile = true;
              break;

            case PED_ACTION_PROFILE_MINUS:
              if (reloadProfile) return;
              eeprom_update_current_bank();
              currentProfile = (currentProfile == 0 ? PROFILES - 1 : currentProfile - 1);
              reloadProfile = true;
              break;

            case PED_ACTION_LED_COLOR:
              fastleds[led_button(act->pedal, act->button, act->led)] = act->color0;
              fastleds[led_button(act->pedal, act->button, act->led)] = swap_rgb_order(fastleds[led_button(act->pedal, act->button, act->led)], rgbOrder);
              FastLED.show();
              lastLedColor[currentBank][led_button(act->pedal, act->button, act->led)] = fastleds[led_button(act->pedal, act->button, act->led)];
            break;

            case PED_ACTION_DEVICE_INFO:
              scrollingMode = !scrollingMode;
              leds_refresh();
              break;

            case PED_ACTION_POWER_ON_OFF:
              display_off();
              leds_off();
              //esp_sleep_enable_ext1_wakeup(GPIO_SEL_0, ESP_EXT1_WAKEUP_ALL_LOW);
              esp_sleep_enable_ext0_wakeup((gpio_num_t)PIN_D(p), 0);
#ifdef BLE
              esp_bluedroid_disable();
              esp_bt_controller_disable();
#endif
#ifdef WIFI
              esp_wifi_stop();
#endif
              adc_power_off();
              delay(200);
              esp_deep_sleep_start();
              break;
          }
          if (act->midiMessage == PED_PROGRAM_CHANGE) {
            fastleds[led_button(act->pedal, act->button, act->led)] = act->color1;
            fastleds[led_button(act->pedal, act->button, act->led)].nscale8(ledsOnBrightness);
            fastleds[led_button(act->pedal, act->button, act->led)] = swap_rgb_order(fastleds[led_button(act->pedal, act->button, act->led)], rgbOrder);
            FastLED.show();
            lastLedColor[currentBank][led_button(act->pedal, act->button, act->led)] = fastleds[led_button(act->pedal, act->button, act->led)];
            lastColor0 = act->color0;
            lastColor1 = act->color1;
          }
          else if (act->midiMessage != PED_ACTION_LED_COLOR) {
            CRGB off = act->color0;
            CRGB on  = act->color1;
            off.nscale8(ledsOffBrightness);
            on.nscale8(ledsOnBrightness);
            if (act->midiValue1 == act->midiValue2) {
              fastleds[led_button(act->pedal, act->button, act->led)] = (fastleds[led_button(act->pedal, act->button, act->led)] == off) ? on : off;
            }
            else {
              fastleds[led_button(act->pedal, act->button, act->led)] = (currentMIDIValue[currentBank][p][i] == act->midiValue1) ? off : on;
            }
            DPRINT("LED COLOR.....Led %2d......RGB Color #%02x%02x%02x\n", led_button(act->pedal, act->button, act->led),
                                                                           fastleds[led_button(act->pedal, act->button, act->led)].red,
                                                                           fastleds[led_button(act->pedal, act->button, act->led)].green,
                                                                           fastleds[led_button(act->pedal, act->button, act->led)].blue);
            fastleds[led_button(act->pedal, act->button, act->led)] = swap_rgb_order(fastleds[led_button(act->pedal, act->button, act->led)], rgbOrder);
            FastLED.show();
            lastLedColor[currentBank][led_button(act->pedal, act->button, act->led)] = fastleds[led_button(act->pedal, act->button, act->led)];
            lastColor0 = act->color0;
            lastColor1 = act->color1;
          }
        }
        act = act->next;
        if (global && (currentBank != 0) && (act == nullptr)) {
          global = false;
          act = actions[currentBank];
        }
      }
}


void set_or_clear(ButtonConfig *config, ButtonConfig::FeatureFlagType f, bool flag = true)
{
  if (config == nullptr) return;

  flag ? config->setFeature(f) : config->clearFeature(f);
}

//
//  Create new MIDI controllers setup
//
void controller_setup()
{
  lastUsedSwitch = 0xFF;
  lastUsedPedal  = 0xFF;
  lastUsed       = 0xFF;
  lastSlot       = SLOTS;
  memset(lastPedalName, 0, MAXACTIONNAME+1);
  controller_delete();

  analogReadResolution(ADC_RESOLUTION_BITS);
  analogSetAttenuation(ADC_11db);

  DPRINT("Bank %2d\n", currentBank + 1);

  // Build new MIDI controllers setup
  for (byte i = 0; i < PEDALS; i++) {
    DPRINT("Pedal %2d     %4s   ", i + 1, pedalPressModeName[pedals[i].pressMode]);
    switch (pedals[i].mode) {
      case PED_MOMENTARY1:        DPRINT("MOMENTARY1      "); break;
      case PED_MOMENTARY2:        DPRINT("MOMENTARY2      "); break;
      case PED_MOMENTARY3:        DPRINT("MOMENTARY3      "); break;
      case PED_LATCH1:            DPRINT("LATCH1          "); break;
      case PED_LATCH2:            DPRINT("LATCH2          "); break;
      case PED_ANALOG:            DPRINT("ANALOG          "); break;
      case PED_JOG_WHEEL:         DPRINT("JOG_WHEEL       "); break;
      case PED_LADDER:            DPRINT("LADDER          "); break;
      case PED_ULTRASONIC:        DPRINT("ULTRASONIC      "); break;
      case PED_ANALOG_MOMENTARY:  DPRINT("ANALOG+MOMENTARY"); break;
      case PED_ANALOG_LATCH:      DPRINT("ANALOG+LATCH    "); break;
      default:                    DPRINT("                "); break;
    }
    DPRINT("   ");
    switch (pedals[i].pressMode) {
      case PED_PRESS_1:     DPRINT("PRESS_1    "); break;
      case PED_PRESS_2:     DPRINT("PRESS_2    "); break;
      case PED_PRESS_L:     DPRINT("PRESS_L    "); break;
      case PED_PRESS_1_2:   DPRINT("PRESS_1_2  "); break;
      case PED_PRESS_1_L:   DPRINT("PRESS_1_L  "); break;
      case PED_PRESS_1_2_L: DPRINT("PRESS_1_2_L"); break;
      case PED_PRESS_2_L:   DPRINT("PRESS_2_L  "); break;
      default:              DPRINT("           "); break;
    }
    DPRINT("   ");
    switch (pedals[i].invertPolarity) {
      case false:           DPRINT("POLARITY+"); break;
      case true:            DPRINT("POLARITY-"); break;
    }
    DPRINT("   ");
    switch (banks[currentBank][i].midiMessage) {
      case PED_PROGRAM_CHANGE:
        DPRINT("PROGRAM_CHANGE     %3d", banks[currentBank][i].midiCode);
        break;
      case PED_CONTROL_CHANGE:
        DPRINT("CONTROL_CHANGE     %3d", banks[currentBank][i].midiCode);
        break;
      case PED_NOTE_ON:
        DPRINT("NOTE_ON            %3d", banks[currentBank][i].midiCode);
        break;
      case PED_NOTE_OFF:
        DPRINT("NOTE_OFF           %3d", banks[currentBank][i].midiCode);
        break;
      case PED_BANK_SELECT_INC:
        DPRINT("BANK_SELECT_INC    %3d", banks[currentBank][i].midiCode);
        break;
      case PED_BANK_SELECT_DEC:
        DPRINT("BANK_SELECT_DEC    %3d", banks[currentBank][i].midiCode);
        break;
      case PED_PROGRAM_CHANGE_INC:
        DPRINT("PROGRAM_CHANGE_INC %3d", banks[currentBank][i].midiCode);
        break;
      case PED_PROGRAM_CHANGE_DEC:
        DPRINT("PROGRAM_CHANGE_DEC %3d", banks[currentBank][i].midiCode);
        break;
      case PED_PITCH_BEND:
        DPRINT("PITCH_BEND            ");
        break;
      case PED_CHANNEL_PRESSURE:
        DPRINT("CHANNEL PRESSURE      ");
        break;
      case PED_MIDI_START:
        DPRINT("MIDI_START            ");
        break;
      case PED_MIDI_STOP:
        DPRINT("MIDI_STOP             ");
        break;
      case PED_MIDI_CONTINUE:
        DPRINT("MIDI_CONTINUE         ");
        break;
      case PED_SEQUENCE:
        DPRINT("SEQUENCE           %3d", banks[currentBank][i].midiCode);
        break;
    }
    DPRINT("   Channel %2d", banks[currentBank][i].midiChannel);

    // Pedals setup
    switch (pedals[i].mode) {

      case PED_MOMENTARY1:
      case PED_LATCH1:

        pedals[i].buttonConfig = new ButtonConfig;
        set_or_clear(pedals[i].buttonConfig, ButtonConfig::kFeatureClick,       (pedals[i].pressMode & PED_PRESS_1) == PED_PRESS_1);
        set_or_clear(pedals[i].buttonConfig, ButtonConfig::kFeatureDoubleClick, (pedals[i].pressMode & PED_PRESS_2) == PED_PRESS_2);
        set_or_clear(pedals[i].buttonConfig, ButtonConfig::kFeatureLongPress,   (pedals[i].pressMode & PED_PRESS_L) == PED_PRESS_L);
        set_or_clear(pedals[i].buttonConfig, ButtonConfig::kFeatureRepeatPress, (pedals[i].pressMode & PED_PRESS_L) == PED_PRESS_L);
        pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureSuppressClickBeforeDoubleClick);
        //pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureSuppressAfterClick);
        //pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureSuppressAfterDoubleClick);
        pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureSuppressAfterLongPress);
        pedals[i].buttonConfig->setDebounceDelay(DEBOUNCE_INTERVAL);
        pedals[i].buttonConfig->setClickDelay(pressTime);
        pedals[i].buttonConfig->setDoubleClickDelay(doublePressTime);
        pedals[i].buttonConfig->setLongPressDelay(longPressTime);
        pedals[i].buttonConfig->setRepeatPressDelay(repeatPressTime);
        pedals[i].buttonConfig->setRepeatPressInterval(repeatPressTime);
        pedals[i].button[0] = new AceButton(pedals[i].buttonConfig, PIN_D(i), pedals[i].invertPolarity ? LOW : HIGH, (i + 1) * 10 + 1);
        pinMode(PIN_D(i), INPUT_PULLUP);
        pedals[i].button[0]->setEventHandler(controller_event_handler_button);
        DPRINT("   Pin D%d", PIN_D(i));
        break;

      case PED_MOMENTARY2:
      case PED_LATCH2:

        pedals[i].buttonConfig = new ButtonConfig;
        set_or_clear(pedals[i].buttonConfig, ButtonConfig::kFeatureClick,       (pedals[i].pressMode & PED_PRESS_1) == PED_PRESS_1);
        set_or_clear(pedals[i].buttonConfig, ButtonConfig::kFeatureDoubleClick, (pedals[i].pressMode & PED_PRESS_2) == PED_PRESS_2);
        set_or_clear(pedals[i].buttonConfig, ButtonConfig::kFeatureLongPress,   (pedals[i].pressMode & PED_PRESS_L) == PED_PRESS_L);
        set_or_clear(pedals[i].buttonConfig, ButtonConfig::kFeatureRepeatPress, (pedals[i].pressMode & PED_PRESS_L) == PED_PRESS_L);
        pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureSuppressClickBeforeDoubleClick);
        //pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureSuppressAfterClick);
        //pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureSuppressAfterDoubleClick);
        pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureSuppressAfterLongPress);
        pedals[i].buttonConfig->setDebounceDelay(DEBOUNCE_INTERVAL);
        pedals[i].buttonConfig->setClickDelay(pressTime);
        pedals[i].buttonConfig->setDoubleClickDelay(doublePressTime);
        pedals[i].buttonConfig->setLongPressDelay(longPressTime);
        pedals[i].buttonConfig->setRepeatPressDelay(repeatPressTime);
        pedals[i].buttonConfig->setRepeatPressInterval(repeatPressTime);
        pedals[i].button[0] = new AceButton(pedals[i].buttonConfig, PIN_D(i), pedals[i].invertPolarity ? LOW : HIGH, (i + 1) * 10 + 1);
        pinMode(PIN_D(i), INPUT_PULLUP);
        pedals[i].button[0]->setEventHandler(controller_event_handler_button);
        pedals[i].button[1] = new AceButton(pedals[i].buttonConfig, PIN_A(i), pedals[i].invertPolarity ? LOW : HIGH, (i + 1) * 10 + 2);
        pinMode(PIN_A(i), INPUT_PULLUP);
        pedals[i].button[1]->setEventHandler(controller_event_handler_button);
        DPRINT("   Pin A%d D%d", PIN_A(i), PIN_D(i));
        break;

      case PED_MOMENTARY3:

        pedals[i].buttonConfig = new Encoded4To2ButtonConfig(PIN_D(i), PIN_A(i), pedals[i].invertPolarity ? LOW : HIGH);
        set_or_clear(pedals[i].buttonConfig, ButtonConfig::kFeatureClick,       (pedals[i].pressMode & PED_PRESS_1) == PED_PRESS_1);
        set_or_clear(pedals[i].buttonConfig, ButtonConfig::kFeatureDoubleClick, (pedals[i].pressMode & PED_PRESS_2) == PED_PRESS_2);
        set_or_clear(pedals[i].buttonConfig, ButtonConfig::kFeatureLongPress,   (pedals[i].pressMode & PED_PRESS_L) == PED_PRESS_L);
        set_or_clear(pedals[i].buttonConfig, ButtonConfig::kFeatureRepeatPress, (pedals[i].pressMode & PED_PRESS_L) == PED_PRESS_L);
        pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureSuppressClickBeforeDoubleClick);
        //pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureSuppressAfterClick);
        //pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureSuppressAfterDoubleClick);
        pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureSuppressAfterLongPress);
        pedals[i].buttonConfig->setDebounceDelay(DEBOUNCE_INTERVAL);
        pedals[i].buttonConfig->setClickDelay(pressTime);
        pedals[i].buttonConfig->setDoubleClickDelay(doublePressTime);
        pedals[i].buttonConfig->setLongPressDelay(longPressTime);
        pedals[i].buttonConfig->setRepeatPressDelay(repeatPressTime);
        pedals[i].buttonConfig->setRepeatPressInterval(repeatPressTime);
        pedals[i].button[0] = new AceButton(pedals[i].buttonConfig, 1, pedals[i].invertPolarity ? LOW : HIGH, (i + 1) * 10 + 1);
        pedals[i].button[1] = new AceButton(pedals[i].buttonConfig, 2, pedals[i].invertPolarity ? LOW : HIGH, (i + 1) * 10 + 2);
        pedals[i].button[2] = new AceButton(pedals[i].buttonConfig, 3, pedals[i].invertPolarity ? LOW : HIGH, (i + 1) * 10 + 3);
        pinMode(PIN_D(i), INPUT_PULLUP);
        pinMode(PIN_A(i), INPUT_PULLUP);
        DPRINT("   Pin A%d D%d", PIN_A(i), PIN_D(i));
        pedals[i].buttonConfig->setEventHandler(controller_event_handler_button);
        break;

      case PED_ANALOG:

        pinMode(PIN_D(i), OUTPUT);
        digitalWrite(PIN_D(i), HIGH);
        pedals[i].analogPedal = new ResponsiveAnalogRead(PIN_A(i), true);
        pedals[i].analogPedal->setAnalogResolution(ADC_RESOLUTION);
        pedals[i].analogPedal->enableEdgeSnap();
        pedals[i].analogPedal->setSnapMultiplier(pedals[i].snapMultiplier);
        pedals[i].analogPedal->setActivityThreshold(pedals[i].activityThreshold);
        if (lastUsedPedal == 0xFF) {
          lastUsedPedal = i;
          lastUsed = i;
          lastSlot = SLOTS;
          strncpy(lastPedalName, banks[currentBank][i].pedalName, MAXACTIONNAME+1);
        }
        DPRINT("   Pin A%d D%d", PIN_A(i), PIN_D(i));
        break;

      case PED_LADDER:

        for (byte s = 0; s < LADDER_STEPS; s++) {
          pedals[i].button[s] = new AceButton(s, pedals[i].invertPolarity ? LOW : HIGH, (i + 1) * 10 + s + 1);
          assert(pedals[i].button[s] != nullptr);
         }
        pedals[i].buttonConfig = new LadderButtonConfig(PIN_A(i), LADDER_STEPS +  1, ladderLevels, LADDER_STEPS, pedals[i].button, pedals[i].invertPolarity ? LOW : HIGH);
        assert(pedals[i].buttonConfig != nullptr);
        set_or_clear(pedals[i].buttonConfig, ButtonConfig::kFeatureClick,       (pedals[i].pressMode & PED_PRESS_1) == PED_PRESS_1);
        set_or_clear(pedals[i].buttonConfig, ButtonConfig::kFeatureDoubleClick, (pedals[i].pressMode & PED_PRESS_2) == PED_PRESS_2);
        set_or_clear(pedals[i].buttonConfig, ButtonConfig::kFeatureLongPress,   (pedals[i].pressMode & PED_PRESS_L) == PED_PRESS_L);
        set_or_clear(pedals[i].buttonConfig, ButtonConfig::kFeatureRepeatPress, (pedals[i].pressMode & PED_PRESS_L) == PED_PRESS_L);
        pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureSuppressClickBeforeDoubleClick);
        //pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureSuppressAfterClick);
        //pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureSuppressAfterDoubleClick);
        pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureSuppressAfterLongPress);
        pedals[i].buttonConfig->setDebounceDelay(DEBOUNCE_INTERVAL);
        pedals[i].buttonConfig->setClickDelay(pressTime);
        pedals[i].buttonConfig->setDoubleClickDelay(doublePressTime);
        pedals[i].buttonConfig->setLongPressDelay(longPressTime);
        pedals[i].buttonConfig->setRepeatPressDelay(repeatPressTime);
        pedals[i].buttonConfig->setRepeatPressInterval(repeatPressTime);
        pinMode(PIN_D(i), OUTPUT);
        digitalWrite(PIN_D(i), HIGH);
        pinMode(PIN_A(i), INPUT_PULLUP);
        DPRINT("   Pin A%d D%d", PIN_A(i), PIN_D(i));
        pedals[i].buttonConfig->setEventHandler(controller_event_handler_button);
        break;

      case PED_JOG_WHEEL:
        pedals[i].jogwheel = new MD_REncoder(PIN_D(i), PIN_A(i));
        pedals[i].jogwheel->begin();
        pedals[i].jogwheel->setPeriod((11 - encoderSensitivity) * 100);   // From 1...10 to 1000..100
        DPRINT("   Pin A%d (CLK) D%d (DT)", PIN_A(i), PIN_D(i));
        break;

      case PED_ULTRASONIC:
        ultrasonicEcho.push_back(PIN_A(i));
        if (ultrasonicTrigger == 0) ultrasonicTrigger = PIN_D(i);
        pedals[i].analogPedal = new ResponsiveAnalogRead();
        pedals[i].analogPedal->setAnalogResolution(ADC_RESOLUTION);
        pedals[i].analogPedal->enableSleep();
        pedals[i].analogPedal->enableEdgeSnap();
        pedals[i].analogPedal->setSnapMultiplier(pedals[i].snapMultiplier);
        pedals[i].analogPedal->setActivityThreshold(pedals[i].activityThreshold);
        DPRINT("   Pin A%d (ECHO) D%d (TRIG)", PIN_A(i), PIN_D(i));
        break;

      case PED_ANALOG_MOMENTARY:
      case PED_ANALOG_LATCH:
        pedals[i].analogPedal = new ResponsiveAnalogRead(PIN_A(i), true);
        pedals[i].analogPedal->setAnalogResolution(ADC_RESOLUTION);
        pedals[i].analogPedal->enableEdgeSnap();
        pedals[i].analogPedal->setSnapMultiplier(pedals[i].snapMultiplier);
        pedals[i].analogPedal->setActivityThreshold(pedals[i].activityThreshold);
        if (lastUsedPedal == 0xFF) {
          lastUsedPedal = i;
          lastUsed = i;
          lastSlot = SLOTS;
          strncpy(lastPedalName, banks[currentBank][i].pedalName, MAXACTIONNAME+1);
        }
        pedals[i].buttonConfig = new ButtonConfig;
        set_or_clear(pedals[i].buttonConfig, ButtonConfig::kFeatureClick,       (pedals[i].pressMode & PED_PRESS_1) == PED_PRESS_1);
        set_or_clear(pedals[i].buttonConfig, ButtonConfig::kFeatureDoubleClick, (pedals[i].pressMode & PED_PRESS_2) == PED_PRESS_2);
        set_or_clear(pedals[i].buttonConfig, ButtonConfig::kFeatureLongPress,   (pedals[i].pressMode & PED_PRESS_L) == PED_PRESS_L);
        set_or_clear(pedals[i].buttonConfig, ButtonConfig::kFeatureRepeatPress, (pedals[i].pressMode & PED_PRESS_L) == PED_PRESS_L);
        pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureSuppressClickBeforeDoubleClick);
        //pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureSuppressAfterClick);
        //pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureSuppressAfterDoubleClick);
        pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureSuppressAfterLongPress);
        pedals[i].buttonConfig->setDebounceDelay(DEBOUNCE_INTERVAL);
        pedals[i].buttonConfig->setClickDelay(pressTime);
        pedals[i].buttonConfig->setDoubleClickDelay(doublePressTime);
        pedals[i].buttonConfig->setLongPressDelay(longPressTime);
        pedals[i].buttonConfig->setRepeatPressDelay(repeatPressTime);
        pedals[i].buttonConfig->setRepeatPressInterval(repeatPressTime);
        pedals[i].button[0] = new AceButton(pedals[i].buttonConfig, PIN_D(i), pedals[i].invertPolarity ? LOW : HIGH, (i + 1) * 10 + 1);
        pinMode(PIN_D(i), INPUT_PULLUP);
        pedals[i].button[0]->setEventHandler(controller_event_handler_button);
        DPRINT("   Pin A%d D%d", PIN_A(i), PIN_D(i));
        break;
    }
    pedals[i].pedalValue[0] = pedals[i].invertPolarity ? LOW : HIGH;
    pedals[i].lastUpdate[0] = millis();
    pedals[i].pedalValue[1] = pedals[i].pedalValue[0];
    pedals[i].lastUpdate[1] = pedals[i].lastUpdate[0];
    DPRINT("\n");
  }

  if (ultrasonicTrigger) HCSR04.begin(ultrasonicTrigger, ultrasonicEcho.data(), ultrasonicEcho.size());

  // Set initial led color for all banks
  lastColor0 = CRGB::Black;
  lastColor1 = CRGB::Black;
  for (byte b = 0; b < BANKS; b++)
    for (byte l = 0; l < LEDS; l++)
      lastLedColor[b][l] = CRGB::Black;

  for (byte b = 0; b < BANKS; b++) {
    bool ledstatus[LEDS];
    for (byte l = 0; l < LEDS; l++)
      ledstatus[l] = false;
    action *act = actions[b];
    while (act != nullptr) {
      act->tag0[MAXACTIONNAME-1] = 0;   // ensure string is null terminated
      act->tag1[MAXACTIONNAME-1] = 0;   // ensure string is null terminated
      if (!ledstatus[led_button(act->pedal, act->button, act->led)] ||
         (act->tag0[0] != 0 && act->tag0[strlen(act->tag0) - 1] == '.') ||
         (act->tag1[0] != 0 && act->tag1[strlen(act->tag1) - 1] == '.')) {
        ledstatus[led_button(act->pedal, act->button, act->led)] = true;
        if (act->tag1[0] != 0 && act->tag1[strlen(act->tag1) - 1] == '.') {
          lastLedColor[b][led_button(act->pedal, act->button, act->led)] = act->color1;
          lastLedColor[b][led_button(act->pedal, act->button, act->led)].nscale8(ledsOnBrightness);
        }
        else {
          lastLedColor[b][led_button(act->pedal, act->button, act->led)] = act->color0;
          lastLedColor[b][led_button(act->pedal, act->button, act->led)].nscale8(ledsOffBrightness);
        }
      }
      act = act->next;
    }
  }
  leds_refresh();

  for (int i = 0; i < 100; i++) {
    controller_run(false);            // to avoid spurious readings
  }
}