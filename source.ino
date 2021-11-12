#include <MIDI.h>
#include <Keypad.h>

const byte ROWS = 9; // 9 Linhas - KO0~KO8 
const byte COLS = 6; // 6 Colunas - KI2~KI7
// more info about ROWS and COLS in Readme With Service Manual's Prints. 
char keys[ROWS][COLS] = {
  {36, 44, 52, 60, 68, 76},
  {37, 45, 53, 61, 69, 77},
  {38, 46, 54, 62, 70, 78},
  {39, 47, 55, 63, 71, 79},
  {40, 48, 56, 64, 72, 80},
  {41, 49, 57, 65, 73, 81},
  {42, 50, 58, 66, 74, 82},
  {43, 51, 59, 67, 75, 83},
  {84, 00, 00, 00, 00, 00} 
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
