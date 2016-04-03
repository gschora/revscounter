/*
   This sketch provides a simple counter example for the HP Bubble display from SparkFun.
   https://www.sparkfun.com/products/12710

   Pinout for HP Bubble Display:
   1:  Cathode 1
   2:  Anode E
   3:  Anode C
   4:  Cathode 3
   5:  Anode dp
   6:  Cathode 4
   7:  Anode G
   8:  Anode D
   9:  Anode F
   10: Cathode 2
   11: Anode B
   12: Anode A
 */


#include "SevSeg.h"
#include <SerialCommand.h>

SerialCommand SCmd;

//Create an instance of the object.
SevSeg myDisplay;

const int revPin = 4;
unsigned long revTimeOld = 0;
unsigned long revTimeNew = 0;
unsigned long revTimeDiff = 0;
unsigned int revs = 0;
unsigned int divider = 1;
unsigned int precision = 1;

const int ledPin = 13; // blinking led pin
bool ledState = 0;
unsigned long previousMillis = 0;
const int blinkONInterval = 100;
const int blinkOFFInterval = 2000;

void setup()
{
        Serial.begin(115200);
        pinMode(ledPin, OUTPUT);
        pinMode(revPin, INPUT);
        attachInterrupt(revPin, countRevs, FALLING);

        SCmd.addDefaultHandler(unrecognized);
        SCmd.addCommand("setdivider",setDivider);
        SCmd.addCommand("setprecision",setPrecision);

        int displayType = COMMON_CATHODE; //Your display is either common cathode or common anode


        //This pinout is for a bubble dispaly
        //Declare what pins are connected to the GND pins (cathodes)
        int digit1 = 16; //Pin 1 #
        int digit2 = 15; //Pin 10  #
        int digit3 = 17; //Pin 4 #
        int digit4 = 10; //Pin 6 #

        //Declare what pins are connected to the segments (anodes)
        int segA = 19; //Pin 12  #
        int segB = 18; //Pin 11  #
        int segC = 9; //Pin 3  #
        int segD = 8; //Pin 8  #
        int segE = 14; //Pin 2 #
        int segF = 7; //Pin 9  #
        int segG = 11; //Pin 7 #
        int segDP= 12; //Pin 5 #

        int numberOfDigits = 4; //Do you have a 1, 2 or 4 digit display?

        myDisplay.Begin(displayType, numberOfDigits, digit1, digit2, digit3, digit4, segA, segB, segC, segD, segE, segF, segG, segDP);

        myDisplay.SetBrightness(100); //Set the display to 100% brightness level

}

void loop()
{
        blink();
        SCmd.readSerial();

        if((millis() - revTimeOld) > 1000) {
                revs = 0;
        }
        //Example ways of displaying a decimal number
        char tempString[10]; //Used for sprintf
        sprintf(tempString, "%4d", revs); //Convert deciSecond into a string that is right adjusted


        //Produce an output on the display
        myDisplay.DisplayString(tempString, 0); //(numberToDisplay, decimal point location)

        delay(5);
}

void blink(){
        unsigned long currentMillis = millis();

        if(ledState == 0) {
                if (currentMillis - previousMillis >= blinkOFFInterval) {
                        previousMillis = currentMillis;
                        ledState = 1;
                }
        } else {
                if(currentMillis - previousMillis >= blinkONInterval) {
                        previousMillis = currentMillis;
                        ledState = 0;
                }
        }

        // set the LED with the ledState of the variable:
        digitalWrite(ledPin, ledState);

}


void countRevs(){
        revTimeNew = millis();
        revTimeDiff = revTimeNew - revTimeOld;
        revs = 1000 * 60 / revTimeDiff / divider / precision * precision;
        revTimeOld = revTimeNew;
        Serial.println(revs);
        // Serial.println(revTimeDiff);
        // Serial.println();

}

void setDivider(){
        char *arg;
        arg = SCmd.next();
        if (arg != NULL) {
                divider=(float)atoi(arg); // Converts a char string to an integer
                if (divider < 1) {divider = 1; }
                Serial.print("setdivider ");
                Serial.print(divider);
                Serial.print(" ok");
                Serial.println();
        }
}

void setPrecision(){
        char *arg;
        arg = SCmd.next();
        if (arg != NULL) {
                precision=(float)atoi(arg); // Converts a char string to an integer

                if (precision < 10) {precision = 1; }
                else if (precision >= 10 && precision < 100) {precision = 10; }
                else if (precision >= 100 && precision < 1000) {precision = 100; }
                else if (precision >= 1000) {precision = 1000; }

                Serial.print("setprecision ");
                Serial.print(precision);
                Serial.print(" ok");
                Serial.println();
        }
}
void unrecognized()
{
        Serial.println("command not understood! try something else like");
        Serial.println("setdivider 1");
        Serial.println("setprecision 1|10|100|1000");
}
