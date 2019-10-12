#include <Time.h>

const int clockSetBtnPin = 8;
const int clockIncBtnPin = 9;


int clockSetBtnPinState = 0;  
int clockIncBtnPinState= 0;
int setModePosition = 0; // 0 doing nothing, 1 changing hours, 2 changing minutes
int internalHour = 0;
int internalMinute = 0;
boolean lightsOn = true;
int pushDelay = 350;

void setup() {
//Initialize serial and wait for port to open:
    Serial.begin(9600); 
    while (!Serial) {
        // wait for serial port to connect. Needed for native USB
    }
    
    // put your setup code here, to run once:
    pinMode(clockSetBtnPinState, INPUT);
    pinMode(clockIncBtnPinState, INPUT);
}

void loop() {  
    clockIncBtnPinState = digitalRead(clockIncBtnPin);
    clockSetBtnPinState = digitalRead(clockSetBtnPin);

    Serial.print("MODE ");
    Serial.print(setModePosition);
    Serial.print(" LIGHTS ");
    Serial.print(lightsOn);
    Serial.print(" BTN ");
    Serial.print(clockSetBtnPinState);
    Serial.print(clockIncBtnPinState);
    Serial.print(" TIME ");
    Serial.print(internalHour);
    Serial.print(":");
    Serial.print(internalMinute);

    Serial.print(" CLOCK ");
    Serial.print(hour());
    Serial.print(":");
    Serial.print(minute());
    Serial.print(":");
    Serial.print(second()); 
    Serial.println("");


    if (clockIncBtnPinState == HIGH) {
        if (setModePosition == 0) {
            lightsOn = !lightsOn;
        }
        else if (setModePosition == 1) {
            internalHour = (internalHour + 1) % 24;
        }
        else if (setModePosition == 2) {
            internalMinute = (internalMinute +1) % 60;
        }
        delay(pushDelay);
    }

    if (clockSetBtnPinState == HIGH && lightsOn) {
        //allow changing time only when display is light on
        setModePosition = (setModePosition + 1) % 3; // 0, 1 or 2
        if (setModePosition == 0) {
            // ended setting time , update internal clock, for the final time
            setTime(internalHour, internalMinute, 0, 0, 0, 0); 
        }
        else if (setModePosition == 1) {
            // update initial values for editor
            internalHour = hour();
            internalMinute = minute();      
        }
        delay(pushDelay);
    }

}
