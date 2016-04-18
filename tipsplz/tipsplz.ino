// !! Requires Arduino-MOS scheduler library
#include <MOS.h>
#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include <elapsedMillis.h>

/****************************************
Example Sound Level Sketch for the 
Adafruit Microphone Amplifier
****************************************/
#define KC1     2
#define KCS1    3
#define KD1     4
#define KDS1    5
#define KE1     6
#define KF1     7
#define KFS1    8
#define KG1     9
#define KGS1    10
#define KA1     11
#define KAS1    12
#define KB1     13
#define KC2     14
#define KCS2    15
#define KD2     16
#define KDS2    17
#define KE2     18
#define KF2     19
#define KFS2    20
#define KG2     21
#define KGS2    22
#define KA2     23
#define KAS2    24
#define KB2     25
#define KC3     26

#define E1_A1_C2_E2    27
#define F1_A1_C2_DS2   28
#define F1_A1_D2       29
#define D1_F1_C2       30
#define D1_F1_A1       31
#define D1_FS1_C2      32
#define C1_E1_A1       33
#define A1_C2_E2_A2    34
#define REST    -1

const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;

int peaks = 0; 

int looplength = 2;

int Loops [6][42][2] = { 
  // initial loop (no money :( )
  {{E1_A1_C2_E2, 250}, {REST, 1750}},

  // second loop (tiny bit of money)
  {{E1_A1_C2_E2, 200}, {REST, 50}},

  // third loop (little bit more money)
  {{E1_A1_C2_E2, 200}, {REST, 50}, {E1_A1_C2_E2, 200}, {REST, 50}, {E1_A1_C2_E2, 200}, {REST, 50}, {E1_A1_C2_E2, 200}, {REST, 50}, {E1_A1_C2_E2, 200}, {REST, 50}, {E1_A1_C2_E2, 200}, {REST, 50}, 
  {KE1, 125}, {KA1, 125}, {KC2, 125}, {KE2, 125}},

   // fourth loop (little bit more money)
  {{E1_A1_C2_E2, 200}, {REST, 50}, {E1_A1_C2_E2, 200}, {REST, 50}, {E1_A1_C2_E2, 200}, {REST, 50}, {E1_A1_C2_E2, 200}, {REST, 50}, {E1_A1_C2_E2, 200}, {REST, 50}, {E1_A1_C2_E2, 200}, {REST, 50}, 
  {KE1, 125}, {KA1, 125}, {KC2, 125}, {KE2, 125},
  {F1_A1_C2_DS2, 200}, {REST, 50}, {F1_A1_C2_DS2, 200}, {REST, 50}, {F1_A1_C2_DS2, 200}, {REST, 50}, {F1_A1_C2_DS2, 200}, {REST, 50}, {F1_A1_C2_DS2, 200}, {REST, 750}},

  // fifth loop (little bit more money)
  {{E1_A1_C2_E2, 200}, {REST, 50}, {E1_A1_C2_E2, 200}, {REST, 50}, {E1_A1_C2_E2, 200}, {REST, 50}, {E1_A1_C2_E2, 200}, {REST, 50}, {E1_A1_C2_E2, 200}, {REST, 50}, {E1_A1_C2_E2, 200}, {REST, 50}, 
  {KE1, 125}, {KA1, 125}, {KC2, 125}, {KE2, 125},
  {F1_A1_C2_DS2, 200}, {REST, 50}, {F1_A1_C2_DS2, 200}, {REST, 50}, {F1_A1_C2_DS2, 200}, {REST, 50}, {F1_A1_C2_DS2, 200}, {REST, 50}, {F1_A1_C2_DS2, 200}, {REST, 50}, {F1_A1_C2_DS2, 200}, {REST, 50},
  {KF1, 125}, {KA1, 125}, {KC2, 125}, {KDS2, 125}}, 
  
  // full loop (so much money)
  {{E1_A1_C2_E2, 200}, {REST, 50}, {E1_A1_C2_E2, 200}, {REST, 50}, {E1_A1_C2_E2, 200}, {REST, 50}, {E1_A1_C2_E2, 200}, {REST, 50}, {E1_A1_C2_E2, 200}, {REST, 50}, {E1_A1_C2_E2, 200}, {REST, 50}, 
  {KE1, 125}, {KA1, 125}, {KC2, 125}, {KE2, 125},
  {F1_A1_C2_DS2, 200}, {REST, 50}, {F1_A1_C2_DS2, 200}, {REST, 50}, {F1_A1_C2_DS2, 200}, {REST, 50}, {F1_A1_C2_DS2, 200}, {REST, 50}, {F1_A1_C2_DS2, 200}, {REST, 50}, {F1_A1_C2_DS2, 200}, {REST, 50},
  {KF1, 125}, {KA1, 125}, {KC2, 125}, {KDS2, 125},
  {F1_A1_D2, 500},
  {D1_F1_C2, 250}, {D1_F1_A1, 250}, {D1_FS1_C2, 200}, {REST, 50}, {D1_FS1_C2, 500}, {C1_E1_A1, 1250}, 
  {A1_C2_E2_A2, 250}, {REST, 250}, {REST, 500}}
};

void setup() 
{
   Serial.begin(9600);
}


void loop() 
{
   unsigned long startMillis= millis();  // Start of sample window
   unsigned int peakToPeak = 0;   // peak-to-peak level

   unsigned int signalMax = 0;
   unsigned int signalMin = 1024;

   // collect data for 50 mS
   while (millis() - startMillis < sampleWindow)
   {
      sample = analogRead(0);

      
      if(sample >= 678) {
        peaks++; 
        Serial.print("peak! ");
        Serial.println(peaks);
        Serial.println(sample);
      }
      
   }
}
