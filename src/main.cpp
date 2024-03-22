#include <Arduino.h>

#define SG_Enable 0
#define SG_Latch 12
#define SG_Clock 13
#define SG_Data 14
#define Buzzer 16
#define Start 5
#define Stop 4

//           ABCDEFG
#define _1 0b01100000
#define _2 0b11011010
#define _3 0b11110010
#define _4 0b01100110
#define _5 0b10110110
#define _6 0b10111110
#define _7 0b11100000
#define _8 0b11111110
#define _9 0b11110110
#define _0 0b11111100

uint8_t numberArray[10] = {_0, _1, _2, _3, _4, _5, _6, _7, _8, _9};

// put function declarations here:
void Blink(int);
void update7segment(void);
void CoundownLOOP(void);
void stopCoundown(void);
void startCoundown(void);
void shiftOut1(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val);
void updateShiftRegister();

byte leds = 0;
byte _index = 0;
int8_t countdownIndex = 0;
int8_t countdownFrom = 5;
boolean countdowning = false;
boolean countdownDone = false;
uint32_t countdownMillis = 0;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, World!!!");

  pinMode(Buzzer, OUTPUT);
  pinMode(Start, INPUT_PULLUP);
  pinMode(Stop, INPUT_PULLUP);

  pinMode(SG_Enable, OUTPUT);
  digitalWrite(SG_Enable, LOW);
  pinMode(SG_Latch, OUTPUT);
  pinMode(SG_Clock, OUTPUT);
  pinMode(SG_Data, OUTPUT);
  leds = numberArray[countdownFrom];
  delayMicroseconds(100);
}

void loop()
{
  // put your main code here, tso run repeatedly:
  // Blink(Buzzer);

  if (!digitalRead(Start))
    startCoundown();

  if (!digitalRead(Stop))
    stopCoundown();

  CoundownLOOP();
  update7segment();
}

void stopCoundown(void)
{
  if (countdowning)
  {
    countdowning = false;
    leds = numberArray[countdownFrom];
  }
}

void startCoundown(void)
{
  if (!countdowning)
  {
    countdowning = true;
    countdownMillis = millis();
    countdownIndex = countdownFrom;
  }

  if (countdownDone)
  {
    countdownDone = false;
    leds = numberArray[countdownFrom];
  }
}

void CoundownLOOP(void)
{
  if (countdowning && millis() - countdownMillis >= 1000)
  {
    countdownMillis = millis();
    countdownIndex--;
    leds = numberArray[countdownIndex];
    if (countdownIndex <= 0)
    {
      countdowning = false;
      countdownDone = true;
    }
  }

  if (countdownDone)
    digitalWrite(Buzzer, HIGH);
  else
    digitalWrite(Buzzer, LOW);
}

// put function definitions here:
void shiftOut1(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val)
{
  uint8_t i;

  for (i = 0; i < 8; i++)
  {
    if (bitOrder == LSBFIRST)
      digitalWrite(dataPin, !!(val & (1 << i)));
    else
      digitalWrite(dataPin, !!(val & (1 << (7 - i))));

    digitalWrite(clockPin, HIGH);
    delayMicroseconds(100);
    digitalWrite(clockPin, LOW);
    delayMicroseconds(100);
  }
}

void Blink(int Pin)
{
  static uint32_t countdownMillis = 0;
  if (millis() - countdownMillis > 500)
  {
    countdownMillis = millis();

    // leds = numberArray[_index];

    // if (digitalRead(Pin))
    // {
    //   Serial.print("Index: ");
    //   Serial.print(_index);
    //   Serial.print(", Binary: ");
    //   for (int i = 7; i >= 0; i--)
    //   {
    //     Serial.print(bitRead(leds, i)); // Print each bit
    //   }
    //   Serial.println(); // Print newline for readability
    //   _index++;
    // }

    // if (_index > 9)
    // {
    //   _index = 0;
    //   leds = 0;
    // }

    digitalWrite(Pin, !digitalRead(Pin));
  }
}

void updateShiftRegister()
{
  digitalWrite(SG_Latch, LOW);
  delayMicroseconds(100);
  shiftOut1(SG_Data, SG_Clock, LSBFIRST, leds);
  // delayMicroseconds(100);
  digitalWrite(SG_Latch, HIGH);
  delayMicroseconds(100);
}

void update7segment(void)
{
  static uint32_t countdownMillis = 0;
  if (millis() - countdownMillis >= 30)
  {
    countdownMillis = millis();
    updateShiftRegister();
  }
}