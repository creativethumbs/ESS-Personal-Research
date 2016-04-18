// !! Requires Arduino-MOS scheduler library
#include <MOS.h>
#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include <elapsedMillis.h>

// notes
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
#define KA2     24
#define KAS2    26
#define KB2     28
#define KC3     30

// chords
#define E1_A1_C2_E2    31
#define F1_A1_C2_DS2   32
#define F1_A1_D2       33
#define D1_F1_C2       34
#define D1_F1_A1       35
#define D1_FS1_C2      36
#define C1_E1_A1       37
#define A1_C2_E2_A2    38

#define REST    -1

unsigned long startMillis;  
const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;

// time stamp for the last time a tip was received
unsigned long lastTip;  

int peaks = 0;

int LoopLength = 2;
int CurrLoop = 0;

int LoopPosition = 0;

int LoopNote;
int LoopDuration;

int Loops [6][42][2] = {
  // initial loop (no money :( )
  {{E1_A1_C2_E2, 250}, {REST, 1750}},

  // second loop (tiny bit of money)
  {{E1_A1_C2_E2, 200}, {REST, 50}},

  // third loop (little bit more money)
  { {E1_A1_C2_E2, 200}, {REST, 50}, {E1_A1_C2_E2, 200}, {REST, 50}, {E1_A1_C2_E2, 200}, {REST, 50}, {E1_A1_C2_E2, 200}, {REST, 50}, {E1_A1_C2_E2, 200}, {REST, 50}, {E1_A1_C2_E2, 200}, {REST, 50},
    {KE1, 125}, {KA1, 125}, {KC2, 125}, {KE2, 125}
  },

  // fourth loop (little bit more money)
  { {E1_A1_C2_E2, 200}, {REST, 50}, {E1_A1_C2_E2, 200}, {REST, 50}, {E1_A1_C2_E2, 200}, {REST, 50}, {E1_A1_C2_E2, 200}, {REST, 50}, {E1_A1_C2_E2, 200}, {REST, 50}, {E1_A1_C2_E2, 200}, {REST, 50},
    {KE1, 125}, {KA1, 125}, {KC2, 125}, {KE2, 125},
    {F1_A1_C2_DS2, 200}, {REST, 50}, {F1_A1_C2_DS2, 200}, {REST, 50}, {F1_A1_C2_DS2, 200}, {REST, 50}, {F1_A1_C2_DS2, 200}, {REST, 50}, {F1_A1_C2_DS2, 200}, {REST, 750}
  },

  // fifth loop (little bit more money)
  { {E1_A1_C2_E2, 200}, {REST, 50}, {E1_A1_C2_E2, 200}, {REST, 50}, {E1_A1_C2_E2, 200}, {REST, 50}, {E1_A1_C2_E2, 200}, {REST, 50}, {E1_A1_C2_E2, 200}, {REST, 50}, {E1_A1_C2_E2, 200}, {REST, 50},
    {KE1, 125}, {KA1, 125}, {KC2, 125}, {KE2, 125},
    {F1_A1_C2_DS2, 200}, {REST, 50}, {F1_A1_C2_DS2, 200}, {REST, 50}, {F1_A1_C2_DS2, 200}, {REST, 50}, {F1_A1_C2_DS2, 200}, {REST, 50}, {F1_A1_C2_DS2, 200}, {REST, 50}, {F1_A1_C2_DS2, 200}, {REST, 50},
    {KF1, 125}, {KA1, 125}, {KC2, 125}, {KDS2, 125}
  },

  // full loop (so much money)
  { {E1_A1_C2_E2, 200}, {REST, 50}, {E1_A1_C2_E2, 200}, {REST, 50}, {E1_A1_C2_E2, 200}, {REST, 50}, {E1_A1_C2_E2, 200}, {REST, 50}, {E1_A1_C2_E2, 200}, {REST, 50}, {E1_A1_C2_E2, 200}, {REST, 50},
    {KE1, 125}, {KA1, 125}, {KC2, 125}, {KE2, 125},
    {F1_A1_C2_DS2, 200}, {REST, 50}, {F1_A1_C2_DS2, 200}, {REST, 50}, {F1_A1_C2_DS2, 200}, {REST, 50}, {F1_A1_C2_DS2, 200}, {REST, 50}, {F1_A1_C2_DS2, 200}, {REST, 50}, {F1_A1_C2_DS2, 200}, {REST, 50},
    {KF1, 125}, {KA1, 125}, {KC2, 125}, {KDS2, 125},
    {F1_A1_D2, 500},
    {D1_F1_C2, 250}, {D1_F1_A1, 250}, {D1_FS1_C2, 200}, {REST, 50}, {D1_FS1_C2, 500}, {C1_E1_A1, 1250},
    {A1_C2_E2_A2, 250}, {REST, 250}, {REST, 500}
  }
};

void setup() {
  /*
  for (int i = 2; i <= 22; i++) {
    pinMode(i, OUTPUT);
  }
  pinMode(24, OUTPUT);
  pinMode(26, OUTPUT);
  pinMode(28, OUTPUT);
  pinMode(30, OUTPUT);*/
  
  Serial.begin(9600);
}


void loop() {
  MOS_Call(MoneyListener);
  //MOS_Call(PlayLoop);
}

void PlayLoop(PTCB tcb) {
  MOS_Continue(tcb);
  while (1) {
    LoopNote = Loops[CurrLoop][LoopPosition % LoopLength][0];
    LoopDuration = Loops[CurrLoop][LoopPosition % LoopLength][1];

    // The track is 'muted' if the loop is playing a rest note
    if (LoopNote == REST) {
      MOS_Delay(tcb, LoopDuration);
    }

    else {
      if (LoopNote == E1_A1_C2_E2) {
        digitalWrite(KE1, HIGH);
        digitalWrite(KA1, HIGH);
        digitalWrite(KC2, HIGH);
        digitalWrite(KE2, HIGH);

        MOS_Delay(tcb, LoopDuration);

        digitalWrite(KE1, LOW);
        digitalWrite(KA1, LOW);
        digitalWrite(KC2, LOW);
        digitalWrite(KE2, LOW);
      }

      else if (LoopNote == F1_A1_C2_DS2) {
        digitalWrite(KF1, HIGH);
        digitalWrite(KA1, HIGH);
        digitalWrite(KC2, HIGH);
        digitalWrite(KDS2, HIGH);

        MOS_Delay(tcb, LoopDuration);

        digitalWrite(KF1, LOW);
        digitalWrite(KA1, LOW);
        digitalWrite(KC2, LOW);
        digitalWrite(KDS2, LOW);
      }

      else if (LoopNote == F1_A1_D2) {
        digitalWrite(KF1, HIGH);
        digitalWrite(KA1, HIGH); 
        digitalWrite(KD2, HIGH);

        MOS_Delay(tcb, LoopDuration);

        digitalWrite(KF1, LOW);
        digitalWrite(KA1, LOW); 
        digitalWrite(KD2, LOW);
      }

      else if (LoopNote == D1_F1_C2) {
        digitalWrite(KD1, HIGH);
        digitalWrite(KF1, HIGH); 
        digitalWrite(KC2, HIGH);

        MOS_Delay(tcb, LoopDuration);

        digitalWrite(KD1, LOW);
        digitalWrite(KF1, LOW); 
        digitalWrite(KC2, LOW);
      }

      else if (LoopNote == D1_F1_A1) {
        digitalWrite(KD1, HIGH);
        digitalWrite(KF1, HIGH); 
        digitalWrite(KA1, HIGH);

        MOS_Delay(tcb, LoopDuration);

        digitalWrite(KD1, LOW);
        digitalWrite(KF1, LOW); 
        digitalWrite(KA1, LOW);
      }

      else if (LoopNote == D1_FS1_C2) {
        digitalWrite(KD1, HIGH);
        digitalWrite(KFS1, HIGH); 
        digitalWrite(KC2, HIGH);

        MOS_Delay(tcb, LoopDuration);

        digitalWrite(KD1, LOW);
        digitalWrite(KFS1, LOW); 
        digitalWrite(KC2, LOW);
      }

      else if (LoopNote == C1_E1_A1) {
        digitalWrite(KC1, HIGH);
        digitalWrite(KE1, HIGH); 
        digitalWrite(KA1, HIGH);

        MOS_Delay(tcb, LoopDuration);

        digitalWrite(KC1, LOW);
        digitalWrite(KE1, LOW); 
        digitalWrite(KA1, LOW);
      }

      else if (LoopNote == A1_C2_E2_A2) {
        digitalWrite(KA1, HIGH);
        MOS_Delay(tcb, 20);
        digitalWrite(KC2, HIGH); 
        MOS_Delay(tcb, 20);
        digitalWrite(KE2, HIGH);
        MOS_Delay(tcb, 20);
        digitalWrite(KA2, HIGH); 

        MOS_Delay(tcb, LoopDuration-60);

        digitalWrite(KA1, LOW);
        digitalWrite(KC2, LOW); 
        digitalWrite(KE2, LOW);
        digitalWrite(KA2, LOW);
      }

      else {
        digitalWrite(LoopNote, HIGH);
        MOS_Delay(tcb, LoopDuration);
        digitalWrite(LoopNote, LOW);
      }
    }

    LoopPosition++;
  }

}

// listens for money
void MoneyListener(PTCB tcb) {
  MOS_Continue(tcb);
  while (1) {

    startMillis = millis(); // Start of sample window

    // collect data for 50 mS
    while (millis() - startMillis < sampleWindow) {
      sample = analogRead(0); 
      
      if (CurrLoop < 5 && sample >= 678) {
        lastTip = millis(); 
        CurrLoop++; 
        
        peaks++;
        Serial.print("peak! ");
        Serial.println(peaks);
        Serial.println(sample);

        // wait for 5 seconds before listening for the next tip
        MOS_Delay(tcb, 5000);
      }

      // no tip for 30 seconds
      else if(CurrLoop > 0 && sample < 678 && startMillis-lastTip >= 30000) {
        Serial.println("no tip? :(");
        CurrLoop--; 
        lastTip = millis(); 
      }

    }
  }

}



