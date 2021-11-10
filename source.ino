#include <MIDI.h>
#include <Keypad.h>

const byte ROWS = 9; // 9 Linhas - KO0~KO8 
const byte COLS = 6; // 6 Colunas - KI2~KI7
// more info about ROWS and COLS in Readme With Service Manual's Prints. 
char keys[COLS][ROWS] = {
  {36, 37, 38, 39, 40, 41}, // First Key = C2 (MIDI Number 36)
  {42, 43, 44, 45, 46, 47},
  {48, 49, 50, 51, 52, 53},
  {54, 55, 56, 57, 58, 59},
  {60, 61, 62, 63, 64, 65},
  {66, 67, 68, 69, 70, 71},
  {72, 73, 74, 75, 76, 77},
  {78, 79, 80, 81, 82, 83},
  {84, 85, 86, 87, 88, 89} // Last Key = C6 (MIDI Number 84)
}; 
// more info about keys in Readme With Service Manual's Prints. - Keys can be charged, changing MIDI Numbers.
byte rowPins[ROWS] = {22, 24, 26, 28, 30, 32, 34, 36, 38}; //connect to the row pinouts of the kpd - Arduino MEGA
byte colPins[COLS] = {A0, A1, A2, A3, A4, A5}; //connect to the column pinouts of the kpd

Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

byte pressed = 32;
byte chanel = 0; // MIDI channel to use

void setup() {
 Serial.begin(115200); // set this the same as Hairless
}


void loop() {
 // Fills kpd.key[49] array with up-to 10 active keys.
 // Returns true if there are ANY active keys.
 if (kpd.getKeys())
 {
   for (int i = 0; i < LIST_MAX; i++) // Scan the whole key list.
   {
     if ( kpd.key[i].stateChanged )   // Only find keys that have changed state.
     {
       pressed = kpd.key[i].kchar + 12;
       switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
         case PRESSED:
           sendMIDI(chanel | 0x90, pressed, 100);
           break;
          case RELEASED:
           sendMIDI(chanel | 0x80, pressed, 64);
           break;
       }
     }
   }
 }
}  // End loop

void sendMIDI(byte type, byte note, byte velocity){
  Serial.write(type);
  Serial.write(note & 0x7F);
  Serial.write(velocity);
}
