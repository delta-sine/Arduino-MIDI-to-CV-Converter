/*
Adapted from denkitribe's code, which can be found here:
http://code.google.com/p/denkitribe/source/browse/trunk/Arduino/MonotronCVGateTest/MonotronCVGateTest.pde

pin setup: - DAC 1  --->  Arduino +5V
           - DAC 2  --->  Arduino 10
           - DAC 3  --->  Arduino 13
           - DAC 4  --->  Arduino 11
           - DAC 5  --->  Gnd
           - DAC 6  --->  Not Connected
           - DAC 7  --->  Arduino Gnd
           - DAC 8  --->  CV Out

For information on why the pins are configured this way see the links below:

MCP4822 datasheet - http://ww1.microchip.com/downloads/en/devicedoc/21953a.pdf
SPI library - http://www.arduino.cc/playground/Code/Spi
*/

#include <MIDI.h>  // Add Midi Library
#include <SPI.h>

const int SS_PIN = 10; //SPI select slave pin
const int gatePin = 2;

#define LED 13    // Arduino Board LED is on Pin 13

//Create an instance of the library with default name, serial port and settings
MIDI_CREATE_DEFAULT_INSTANCE();

void setup() {
  pinMode (LED, OUTPUT); // Set Arduino board pin 13 to output
  MIDI.begin(MIDI_CHANNEL_OMNI); // Initialize the Midi Library.
  // OMNI sets it to listen to all channels.. MIDI.begin(2) would set it
  // to respond to notes on channel 2 only.
  MIDI.setHandleNoteOn(MyHandleNoteOn); // This is important!! This command
  // tells the Midi Library which function you want to call when a NOTE ON command
  // is received. In this case it's "MyHandleNoteOn".
  MIDI.setHandleNoteOff(MyHandleNoteOff); // This command tells the Midi Library
  // to call "MyHandleNoteOff" when a NOTE OFF command is received.

  pinMode(SS_PIN, OUTPUT);
  SPI.begin();
}


// MyHandleNoteON is the function that will be called by the Midi Library
// when a MIDI NOTE ON message is received.
// It will be passed bytes for Channel, Pitch, and Velocity
void MyHandleNoteOn(byte channel, byte pitch, byte velocity) {
  digitalWrite(LED,HIGH);  //Turn LED on
  sendNote(int(pitch));
}

// MyHandleNoteOFF is the function that will be called by the Midi Library
// when a MIDI NOTE OFF message is received.
// * A NOTE ON message with Velocity = 0 will be treated as a NOTE OFF message *
// It will be passed bytes for Channel, Pitch, and Velocity
void MyHandleNoteOff(byte channel, byte pitch, byte velocity) {
  digitalWrite(LED,LOW);  //Turn LED off
}

//Converts a Midi note into CV
void sendNote (int key) {
  int pitch = 0xa00l * key / 12; //divide by 12 to map to one octave

  digitalWrite(SS_PIN, LOW); //DAC is selected when LOW
  SPI.transfer(0x10 + (pitch >> 8));
  SPI.transfer(pitch & 0xff);
  digitalWrite(SS_PIN, HIGH); //Deselect DAC

  digitalWrite(gatePin, HIGH); //send gate signal
  delay(90);
  digitalWrite(gatePin, LOW); //stop sending gate signal
  delay(10);
}

void loop() { // Main loop
  MIDI.read(); // Continuously check if Midi data has been received.
}
