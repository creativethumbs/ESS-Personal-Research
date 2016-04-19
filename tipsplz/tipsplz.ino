// !! Requires Arduino-MOS scheduler library
//#include <MOS.h>
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
#define E1_A1_C2_E2    51
#define F1_A1_C2_DS2   52
#define F1_A1_D2       53
#define D1_F1_C2       54
#define D1_F1_A1       55
#define D1_GS1_C2      56
#define C1_E1_A1       57
#define A1_C2_E2_A2    58

#define REST    -1

// note to self: always use unsigned long w/ millis or else time would become negative....
unsigned long startMillis;
unsigned long noteStartTime;

unsigned int sample;

// time stamp for the last time a tip was received
unsigned long lastTip = -5000;

int peaks = 0;

int LoopLength = 2;
int CurrLoop = 0;
//int LoopLength = 45;
//int CurrLoop = 5;

int LoopPosition = 0;

int CurrNote;
unsigned int NoteDuration;

boolean playingNote = false;

int Loops [6][45][2] = {
  // initial loop (no money :( )
  {{E1_A1_C2_E2, 250}, {REST, 1750}},

  // second loop (tiny bit of money)
  {{E1_A1_C2_E2, 150}, {REST, 100}},

  // third loop (little bit more money)
  { {E1_A1_C2_E2, 150}, {REST, 100}, {E1_A1_C2_E2, 150}, {REST, 100}, {E1_A1_C2_E2, 150}, {REST, 100}, {E1_A1_C2_E2, 150}, {REST, 100}, {E1_A1_C2_E2, 150}, {REST, 100}, {E1_A1_C2_E2, 150}, {REST, 100},
    {KE1, 125}, {KA1, 125}, {KC2, 125}, {KE2, 125}
  },

  // fourth loop (little bit more money)
  { {E1_A1_C2_E2, 150}, {REST, 100}, {E1_A1_C2_E2, 150}, {REST, 100}, {E1_A1_C2_E2, 150}, {REST, 100}, {E1_A1_C2_E2, 150}, {REST, 100}, {E1_A1_C2_E2, 150}, {REST, 100}, {E1_A1_C2_E2, 150}, {REST, 100},
    {KE1, 125}, {KA1, 125}, {KC2, 125}, {KE2, 125},
    {F1_A1_C2_DS2, 150}, {REST, 100}, {F1_A1_C2_DS2, 150}, {REST, 100}, {F1_A1_C2_DS2, 150}, {REST, 100}, {F1_A1_C2_DS2, 150}, {REST, 100}, {F1_A1_C2_DS2, 150}, {REST, 750}
  },

  // fifth loop (little bit more money)
  { {E1_A1_C2_E2, 150}, {REST, 100}, {E1_A1_C2_E2, 150}, {REST, 100}, {E1_A1_C2_E2, 150}, {REST, 100}, {E1_A1_C2_E2, 150}, {REST, 100}, {E1_A1_C2_E2, 150}, {REST, 100}, {E1_A1_C2_E2, 150}, {REST, 100},
    {KE1, 125}, {KA1, 125}, {KC2, 125}, {KE2, 125},
    {F1_A1_C2_DS2, 150}, {REST, 100}, {F1_A1_C2_DS2, 150}, {REST, 100}, {F1_A1_C2_DS2, 150}, {REST, 100}, {F1_A1_C2_DS2, 150}, {REST, 100}, {F1_A1_C2_DS2, 150}, {REST, 100}, {F1_A1_C2_DS2, 150}, {REST, 100},
    {KF1, 125}, {KA1, 125}, {KC2, 125}, {KDS2, 125}
  },

  // full loop (so much money)
  { {E1_A1_C2_E2, 150}, {REST, 100}, {E1_A1_C2_E2, 150}, {REST, 100}, {E1_A1_C2_E2, 150}, {REST, 100}, {E1_A1_C2_E2, 150}, {REST, 100}, {E1_A1_C2_E2, 150}, {REST, 100}, {E1_A1_C2_E2, 150}, {REST, 100},
    {KE1, 125}, {KA1, 125}, {KC2, 125}, {KE2, 125},
    {F1_A1_C2_DS2, 150}, {REST, 100}, {F1_A1_C2_DS2, 150}, {REST, 100}, {F1_A1_C2_DS2, 150}, {REST, 100}, {F1_A1_C2_DS2, 150}, {REST, 100}, {F1_A1_C2_DS2, 150}, {REST, 100}, {F1_A1_C2_DS2, 150}, {REST, 100},
    {KF1, 125}, {KA1, 125}, {KC2, 125}, {KDS2, 125},
    {F1_A1_D2, 400}, {REST, 100},
    {D1_F1_C2, 150}, {REST, 100}, {D1_F1_A1, 150}, {REST, 100}, {D1_GS1_C2, 150}, {REST, 100}, {D1_GS1_C2, 500}, {C1_E1_A1, 1250},
    {A1_C2_E2_A2, 250}, {REST, 250}, {REST, 500}
  }
};

void setup() {
  for (int i = 2; i < 22; i++) {
    pinMode(i, OUTPUT);
  }

  for (int j = 22; j <= 30; j += 2) {
    pinMode(j, OUTPUT);
  }

  Serial.begin(9600);

  /*// Testing keys
    for (int i = 2; i < 22; i++) {
      digitalWrite(i, HIGH);
      delay(1000);
      digitalWrite(i, LOW);
    }

    for (int j = 22; j <= 30; j += 2) {
      digitalWrite(j, HIGH);
      delay(1000);
      digitalWrite(j, LOW);
    }
  */
}


void loop() {
    startMillis = millis(); // Start of sample window

    sample = analogRead(0);

    // wait 5 seconds between tips
    if (sample >= 678 && startMillis - lastTip >= 5000) {
    lastTip = millis();

    peaks++;
    Serial.print("thanks! ");
    Serial.println(peaks);

    if (CurrLoop < 5) {
      CurrLoop++;
      LoopPosition = 0;
      Serial.println(CurrLoop);
      playingNote = false;

      switch (CurrLoop) {
        case 0:
        case 1:
          LoopLength = 2;
          break;
        case 2:
          LoopLength = 16;
          break;
        case 3:
          LoopLength = 26;
          break;
        case 4:
          LoopLength = 32;
          break;
        case 5:
          LoopLength = 45;
          break;
      }
    }
    }

    // no tip for 30 seconds causes piano to fall back to previous loop

    else if (CurrLoop > 0 && sample < 678 && startMillis - lastTip >= 30000) {
    Serial.println("no tip? :(");
    CurrLoop--;
    lastTip = millis();
    LoopPosition = 0;

    switch (CurrLoop) {
      case 0:
      case 1:
        LoopLength = 2;
        break;
      case 2:
        LoopLength = 16;
        break;
      case 3:
        LoopLength = 26;
        break;
      case 4:
        LoopLength = 32;
        break;
      case 5:
        LoopLength = 45;
        break;
    }
    }

    if (!playingNote) {
    CurrNote = Loops[CurrLoop][LoopPosition % LoopLength][0];
    NoteDuration = Loops[CurrLoop][LoopPosition % LoopLength][1];


    if (CurrNote == E1_A1_C2_E2) {
      digitalWrite(KE1, HIGH);
      digitalWrite(KA1, HIGH);
      digitalWrite(KC2, HIGH);
      digitalWrite(KE2, HIGH);

    }

    else if (CurrNote == F1_A1_C2_DS2) {
      digitalWrite(KF1, HIGH);
      digitalWrite(KA1, HIGH);
      digitalWrite(KC2, HIGH);
      digitalWrite(KDS2, HIGH);

    }

    else if (CurrNote == F1_A1_D2) {
      digitalWrite(KF1, HIGH);
      digitalWrite(KA1, HIGH);
      digitalWrite(KD2, HIGH);

    }

    else if (CurrNote == D1_F1_C2) {
      digitalWrite(KD1, HIGH);
      digitalWrite(KF1, HIGH);
      digitalWrite(KC2, HIGH);
    }

    else if (CurrNote == D1_F1_A1) {
      digitalWrite(KD1, HIGH);
      digitalWrite(KF1, HIGH);
      digitalWrite(KA1, HIGH);
    }

    else if (CurrNote == D1_GS1_C2) {
      digitalWrite(KD1, HIGH);
      digitalWrite(KFS1, HIGH);
      digitalWrite(KC2, HIGH);
    }

    else if (CurrNote == C1_E1_A1) {
      digitalWrite(KC1, HIGH);
      digitalWrite(KE1, HIGH);
      digitalWrite(KA1, HIGH);
    }

    else if (CurrNote == A1_C2_E2_A2) {
      digitalWrite(KA1, HIGH);
      digitalWrite(KC2, HIGH);
      digitalWrite(KE2, HIGH);
      digitalWrite(KA2, HIGH);
    }

    else {
      digitalWrite(CurrNote, HIGH);
    }

    noteStartTime = startMillis;
    LoopPosition++;
    playingNote = true;

    }

    else if (playingNote && startMillis - noteStartTime >= NoteDuration) {
    playingNote = false;

    for (int i = 2; i < 22; i++) {
      digitalWrite(i, LOW);
    }
    for (int i = 22; i <= 30; i += 2) {
      digitalWrite(i, LOW);
    }
    }
  
}




