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
#define KC2     32
#define KCS2    34
#define KD2     36
#define KDS2    38
#define KE2     40
#define KF2     42
#define KFS2    44
#define KG2     46
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

unsigned long startMillis;
const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;

// time stamp for the last time a tip was received
unsigned long lastTip = -5000;

int peaks = 0;

int LoopLength = 2;
int CurrLoop = 0;

int LoopPosition = 0;

int LoopNote;
int LoopDuration;

boolean playingNote = false;
int noteStartTime;

int Loops2[45][2] = { {E1_A1_C2_E2, 150}, {REST, 100}, {E1_A1_C2_E2, 150}, {REST, 100}, {E1_A1_C2_E2, 150}, {REST, 100}, {E1_A1_C2_E2, 150}, {REST, 100}, {E1_A1_C2_E2, 150}, {REST, 100}, {E1_A1_C2_E2, 150}, {REST, 100},
  {KE1, 125}, {KA1, 125}, {KC2, 125}, {KE2, 125},
  {F1_A1_C2_DS2, 150}, {REST, 100}, {F1_A1_C2_DS2, 150}, {REST, 100}, {F1_A1_C2_DS2, 150}, {REST, 100}, {F1_A1_C2_DS2, 150}, {REST, 100}, {F1_A1_C2_DS2, 150}, {REST, 100}, {F1_A1_C2_DS2, 150}, {REST, 100},
  {KF1, 125}, {KA1, 125}, {KC2, 125}, {KDS2, 125},
  {F1_A1_D2, 400}, {REST, 100},
  {D1_F1_C2, 150}, {REST, 100}, {D1_F1_A1, 150}, {REST, 100}, {D1_GS1_C2, 150}, {REST, 100}, {D1_GS1_C2, 500}, {C1_E1_A1, 1250},
  {A1_C2_E2_A2, 250}, {REST, 250}, {REST, 500}
};

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
  for (int i = 2; i <= 13; i++) {
    pinMode(i, OUTPUT);
  }
  for (int i = 24; i <= 46; i += 2) {
    pinMode(i, OUTPUT);
  }

  Serial.begin(9600);

}


void loop() { 
  startMillis = millis(); // Start of sample window

  sample = analogRead(0);

  Serial.println(sample);

  if (sample >= 678 && startMillis - lastTip >= 5000) {
    lastTip = millis();

    peaks++;
    Serial.print("thanks! ");
    Serial.println(peaks);

    if (CurrLoop < 5) {
      CurrLoop++;
      LoopPosition = 0;
    }
  }

  // no tip for 30 seconds
  else if (CurrLoop > 0 && sample < 678 && startMillis - lastTip >= 30000) {
    Serial.println("no tip? :(");
    CurrLoop--;
    lastTip = millis();
    LoopPosition = 0;
  }

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

  if (!playingNote) {
    LoopNote = Loops[CurrLoop][LoopPosition % LoopLength][0];
    LoopDuration = Loops[CurrLoop][LoopPosition % LoopLength][1];

    if (LoopNote == E1_A1_C2_E2) {
      digitalWrite(KE1, HIGH);
      digitalWrite(KA1, HIGH);
      digitalWrite(KC2, HIGH);
      digitalWrite(KE2, HIGH);

    }

    else if (LoopNote == F1_A1_C2_DS2) {
      digitalWrite(KF1, HIGH);
      digitalWrite(KA1, HIGH);
      digitalWrite(KC2, HIGH);
      digitalWrite(KDS2, HIGH);

    }

    else if (LoopNote == F1_A1_D2) {
      digitalWrite(KF1, HIGH);
      digitalWrite(KA1, HIGH);
      digitalWrite(KD2, HIGH);

    }

    else if (LoopNote == D1_F1_C2) {
      digitalWrite(KD1, HIGH);
      digitalWrite(KF1, HIGH);
      digitalWrite(KC2, HIGH);
    }

    else if (LoopNote == D1_F1_A1) {
      digitalWrite(KD1, HIGH);
      digitalWrite(KF1, HIGH);
      digitalWrite(KA1, HIGH);
    }

    else if (LoopNote == D1_GS1_C2) {
      digitalWrite(KD1, HIGH);
      digitalWrite(KFS1, HIGH);
      digitalWrite(KC2, HIGH);
    }

    else if (LoopNote == C1_E1_A1) {
      digitalWrite(KC1, HIGH);
      digitalWrite(KE1, HIGH);
      digitalWrite(KA1, HIGH);
    }

    else if (LoopNote == A1_C2_E2_A2) {
      digitalWrite(KA1, HIGH);
      digitalWrite(KC2, HIGH);
      digitalWrite(KE2, HIGH);
      digitalWrite(KA2, HIGH);
    }

    else {
      digitalWrite(LoopNote, HIGH);
    }

    noteStartTime = millis();
    LoopPosition++;
    playingNote = true;
  }

  else if (playingNote && millis() - noteStartTime >= LoopDuration) {
    playingNote = false;

    for (int i = 2; i <= 13; i++) {
      digitalWrite(i, LOW);
    }
    for (int i = 24; i <= 46; i += 2) {
      digitalWrite(i, LOW);
    }
  }

}




