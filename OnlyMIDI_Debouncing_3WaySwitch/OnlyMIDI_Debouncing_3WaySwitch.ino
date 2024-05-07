#include <MIDI.h>
#include <Keypad.h>
#include <LiquidCrystal.h>

const byte ROWS = 9; // 9 Rows - KO0~KO8 
const byte COLS = 6; // 6 Collumns - KI2~KI7

char keys[ROWS][COLS] = {
  {36, 37, 38, 39, 40, 41},  
  {42, 43, 44, 45, 46, 47},
  {48, 49, 50, 51, 52, 53},
  {54, 55, 56, 57, 58, 59},
  {60, 61, 62, 63, 64, 65},
  {66, 67, 68, 69, 70, 71},
  {72, 73, 74, 75, 76, 77},
  {78, 79, 80, 81, 82, 83},
  {84, 00, 00, 00, 00, 00} // First key: C2, Last Key: C6
}; 

byte rowPins[ROWS] = {22, 24, 26, 28, 30, 32, 34, 36, 38}; //connect to the row pinouts of the kpd - Arduino MEGA
byte colPins[COLS] = {A0, A1, A2, A3, A4, A5}; //connect to the column pinouts of the kpd

const int octaveSwitchPin1 = A6;
const int octaveSwitchPin2 = A7;

Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
LiquidCrystal lcd(22, 23, 24, 25, 26, 27); // RS, E, D4, D5, D6, D7

byte pressed = 32;
byte chanel = 0; // MIDI channel to use

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

int octaveState = 1;
int lastOctaveState = 1;

void setup() {
  Serial.begin(115200); // set this the same as Hairless
  lcd.begin(16, 2); // initialize the LCD with 16 characters and 2 lines
}


void loop() {
  if (kpd.getKeys())
  {
    for (int i = 0; i < LIST_MAX; i++) 
    {
      if ( kpd.key[i].stateChanged )  
      {
        if ((millis() - lastDebounceTime) > debounceDelay) {
          pressed = kpd.key[i].kchar + 0;
          switch (kpd.key[i].kstate) { 
            case PRESSED:
              sendMIDI(chanel | 0x90, pressed, 100);
              break;
            case RELEASED:
              sendMIDI(chanel | 0x80, pressed, 64);
              break;
          }
          lastDebounceTime = millis();  
        }
      }
    }
  }
  
  int switch1State = digitalRead(octaveSwitchPin1);
  int switch2State = digitalRead(octaveSwitchPin2);
  octaveState = switch1State * 2 + switch2State;
  
  // Se o estado da oitava mudou, atualize o visor
  if (octaveState != lastOctaveState) {
    lcd.clear();
    lcd.print("Primeira Tecla: C");
    lcd.print(octaveState + 1); // Ajuste +1 porque octaveState começa de 0
    lcd.print("1");
    lcd.setCursor(0, 1);
    lcd.print("Ultima Tecla: C");
    lcd.print(octaveState + 5); // Ajuste +5 para a última oitava
    lastOctaveState = octaveState;
  }
}

void sendMIDI(byte type, byte note, byte velocity){
  Serial.write(type);
  Serial.write(note & 0x7F);
  Serial.write(velocity);
}
