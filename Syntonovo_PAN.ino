//#define SIMLINK
//#define SILENTMODE

#ifdef SILENTMODE
#ifdef  SIMLINK
#undef SIMLINK
#endif
#endif
#include "PanSim/PAN_TeensyUI.cpp"
#include "PanSim/PanStructs.cpp"
#include "PanSim/PAN_Raspberry_Interface.cpp"

void WriteKnob(int id, uint32_t value)
{
  char b[4];
  b[0] = (id >> 0) & 0xFF;
  b[1] = (id >> 8) & 0xFF;
  b[2] = (value >> 8) & 0xFF;
  b[3] = (value >> 0) & 0xFF;
  Serial4.write(b, 4);
}

void WriteWithSubKnob(int id, int subid, uint32_t value)
{
  id |= subid << 8;
  char b[4];
  b[0] = (id >> 0) & 0xFF;
  b[1] = (id >> 8) & 0xFF;
  b[2] = (value >> 8) & 0xFF;
  b[3] = (value >> 0) & 0xFF;

  Serial4.write(b, 4);
}



typedef struct setpara_t
{
  uint16_t paramid;
  uint16_t value;
} setpara_t;

void doset(struct setpara_t& para)
{
  char b[4];

  //b[0] = 1;
  b[0] = (para.paramid >> 0) & 0xFF;
  b[1] = (para.paramid >> 8) & 0xFF;
  b[2] = (para.value >> 8) & 0xFF;
  b[3] = (para.value >> 0) & 0xFF;

  Serial4.write(b, 4);
}


void WriteSwitch(int id, int state)
{
  setpara_t sp;
  sp.paramid = 0xfdfe;
  sp.value = id | ((state > 0) ? 0x200 : 0x100);
  doset(sp);
}

void WriteSyncLfo(uint8_t* paramids)
{
  //TODO
}

void note_on(int noteid, int notevel)
{
  setpara_t setpara;
  setpara.paramid = 0x02fc;
  setpara.value = noteid | (notevel << 8);
  doset(setpara);
}

void note_off(int noteid, int notevel)
{
  setpara_t setpara;
  setpara.paramid = 0x01fc;
  setpara.value = noteid | (notevel << 8);
  doset(setpara);
}

#define ENC1A 25
#define ENC1B 26

#define ENC2A 27
#define ENC2B 28


#define OUTPUT 1
#define LEDDAT 36
#define LEDCLK 37
#define LEDLATCH 38


#define BUTTONDAT   29
#define BUTTONCLK   30
#define BUTTONLATCH 35

bool RaspberryUpdate = true;

#define POTSADC A22

#define POTCOUNT 64



unsigned short Pots[POTCOUNT];
unsigned short PotStable[POTCOUNT] = {0};
unsigned short PotAvg[POTCOUNT];
unsigned short LastPotAvg[POTCOUNT] = {0};


#define RANGEBITS 10


int32_t iabs(int32_t x)
{
  if (x < 0) return -x;
  return x;
}

struct smooth_state_t
{
  int32_t g;
  int32_t low1z;
  int32_t low2z;
  int32_t bandz;
};
smooth_state_t PotSmooth[POTCOUNT];

void smooth_init(struct smooth_state_t* s, int g0)
{
  // g = 2-2/*(1+tan(pi*wc))
  s->g = g0;
  s->low1z = 0;
  s->low2z = 0;
  s->bandz = 0;
}

int smooth(struct smooth_state_t* s, int in)
{
  int bandz = s->low2z - s->low1z;
  int g = s->g + iabs(bandz);
  if (g >= 1<<RANGEBITS) g = (1<<RANGEBITS) - 1;

  int low1 = s->low1z + ((g*(in - s->low1z)) >> RANGEBITS);
  int low2 = s->low2z + ((g*(low1 - s->low2z)) >> RANGEBITS);
  s->low1z = low1;
  s->low2z = low2;

  return low1;
}


/*
  38 31 5
  24 16 4
  20 42 3
  21 43 C
  22 44 B
  23 45 A
*/

const int POTPIN[6] = {39, 24, 20, 21, 22, 23};

#define LEDCOUNT 104
#define BUTTONCOUNT 104

unsigned char LEDs[LEDCOUNT];

unsigned char LEDTarget[LEDCOUNT];


unsigned char HWButtons[BUTTONCOUNT];
unsigned char LastButtons[BUTTONCOUNT];
unsigned int ButtonTarget[BUTTONCOUNT];

IntervalTimer KeyboardTimer;
int KB[20] = {0, 1, 2, 3, 4, 5, 6, 19, 7, 18, 8, 17, 9, 16, 10, 15, 11, 14, 12, 13};
int KeyboardMKBK[12] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 19, 17};
int KeyboardSink[8] =  {1, 3, 5, 7, 9, 11, 13, 15};


int Keyboard[128] = {0};
int Velocity[128] = {0};
int KeyboardTime[128] = {0};


int StateNew[128];
int StateNewStage[128];
int StateTime[128];
int State[128];

enum
{
  STATE_IDLE = 0,
  STATE_MK =  1,
  STATE_BRK = 2,
  STATE_MKBRK = 3
};
int NoteT = 0;
void MK(int base, int mkbkid, int val)
{
  if (val)
  {
    if (0)
    {
      Serial.print(NoteT);
      Serial.print(" MK ");
      Serial.print(base);
      Serial.print(" ");
      Serial.println( mkbkid);
    }
    StateNewStage[base + mkbkid] += STATE_MK;
  }
}

void BK(int base, int mkbkid, int val)
{
  if (val)
  {
    if (0)
    { Serial.print(NoteT);

      Serial.print(" BK ");
      Serial.print(base);
      Serial.print(" ");
      Serial.println( mkbkid);
    }
    StateNewStage[base + mkbkid] += STATE_BRK;
  }
}

int ConvertVelocity(int i)
{
  if (i < 50) return 127;
  if (i > 2000) return 1;

  return 127 - (((i - 50) * 127) / (2000 - 50));
}
void DoNoteOn(int n, int v)
{
  note_on(n, v);

}

void DoNoteOff(int n)
{
  note_off(n, 127);
}

int ScanSetupGuard[10];
int ScanSetup = 0;
int ScanSetupGuard2[10];
void ScanKeyboard()
{

  NoteT++;
  int offs = ScanSetup * 6;
  for (int z = 0; z < 6; z++) StateNewStage[z + offs] = STATE_IDLE;

  MK(offs, 0, digitalReadFast(KeyboardMKBK[0]));
  BK(offs, 0, digitalReadFast(KeyboardMKBK[1]));
  MK(offs, 1, digitalReadFast(KeyboardMKBK[2]));
  BK(offs, 1, digitalReadFast(KeyboardMKBK[3]));
  MK(offs, 2, digitalReadFast(KeyboardMKBK[4]));
  BK(offs, 2, digitalReadFast(KeyboardMKBK[5]));
  MK(offs, 3, digitalReadFast(KeyboardMKBK[6]));
  BK(offs, 3, digitalReadFast(KeyboardMKBK[7]));
  MK(offs, 4, digitalReadFast(KeyboardMKBK[8]));
  BK(offs, 4, digitalReadFast(KeyboardMKBK[9]));
  MK(offs, 5, digitalReadFast(KeyboardMKBK[10]));
  BK(offs, 5, digitalReadFast(KeyboardMKBK[11]));

  for (int z = 0; z < 6; z++) StateNew[z + offs] = StateNewStage[z + offs];

  digitalWriteFast(KeyboardSink[ScanSetup], LOW);
  ScanSetup = (ScanSetup + 1) % 8;
//  Serial.println();
//  Serial.println(ScanSetup);
  digitalWriteFast(KeyboardSink[ScanSetup], HIGH);



}

void UpdateKeyboard()
{
  for (int i = 0 ; i < 48; i++)
  {
    if (State[i] != StateNew[i])
    {
      int sink = (i / 6);
      int scan = (i % 6);
      int Note = scan * 8 + sink;
      switch (StateNew[i])
      {
        case STATE_IDLE:
          DoNoteOff(Note);
          break;
        case STATE_MKBRK:
          DoNoteOn(Note, ConvertVelocity(NoteT - StateTime[i]));
          break;
      }
      StateTime[i] = NoteT;
      State[i] = StateNew[i];
    }
  }

}

void KeyboardSetup()
{

  for (int i = 0 ; i < 12; i++)
  {
    KeyboardMKBK[i] = KB[KeyboardMKBK[i]];
    pinMode(KeyboardMKBK[i], INPUT_PULLDOWN);
  }
  for (int i = 0 ; i < 8; i++)
  {
    KeyboardSink[i] = KB[KeyboardSink[i]];
    pinMode(KeyboardSink[i], OUTPUT);
  }
  KeyboardTimer.begin(ScanKeyboard, 100);
}




void setup()
{

  KeyboardSetup();

  pinMode(ENC1A, INPUT_PULLUP);
  pinMode(ENC1B, INPUT_PULLUP);
  pinMode(ENC2A, INPUT_PULLUP);
  pinMode(ENC2B, INPUT_PULLUP);

  for(int i =0 ;i<POTCOUNT;i++)
  {
    smooth_init( & PotSmooth[i],50);
  }
  pinMode(LEDDAT, OUTPUT);
  pinMode(LEDCLK, OUTPUT);
  pinMode(LEDLATCH, OUTPUT);

  pinMode(BUTTONLATCH, OUTPUT);
  pinMode(BUTTONCLK, OUTPUT);
  pinMode(BUTTONDAT, INPUT_PULLDOWN);

  for (int i = 0; i < BUTTONCOUNT; i++)
  {
    HWButtons[i] = 0;
  }
  for (int i = 0 ; i < LEDCOUNT; i++)
  {
    LEDs[i] = 255;
  }

  for (int i = 0 ; i < 6; i++)
  {
    pinMode(POTPIN[i], OUTPUT);
  }

  Serial4.begin(1400000);
  delay(2000);
  Teensy_Reset();
  for (int i = 0 ; i < 15; i++)
  {
    Serial4.write(0xff);
  }

  Teensy_InitPreset();
  Teensy_InitSD();

  Serial.begin(1000000);


}

byte last1A = HIGH;
byte last1B = HIGH;


byte last2A = HIGH;
byte last2B = HIGH;

void Enc(int encoder, int delta)
{
  Teensy_EncoderRotate(encoder, delta);
  SendEncoder(encoder, delta);
}


void CheckEncoders()
{
  byte newA = digitalRead(ENC1A);
  byte newB = digitalRead(ENC1B);
  if (newA != last1A) {
    if (newA == LOW) {
      if (last1B == HIGH) Enc(0, -1); else Enc(0, 1);
    }
    else
    {
      //  if (last1B == LOW) Enc(0,-1); else Enc(0,1);
    }
  }


  last1A = newA;
  last1B = newB;

  newA = digitalRead(ENC2A);
  newB = digitalRead(ENC2B);


  if (newA != last2A) {

    if (newA == LOW) {
      if (last2B == HIGH) Enc(1, -1); else Enc(1, 1);
    }
    else
    {
      //   if (last2B == LOW) Enc(1,-1); else Enc(1,1);
    }
  }


  last2A = newA;
  last2B = newB;


}

void ScanPots()
{
  for (int i = 0 ; i < 64; i++)
  {
    for (int j = 0; j < 6; j++)
    {
      if ((i & (1 << j)) > 0) digitalWrite(POTPIN[j], HIGH); else digitalWrite(POTPIN[j], LOW);
    }
     __asm__("nop\n\t"); 

 __asm__("nop\n\t"); 

 __asm__("nop\n\t"); 

 __asm__("nop\n\t"); 

 __asm__("nop\n\t"); 

 __asm__("nop\n\t"); 

    Pots[i] = analogRead(POTSADC);
  }
}
unsigned char ipwm = 0;
void SendLeds()
{
  ipwm += 9;
  digitalWrite(LEDLATCH, HIGH);
  for (int i = 0 ; i < LEDCOUNT; i++)
  {
    digitalWrite(LEDDAT, (LEDs[i] > ipwm) ? HIGH : LOW);
    digitalWrite(LEDCLK, LOW);
    digitalWrite(LEDCLK, HIGH);
  }
  digitalWrite(LEDLATCH, LOW);


}
uint32_t ConvertDat(uint32_t d)
{
  unsigned char d1 = d & 127;
  unsigned char d2 = (d >> 7) & 127;
  unsigned char d3 = (d >> 14) & 127;
  unsigned char d4 = (d >> 21) & 127;

  return d1 + (d2 << 8) + (d3 << 16) + (d4 << 24);
}

unsigned char buffer[10] =
{
  0, 0, 0, 0, 0,
  0, 0, 0, 0, 0
};

void SendButton(unsigned char idx, unsigned char value)
{
#ifndef SIMLINK
  return;
#endif

  uint32_t D = (idx << 8) + value;
  D = ConvertDat(D);
  Serial.write(0x81);
  Serial.write((D & 127));
  Serial.write(((D >> 8) & 127));
  Serial.write(((D >> 16) & 127));
  Serial.write(((D >> 24) & 127));
}

void SendCommand(unsigned char command, uint32_t data)
{
  #ifdef SILENTMODE
  return;
  #endif
  
  uint8_t buffer[10];
  uint32_t D = ConvertDat(data);
  buffer[0] = command;
  buffer[1] = (D & 127);
  buffer[2] = (((D >> 8) & 127));
  buffer[3] = (((D >> 16) & 127));
  buffer[4] = (((D >> 24) & 127));
  Serial.write(buffer, 5);

}

void SendEncoder(int encoder, int delta)
{
#ifndef SIMLINK
  return;
#endif

  if (delta < 0) delta = 2;
  SendCommand(0xe0, (encoder << 8) + delta);
}

void SendRaspberryState()
{
  unsigned char *b = (unsigned char * ) &Raspberry_guidata;
  uint sent = 0 ;
  byte commandb = 0xd0;
  while (sent < sizeof(Raspberry_GuiData_t) )
  {
    byte b1 = *b++;
    byte b2 = *b++;
    byte b3 = *b++;
    SendCommand(commandb, b1 + (b2 << 8) + (b3 << 16)) ;
    sent += 3;
    commandb = 0xd1;
  }
  SendCommand(0xd2, 0) ;

}

void SendPot(unsigned char idx, uint16_t value)
{
#ifndef SIMLINK
  return;
#endif

  uint32_t D = (idx << 16) + value;
  SendCommand(0x82, D);
}

int blinkcount;
int blink;
void ScanButtons()
{
  blinkcount++;
  blink = (blinkcount%1000)<500?64:0;
  digitalWrite(BUTTONLATCH, LOW);
  digitalWrite(BUTTONLATCH, HIGH);

  for (int i = 0 ; i < BUTTONCOUNT; i++)
  {
    HWButtons[i] =   ( digitalRead(BUTTONDAT) == HIGH) ? 255 : 0;

    digitalWrite(BUTTONCLK, LOW);
    digitalWrite(BUTTONCLK, HIGH);
    if (HWButtons[i] != LastButtons[i])
    {
      int N = Teensy_FindButtonIDX(i);
      if (N > -1)
      {
        Buttons[N].value = ((HWButtons[i] == 0) ? true : false);
        Teensy_ButtonPressed(Buttons[N].id, Buttons[N].value);
      }

      LastButtons[i] = HWButtons[i];
      SendButton(i, HWButtons[i]);
    }
    //LEDTarget[Buttons[i].fpid] = Buttons[i].value?255:0;
  }
  for (int i = 0; i < __LEDBUTTON_COUNT; i++)
  {
    switch(Buttons[i].ledmode)
   { 
    case LED_OFF: LEDTarget[Buttons[i].fpid] = 0;break;
    case LED_ON: LEDTarget[Buttons[i].fpid] = 255;break;
    case LED_BLINK: LEDTarget[Buttons[i].fpid] = blink;break;

    }

  }


}

int T = 0;
int SerialStatus = 0;
int SerialCounter = 0;
uint32_t SerialData = 0 ;

void DoCommand(unsigned char comm, uint32_t data)
{
  switch (comm)
  {
    case 0x60:
      for (int i = 0 ; i < 15; i++)
      {
        Serial4.write(0xff);
      }
      Teensy_ReSendPreset();

      break;
    case 0x70:
      {
        uint16_t idx = data >> 16;
        uint16_t val = data & 0xffff;
        note_on(idx, val);
      }
      break;
    case 0x71:
      {
        uint16_t idx = data >> 16;
        uint16_t val = data & 0xffff;
        note_off(idx, val);
      }
      break;
    case 0x83:
      {
        uint16_t idx = data >> 16;
        uint16_t val = data & 0xffff;
#ifndef SIMLINK
        return;
#endif
        LEDTarget[idx] = val ;
      }
      break;
  }
}
void HandleSerial(unsigned char inb)
{
  if ((inb & 0x80) == 0x80)
  {
    SerialStatus = inb;
    SerialCounter = 4;
    SerialData = 0;
  }
  else
  {
    if (SerialCounter > 0)
    {
      SerialCounter--;
      SerialData += inb << ((3 - SerialCounter) * 7);
      if (SerialCounter == 0)
      {
        DoCommand(SerialStatus, SerialData);
      }
    }

  }
}

void loop()
{
  while (Serial.available())
  {
    HandleSerial(Serial.read());
  }

  ScanPots();
  ScanButtons();
  T++;
  int highestpot = 0;
  for (int i = 0 ; i < POTCOUNT; i++)
  {
    PotAvg[i] =  smooth(&PotSmooth[i],  Pots[i]);
    if (PotAvg[i] != LastPotAvg[i])
    {

      SendPot(i, PotAvg[i]);

      int TargetIDX = Teensy_FindKnobIDX(i);
      if (TargetIDX > -1)
      {

        Knobs[TargetIDX].value =  PotAvg[i] / 1023.0f;
        Teensy_KnobChanged(Knobs[TargetIDX].id, uint32_t(floor((Knobs[TargetIDX].value * 65535.0))));
        //  printf("incoming pot: %d, %d\n", idx, val);
      }


      LastPotAvg[i] = PotAvg[i] ;
      PotStable[i] = 0;
    }
    else
    {
      PotStable[i]++;
      if (PotStable[i] == 20)
      {

      }
      else
      {
        if (PotStable[i] > 30) PotStable[i] = 30;
      }
    }
    if (PotAvg[i] > highestpot)
    {
      highestpot = PotAvg[i];
    }
  }
  for (int i = 0 ; i < LEDCOUNT; i++)
  {
    LEDs[i] = LEDTarget[i];
  }

  static int lastsend = millis();
  if (Raspberry_guidata.dirty && millis() - lastsend > 20)
  {
    Raspberry_guidata.dirty = false;
    SendRaspberryState();
  }
  CheckEncoders();
  UpdateKeyboard();
  Serial.flush();
  SendLeds();
}

char presetname[30];
void Teensy_BuildPresetName(int bank, int slot)
{
  sprintf(presetname, "pan%d_%d.prs", bank, slot);
}


#include <SD.h>
#include <SPI.h>

void Teensy_LoadSDPreset(int bank, int slot)
{
  
  Teensy_BuildPresetName(bank, slot);
  if (!SD.begin(BUILTIN_SDCARD))
  {
    InitPreset(gPreset);
    LoadPreset(gPreset);

    return;
  }
  File myFile = SD.open(presetname, FILE_READ);

  if (myFile) {
    PanPreset_t p;
    unsigned char *b = (unsigned char *)&p;
    myFile.read(b, sizeof(PanPreset_t));
    myFile.close();
    
    memcpy(&gPreset, &p, sizeof(PanPreset_t));
    LoadPreset(gPreset);

  }
  else
  {
      InitPreset(gPreset);
    LoadPreset(gPreset);

  }

}

void Teensy_InitSD()
{
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      Teensy_BuildPresetName(i, j);
      if (!SD.begin(BUILTIN_SDCARD))
      {
        return;
      }
      if (SD.exists(presetname) == false)
      {
          Teensy_SaveSDPreset(i, j);
      }
    }
  }

}

void Teensy_SaveSDPreset(int bank, int slot)
{
  Teensy_BuildPresetName(bank, slot);
  if (!SD.begin(BUILTIN_SDCARD))
  {
    return;
  }
  File myFile = SD.open(presetname, FILE_WRITE);

  if (myFile) {
    unsigned char *b = (unsigned char *)&gPreset;
    myFile.seek(0);
    myFile.write(b, sizeof(PanPreset_t));
    myFile.close();
  }
}

