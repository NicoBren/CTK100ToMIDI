#include <MIDI.h>

const int ROWS = 9;  // Number of rows in the matrix
const int COLS = 6;  // Number of columns in the matrix
const int MIDI_CHANNEL = 1;  // MIDI channel for sending data

// Mapping of keys to MIDI notes
const byte NOTE_MAP[ROWS][COLS] = {
  {36, 37, 38, 39, 40, 41},  // Notes of the first row
  {42, 43, 44, 45, 46, 47},  // Notes of the second row
  {48, 49, 50, 51, 52, 53},  // Notes of the third row
  {54, 55, 56, 57, 58, 59},  // Notes of the fourth row
  {60, 61, 62, 63, 64, 65},  // Notes of the fifth row
  {66, 67, 68, 69, 70, 71},  // Notes of the sixth row
  {72, 73, 74, 75, 76, 77},  // Notes of the seventh row
  {78, 79, 80, 81, 82, 83},  // Notes of the eighth row
  {84, 00, 00, 00, 00, 00}   // Last row, only the first column is used
};

// Pins of the matrix
const int rowPins[ROWS] = {2, 3, 4, 5, 6, 7, 8, 9, 10};
const int colPins[COLS] = {11, 12, 13, A0, A1, A2};

// Pins of the 3-way switch to control the octave
const int octaveSwitchPin1 = A3;
const int octaveSwitchPin2 = A4;

// Variable to store the state of the octave (0, 1, or 2)
int octaveState = 1; // By default, normal configuration (octave 1)

// Array to store the current state of the keys
bool keyState[ROWS][COLS];
unsigned long lastDebounceTime[ROWS][COLS] = {0};
unsigned long debounceDelay = 20;

MIDI_CREATE_DEFAULT_INSTANCE();  // Creation of the MIDI object

void setup() {
  MIDI.begin(MIDI_CHANNEL);  // Initialization of the MIDI object

  // Pin configuration
  for (int i = 0; i < ROWS; i++) {
    pinMode(rowPins[i], INPUT_PULLUP);
  }
  for (int i = 0; i < COLS; i++) {
    pinMode(colPins[i], OUTPUT);
    digitalWrite(colPins[i], HIGH);
  }
  pinMode(octaveSwitchPin1, INPUT_PULLUP);
  pinMode(octaveSwitchPin2, INPUT_PULLUP);
}

void loop() {
  // Read the current state of the keys
  readKeys();
  
  // Read the current state of the 3-way switch
  readOctaveSwitch();

  // Send MIDI events for pressed keys
  sendMIDI();
}

void readKeys() {
  for (int col = 0; col < COLS; col++) {
    // Activate the column
    digitalWrite(colPins[col], LOW);

    // Check the rows to see if any key is pressed
    for (int row = 0; row < ROWS; row++) {
      if (digitalRead(rowPins[row]) == LOW) {
        if ((millis() - lastDebounceTime[row][col]) > debounceDelay) {
          keyState[row][col] = true;
          lastDebounceTime[row][col] = millis();
        }
      } else {
        keyState[row][col] = false;
      }
    }

    // Deactivate the column
    digitalWrite(colPins[col], HIGH);
  }
}

void readOctaveSwitch() {
  // Read the state of the 3-way switch
  int switch1State = digitalRead(octaveSwitchPin1);
  int switch2State = digitalRead(octaveSwitchPin2);

  // Calculate the octave state
  octaveState = switch1State * 2 + switch2State;
}

void sendMIDI() {
  for (int row = 0; row < ROWS; row++) {
    for (int col = 0; col < COLS; col++) {
      // Check if the key is pressed and valid
      if (keyState[row][col] && NOTE_MAP[row][col] != 0) {
        // Adjust the MIDI note based on the octave state
        byte adjustedNote = NOTE_MAP[row][col] + (octaveState - 1) * 12;
        
        // Send a pressed MIDI note
        MIDI.sendNoteOn(adjustedNote, 127, MIDI_CHANNEL);
      } else {
        // Adjust the MIDI note based on the octave state
        byte adjustedNote = NOTE_MAP[row][col] + (octaveState - 1) * 12;
        
        // Send a release message for the released MIDI note
        MIDI.sendNoteOff(adjustedNote, 0, MIDI_CHANNEL);
      }
    }
  }
}
