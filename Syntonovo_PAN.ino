
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



void WriteSwitch(int id, int state)
{
  //TODO
}

void WriteSyncLfo(uint8_t* paramids)
{
  //TODO
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

void setup()
{
  pinMode(ENC1A, INPUT_PULLUP);
  pinMode(ENC1B, INPUT_PULLUP);
  pinMode(ENC2A, INPUT_PULLUP);
  pinMode(ENC2B, INPUT_PULLUP);

  
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
  Teensy_Reset();
  Teensy_InitPreset();
  
  Serial.begin(1000000);
  Serial4.begin(115200);
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
      if (last1B == HIGH) Enc(0,-1); else Enc(0,1);
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
      if (last2B == HIGH) Enc(1,-1); else Enc(1,1);
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
  uint8_t buffer[10];
  uint32_t D = ConvertDat(data);
  buffer[0] = command;
  buffer[1] = (D & 127);
  buffer[2] = (((D >> 8) & 127));
  buffer[3] = (((D >> 16) & 127));
  buffer[4] = (((D >> 24) & 127));
  Serial.write(buffer, 5);
  
}

void SendEncoder(int encoder,int delta)
{
  if (delta <0) delta = 2;
  SendCommand(0xe0, (encoder<<8) + delta);
}

void SendRaspberryState()
{
  unsigned char *b =(unsigned char * ) &Raspberry_guidata;
  uint sent =0 ;
  byte commandb = 0xd0;
  while (sent <sizeof(Raspberry_GuiData_t) )
  {
    byte b1 = *b++;
    byte b2 = *b++;
    byte b3 = *b++;
    SendCommand(commandb, b1 + (b2<<8) + (b3<<16)) ;
    sent+=3;
    commandb = 0xd1;
  }
   SendCommand(0xd2, 0) ;
   
}

void SendPot(unsigned char idx, uint16_t value)
{
  uint32_t D = (idx << 16) + value;
  SendCommand(0x82, D);
}


void ScanButtons()
{

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
    case 0x83:
      {
        uint16_t idx = data >> 16;
        uint16_t val = data & 0xffff;
        LEDTarget[idx] = val ;
        SendCommand(0x90, idx);
        SendCommand(0x91, val);
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
      SerialData += inb << ((3-SerialCounter) * 7);
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
    PotAvg[i] =  ( PotAvg[i] * 7 + Pots[i]) / 8;
    if (PotAvg[i] != LastPotAvg[i])
    {
      
      SendPot(i, PotAvg[i]);

       int TargetIDX = Teensy_FindKnobIDX(i);
    if (TargetIDX > -1)
    {

      Knobs[TargetIDX].value =  PotAvg[i] / 1023.0f;
      Teensy_KnobChanged(Knobs[TargetIDX].id, uint32_t(floor((Knobs[TargetIDX].value*65535.0))));
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
  if (Raspberry_guidata.dirty)
  {
    Raspberry_guidata.dirty = false;
    SendRaspberryState();
  }
  CheckEncoders();
  Serial.flush();
  SendLeds();
}


