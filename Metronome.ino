/* @file MetronomeCode.ino
|| @version 1.0
|| @author Adit Anup, Srihari Das, Shashank Raghuraj
||
|| @description
|| | Provides input/output functionality for a simple metronome. A numberpad is 
|| | used for input, an LED and an active buzzer are used as outputs. 
*/

#include <Keypad.h>
#include <LiquidCrystal.h>

/*
 * Constant pin locations
 */ 
LiquidCrystal lcd(4, 6, 10, 11, 12, 13); // initialize the library with the numbers of the interface pins
const int LED_PIN = 2;
const int BUZZER_PIN = 8;
const int PHOTORESISTOR_PIN = A0;

/*
 * Other constants
 */ 
const int BRIGHT_LED_OUTPUT = 255;
const int DIM_LED_OUTPUT = 2;

/*
 * Initial values for global variables. These must be globals as they are accessed and 
 * updated in infinite loops.
 */
int bpm = 0;
int bpmOld = 0;
char BPMValue[]="                          ";  //the string to print on the LCD
char buffer[3] = {'\0', '\0', '\0'}; // input buffer
int largeConstant = 1024; 

/*
 * Keypad initialization
 */
const byte ROWS = 4; //four rows on keyboard
const byte COLS = 4; //four columns on keyboard
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = 
{
  { '1','2','3','A'  },
  { '4','5','6','B'  },
  { '7','8','9','C'  },
  { '*','0','#','D'  }
};
byte rowPins[ROWS] = {21, 20, 19, 18}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {17, 16, 15, 14}; //connect to the column pinouts of the keypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

/*
 * int bpm - beats per minute value to output
 * int brightness - the output brightness of the LED. 
 *
 * Provides output for a particular
 * bpm value. Output is displayed through a blinking
 * LED (pin defined globally as ledPin) and
 * a buzzer (pin defined globally as buzzerPin). 
 */ 
void metronomeOutput(int bpm, int brightness){
  double timeBetweenClicks = ((60.0 / bpm) * 1000)/ 2; // convert bpm to ms between 'clicks'

  while(true){
    analogWrite(LED_PIN,brightness); 
    digitalWrite(BUZZER_PIN, HIGH);

    if(checkBreak(customKeypad.getKey())){
      digitalWrite(LED_PIN,LOW); // need to write low to LED and buzzer if breaking at this point
      digitalWrite(BUZZER_PIN, LOW);
      clearBuffer();
      break;
    }

    delay(timeBetweenClicks);

    digitalWrite(LED_PIN,LOW);
    digitalWrite(BUZZER_PIN, LOW);

    if(checkBreak(customKeypad.getKey())){
      clearBuffer();
      break;
    }

    delay(timeBetweenClicks);
  }
}

/*
 * char input - character detected from keypad
 *
 * Returns true if the detected character is "*", 
 * the symbol used to terminate metronome output.
 */ 
bool checkBreak(char input){
  return input == '*';
}

/*
 * int photoresistorReading - photoresistor reading 
 * 
 * Determines if a bright or dim light should be emitted. 
 */ 
int computeBrightness(int photoresistorReading){
  if(photoresistorReading > 60){
    return BRIGHT_LED_OUTPUT;
  } else {
    return DIM_LED_OUTPUT;
  }
}

/*
 * write a comment here
 */
void clearBuffer(){
  for (int i = 0; i < 3; i++) {
    buffer[i] = '\0'; // replace buffer with blank characters
  }
  bpmOld = bpm;
  bpm = 0;
}

/*
 * char key - the detected key from keyboad
 * 
 * Processes input based on three possible cases based on the value of key param.  
 * CASE 1: clearing buffer. 
 * CASE 2: submitting bpm to be output, writing the buffer content to the bpm variable.
 * CASE 3: reading key value into the buffer.
 * 
 */ 
int processInput(char key){
  if (key) {
    //CASE 1: clearing buffer
    if(key == 'C'){
      clearBuffer();
    }
    //CASE 2: submitting bpm to be output, writing the buffer content to the bpm variable
    else if(key == '#'){
      bpm = 0;

      int temp = buffer[0]; // buffer input is backwards, need to reverse.
      buffer[0] = buffer[2];
      buffer[2] = temp;

      for(int i = 0; i <= 2; i++){
        if (buffer[i]) {
          bpm = bpm * 10 + (buffer[i] - '0'); //convert buffer character to int, then update bpm
        }
      }
    }
    //CASE 3: reading key value into the buffer
    else {
      if(key != 'A' && key !='B' && key !='D' && key !='*'){ //ensure only integer input
        buffer[2] = buffer[1];
        buffer[1] = buffer[0];
        buffer[0] = key;
      }
    }
  }
}

/*
 * TODO
 */ 
int getLargeConstant(){
  if(largeConstant >1050){
    largeConstant = 1024;
  } else {
    largeConstant++;
  }
  return largeConstant;
}

/*
 * setup
 */ 
void setup(){
  Serial.begin(9600);
  lcd.begin(16, 2);  // set up the LCD's number of columns and rows: 
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
}

/*
 * loop
 */  
void loop(){
  int brightness = computeBrightness(analogRead(PHOTORESISTOR_PIN));
  char key = customKeypad.getKey();

  processInput(key);

  //checking if output needs to be updated
  if(bpm == bpmOld){
    lcd.setCursor(0,0);
    for(int i=2; i>= 0; i--){
      lcd.print(buffer[i]);
    }
  } else {
    bpmOld = bpm;
    metronomeOutput(bpm, brightness);
  }


}

