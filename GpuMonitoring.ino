//Mario's Ideas
//MAX7219 - using Led Control library to display 8x8   bitmap
#include <LedControl.h>

int DIN = 11;
int CS = 7;
int CLK   = 10;
int FORCE_DISCONNECT = 4;
int LAST_VALUE = 0;
int DISCONNECT_COUNTER = 100;
const uint64_t DEAD_FACE =  0x787cefff93937f3f;
byte PREVIOUS_VALUES[8] = {};

byte DIGITS[10][8] ={{
  0b11100000,
  0b10100000,
  0b10100000,
  0b10100000,
  0b11100000,
  0b00000000,
  0b00000000,
  0b00000000
},{
  0b00100000,
  0b01100000,
  0b10100000,
  0b00100000,
  0b00100000,
  0b00000000,
  0b00000000,
  0b00000000
},{
  0b11100000,
  0b00100000,
  0b11100000,
  0b10000000,
  0b11100000,
  0b00000000,
  0b00000000,
  0b00000000
},{
  0b11100000,
  0b00100000,
  0b11100000,
  0b00100000,
  0b11100000,
  0b00000000,
  0b00000000,
  0b00000000
},{
  0b10100000,
  0b10100000,
  0b11100000,
  0b00100000,
  0b00100000,
  0b00000000,
  0b00000000,
  0b00000000
},{
  0b11100000,
  0b10000000,
  0b11100000,
  0b00100000,
  0b11100000,
  0b00000000,
  0b00000000,
  0b00000000
},{
  0b11100000,
  0b10000000,
  0b11100000,
  0b10100000,
  0b11100000,
  0b00000000,
  0b00000000,
  0b00000000
},{
  0b11100000,
  0b00100000,
  0b01100000,
  0b00100000,
  0b00100000,
  0b00000000,
  0b00000000,
  0b00000000
},{
  0b11100000,
  0b10100000,
  0b11100000,
  0b10100000,
  0b11100000,
  0b00000000,
  0b00000000,
  0b00000000
},{
  0b11100000,
  0b10100000,
  0b11100000,
  0b00100000,
  0b00100000,
  0b00000000,
  0b00000000,
  0b00000000
}};

LedControl lc=LedControl(DIN, CLK, CS,0);

void  setup() {
  lc.shutdown(0,false);
  lc.setIntensity(0,0);
  lc.clearDisplay(0);
  Serial.begin(9600); //
  Serial.setTimeout(1000);
  pinMode(FORCE_DISCONNECT, INPUT);

};



int calculateByte(int first_index, int second_index, int index){
  return DIGITS[first_index][index] | (DIGITS[second_index][index] >> 4);
};

int readSerial(){
  if (Serial.available() > 0) {
    String incomingByte = Serial.readStringUntil('\n');
    DISCONNECT_COUNTER = 100;
    LAST_VALUE = incomingByte.toInt();
    updatePrevious(LAST_VALUE);
    return LAST_VALUE;
  }
  DISCONNECT_COUNTER--;
  return LAST_VALUE;

}

void displayDead(){
  for(int i =0; i<8; i++){
    byte row = (DEAD_FACE >> i * 8) & 0xFF;
    for (int j = 0; j < 8; j++) {
      lc.setLed(0, i, j, bitRead(row, j));
    }
  }
  
}

void updatePrevious(int target){
  int temp = PREVIOUS_VALUES[7];
  for(int i=0; i<=6; i++){
    PREVIOUS_VALUES[i] = PREVIOUS_VALUES[i+1];
  
  }
  PREVIOUS_VALUES[7] = target;


}

void updateNumbers(int target){
  int first_index = (target / 10) % 100;
  int second_index = (target % 10 % 100);
  for(int i=0;i<5;i++) {
      byte value = calculateByte(first_index, second_index, i);
      lc.setRow(0, i, value);  
  }
  
}

void updateGraph(){
  int previous = PREVIOUS_VALUES[7];
  lc.setLed(0, 5, 7, false);
  lc.setLed(0, 6, 7, true);
  lc.setLed(0, 7, 7, false);

  for(int i=6; i>=0; i--){
      int current = PREVIOUS_VALUES[i] - previous;
      switch(current){
        case -1:
          lc.setLed(0,5, i, false);
          lc.setLed(0,6, i, false);
          lc.setLed(0,7, i, true);
          break;
        case 0:
          lc.setLed(0,5, i, false);
          lc.setLed(0,6, i, true);
          lc.setLed(0,7, i, false);
          break;
        case 1:
          lc.setLed(0,5, i, true);
          lc.setLed(0,6, i, false);
          lc.setLed(0,7, i, false);
          break;
        default:
          lc.setLed(0,5, i, false);
          lc.setLed(0,6, i, false);
          lc.setLed(0,7, i, false);
      }
    }
  }

void loop(){
  int target = readSerial();
  int result = digitalRead(FORCE_DISCONNECT);
  if(DISCONNECT_COUNTER < 0 || result){
      if(target != 0){
        memset(PREVIOUS_VALUES, 0, sizeof PREVIOUS_VALUES);
        target = 0;
      }
      displayDead();
      return;
  }
  updateNumbers(target);
  updateGraph();

};




