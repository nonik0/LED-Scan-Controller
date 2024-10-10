#include <Arduino.h>
#include <SPI.h>

#include "services.hpp"

#define ROWS 16
#define COLS 16
#define NUM_LEDS ROWS * COLS

#define DISPLAY_PIN 3 // !OE, can be tied to GND if need to save pin
#define LATCH_PIN 18   // RCLK
// #define CLOCK_PIN 5 // SRCLK
// #define DATA_PIN 18 // SER

volatile bool display = true;
uint16_t displayBuffer[COLS];

void displayTask(void *pvParameters)
{
  log_d("Starting display task...");

  while (1)
  {
    for (int i = 0; i < ROWS; i++)
    {
      digitalWrite(LATCH_PIN, LOW);
      SPI.transfer16(~displayBuffer[i]); // row data
      SPI.transfer16(~(0x01 << i)); // select col
      digitalWrite(LATCH_PIN, HIGH);
    }

    // dimming/blank cycle
    digitalWrite(LATCH_PIN, LOW);
    SPI.transfer16(0xFFFF); // row data
    SPI.transfer16(0xFFFF); // select col
    digitalWrite(LATCH_PIN, HIGH);
    // delayMicroseconds(500);
    delay(2);
  }
}

void setPixel(uint8_t x, uint8_t y, uint8_t brightness)
{
  if (x < 0 || x >= COLS || y < 0 || y >= ROWS)
  {
    return;
  }

  // set brightness
  if (brightness < 0 || brightness > 3)
  {
    return;
  }

  //pixelBuf[y] |= (brightness & 0x03 << x * 2);
  displayBuffer[y] |= (brightness > 0) << x;
}

void setup()
{
  delay(5000);
  Serial.begin(115200);
  log_d("Starting setup...");

  wifiSetup();
  mDnsSetup();
  otaSetup();
  restSetup();

  pinMode(DISPLAY_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  digitalWrite(DISPLAY_PIN, LOW);
  digitalWrite(LATCH_PIN, LOW);

  SPI.begin();

  xTaskCreatePinnedToCore(
      displayTask,
      "displayTask",
      1000,
      NULL,
      1,
      NULL,
      0); // Arduino loop() is running on core 1

  log_d("Displaying...");
}

int i = 0;
void loop()
{
  for (int x = 0; x < COLS; x++)
  {
    for (int y = 0; y < ROWS; y++)
    {
      setPixel(x, y, (i + x + y) % 4);
    }
  }
  delay(40);
}