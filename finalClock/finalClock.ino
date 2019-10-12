#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <Time.h>

const int clockSetBtnPin = 8;
const int clockIncBtnPin = 9;

/* DISPLAY */
int pinCS = 10; // Attach CS to this pin, DIN to MOSI and CLK to SCK (cf http://arduino.cc/en/Reference/SPI )
int numberOfHorizontalDisplays = 2;
int numberOfVerticalDisplays = 1;

Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

/* CLOCK EDITOR */
int clockSetBtnPinState = 0;  
int clockIncBtnPinState= 0;
int setModePosition = 0; // 0 doing nothing, 1 changing hours, 2 changing minutes
int internalHour = 0;
int internalMinute = 0;
boolean lightsOn = true;
int pushDelay = 340;


/* -------  DIGITS -------  */
byte zero [] = { 
    0,0,0,0,
    0,1,1,1,
    0,1,0,1,
    0,1,0,1,
    0,1,0,1,
    0,1,0,1,
    0,1,1,1,
    0,0,0,0,
}; 

byte one [] = { 
    0,0,0,0,
    0,0,1,0,
    0,0,1,0,
    0,0,1,0,
    0,0,1,0,
    0,0,1,0,
    0,0,1,0,
    0,0,0,0,  
}; 

byte two [] = {
    0,0,0,0,
    0,1,1,1,
    0,0,0,1,
    0,0,0,1,
    0,1,1,1,
    0,1,0,0,
    0,1,1,1,
    0,0,0,0,  
}; 

byte three [] = { 
    0,0,0,0,
    0,1,1,1,
    0,0,0,1,
    0,0,1,1,
    0,0,0,1,
    0,0,0,1,
    0,1,1,1,
    0,0,0,0,  
}; 

byte four [] = { 
    0,0,0,0,
    0,1,0,1,
    0,1,0,1,
    0,1,1,1,
    0,0,0,1,
    0,0,0,1,
    0,0,0,1,
    0,0,0,0,  
};

byte five [] = { 
    0,0,0,0,
    0,1,1,1,
    0,1,0,0,
    0,1,1,1,
    0,0,0,1,
    0,0,0,1,
    0,1,1,1,
    0,0,0,0,  
}; 

byte six [] = { 
    0,0,0,0,
    0,1,1,1,
    0,1,0,0,
    0,1,1,1,
    0,1,0,1,
    0,1,0,1,
    0,1,1,1,
    0,0,0,0,  
}; 

byte seven [] = { 
    0,0,0,0,
    0,1,1,1,
    0,0,0,1,
    0,0,0,1,
    0,0,0,1,
    0,0,0,1,
    0,0,0,1,
    0,0,0,0,  
};

byte eight [] = { 
    0,0,0,0,
    0,1,1,1,
    0,1,0,1,
    0,1,1,1,
    0,1,0,1,
    0,1,0,1,
    0,1,1,1,
    0,0,0,0,  
}; 

byte nine [] = { 
    0,0,0,0,
    0,1,1,1,
    0,1,0,1,
    0,1,1,1,
    0,0,0,1,
    0,0,0,1,
    0,0,0,1,
    0,0,0,0,  
}; 

/* ------- DIGITS END -------  */


void drawNumberArray(byte number [], int offset) {
    for ( int y = 0; y < 8; y++ ) {
        for ( int x = 0; x < 4; x++ ) {
            if (number[x+(y*4)] && (offset+x) >= 0 ) {
                matrix.drawPixel(offset+x, y, HIGH);  
            }
        }    
    }
}


void drawNumber(byte number, int offset) {
    switch(number) {
        case 0:
            drawNumberArray(zero, offset);
            break;
        case 1:
            drawNumberArray(one, offset);
            break;
        case 2:
            drawNumberArray(two, offset);
            break;
        case 3:
            drawNumberArray(three, offset);
            break;
        case 4:
            drawNumberArray(four, offset);
            break;
        case 5:
            drawNumberArray(five, offset);
            break;
        case 6:
            drawNumberArray(six, offset);
            break;
        case 7:
            drawNumberArray(seven, offset);
            break;
        case 8:
            drawNumberArray(eight, offset);
            break;
        case 9:
            drawNumberArray(nine, offset);
            break;
    }
}

void drawDisplay(int a, int b, int c, int d) {
    drawNumber(a, -1);
    drawNumber(b, 3);
    drawNumber(c, 8);
    drawNumber(d, 12);
}

void drawTime(int hour, int minute) {
    drawNumber(hour/10, -1);
    drawNumber(hour%10, 3);
    drawNumber(minute/10, 8);
    drawNumber(minute%10, 12);
}


void setup() {
    matrix.setIntensity(0); // Use a value between 0 and 15 for brightness

    // Adjust to your own needs
    matrix.setPosition(0, 1, 0); // The first display is at <1, 0>
    matrix.setPosition(1, 0, 0); // The second display is at <0, 0>
    matrix.setRotation(0, 3);    // The first display is position upside down
    matrix.setRotation(1, 3);    // The same hold for the last display

    // put your setup code here, to run once:
    pinMode(clockSetBtnPinState, INPUT);
    pinMode(clockIncBtnPinState, INPUT);
}

void loop() {  
    clockIncBtnPinState = digitalRead(clockIncBtnPin);
    clockSetBtnPinState = digitalRead(clockSetBtnPin);

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
    }

    /* DRAW LOOP */

    matrix.fillScreen(LOW);

    if (lightsOn) {
        if (setModePosition == 1) {
            matrix.drawPixel(0, 7, HIGH);
        }
        if (setModePosition == 2) {
            matrix.drawPixel(8, 7, HIGH);
        }

        if (setModePosition == 0) {
            drawTime(hour(), minute());
        } else {
            drawTime(internalHour, internalMinute);
        }
    }
    
    matrix.write();

    delay(pushDelay);
}

