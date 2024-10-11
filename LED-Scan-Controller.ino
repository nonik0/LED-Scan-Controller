#include <Arduino.h>
#include <SPI.h>

#include "services.hpp"

#define REFRESH_RATE_US 1200 //500  // per line
//#define REFRESH_RATE_US 500 * 16  // per frame

#define NUM_ROWS 16
#define NUM_COLS 16
#define NUM_LEDS ROWS *COLS
#define NUM_BLANK_CYCLES 0

#define EXT_DISPLAY_PIN 15
#define DISPLAY_PIN 3  // !OE, can be tied to GND if need to save pin
#define LATCH_PIN 18   // RCLK
// #define CLOCK_PIN 5 // SRCLK
// #define DATA_PIN 18 // SER

volatile bool display = true;
volatile uint16_t displayBuffer[NUM_ROWS] = {
  0b0000000000000000,
  0b0000000110000000,
  0b0000001111000000,
  0b0000001111000000,
  0b0111111111111110,
  0b0011110110111100,
  0b0001110110111000,
  0b0000111111110000,
  0b0000111111110000,
  0b0000111111110000,
  0b0001111111111000,
  0b0001111111111000,
  0b0011111001111100,
  0b0011100000011100,
  0b0110000000000110,
  0b0000000000000000,
};

hw_timer_t *refreshTimer;
volatile int curLine = 0;
void IRAM_ATTR refreshTimerCallback() {
  uint16_t rowData, colData;

  if (curLine < NUM_ROWS) {
    rowData = ~displayBuffer[curLine];
    colData = ~(0x01 << curLine);
  }
  else {
    rowData = 0xFFFF;
    colData = 0xFFFF;
  }

  SPI.transfer16(rowData);
  SPI.transfer16(colData);
  digitalWrite(LATCH_PIN, LOW);
  digitalWrite(LATCH_PIN, HIGH);

  curLine = (curLine + 1) % (NUM_ROWS + NUM_BLANK_CYCLES);

  // for (curLine = 0; curLine < NUM_ROWS; curLine++) {
  //   SPI.transfer16(~displayBuffer[curLine]); // row data
  //   SPI.transfer16(~(0x01 << curLine));  // select col
  //   digitalWrite(LATCH_PIN, LOW);
  //   digitalWrite(LATCH_PIN, HIGH);
  // }
  // SPI.transfer16(0xFFFF);
  // SPI.transfer16(0xFFFF);
  // digitalWrite(LATCH_PIN, LOW);
  // digitalWrite(LATCH_PIN, HIGH);
}

// void setPixel(uint8_t x, uint8_t y, bool state) {
//   if (x < 0 || x >= NUM_COLS || y < 0 || y >= NUM_ROWS) {
//     return;
//   }

//   if (state) {
//     displayBuffer[y] &= ~(1 << x);
//   } else {
//     displayBuffer[y] |= 1 << x;
//   }
// }

void setup() {
  Serial.begin(115200);
  log_d("Starting setup...");
  // xTaskCreatePinnedToCore(core0Task, "core0Task", 2000, NULL, 6, NULL, 1);
  // xTaskCreatePinnedToCore(core1Task, "core1Task", 2000, NULL, 6, NULL, 0);
  pinMode(EXT_DISPLAY_PIN, OUTPUT);
  pinMode(DISPLAY_PIN, OUTPUT);
  digitalWrite(EXT_DISPLAY_PIN, display);
  digitalWrite(DISPLAY_PIN, !display);
  pinMode(LATCH_PIN, OUTPUT);
  digitalWrite(LATCH_PIN, LOW);
  SPI.begin();

  refreshTimer = timerBegin(1000000);
  timerAttachInterrupt(refreshTimer, &refreshTimerCallback);
  timerAlarm(refreshTimer, REFRESH_RATE_US, true, 0);
  log_d("Started refresh timer");

  log_d("Finished setup");
}

void loop() {
  delay(50);
  // for (curLine = 0; curLine < NUM_ROWS; curLine++) {
  //   SPI.transfer16(~displayBuffer[curLine]); // row data
  //   SPI.transfer16(~(0x01 << curLine));  // select col
  //   digitalWrite(LATCH_PIN, LOW);
  //   digitalWrite(LATCH_PIN, HIGH);
  //   delay(1000);
  // }
  // SPI.transfer16(0xFFFF);
  // SPI.transfer16(0xFFFF);
  // digitalWrite(LATCH_PIN, LOW);
  // digitalWrite(LATCH_PIN, HIGH);
  // delay(1000);



  // scan animation test

  // // row scan
  // for (int row = 0; row < NUM_ROWS; row++) {
  //   for (int y = 0; y < NUM_ROWS; y++) {
  //     for (int x = 0; x < NUM_COLS; x++) {
  //       setPixel(x, y, y == row);
  //     }
  //   }
  //   delay(250);
  // }

  // // col scan
  // for (int col = 0; col < NUM_COLS; col++) {
  //   for (int x = 0; x < NUM_COLS; x++) {
  //     for (int y = 0; y < NUM_ROWS; y++) {
  //       setPixel(x, y, x == col);
  //     }
  //   }
  //   delay(250);
  // }

  // // diagonal scan
  // for (int diag = 0; diag < NUM_ROWS + NUM_COLS - 1; diag++) {
  //   for (int x = 0; x < NUM_COLS; x++) {
  //     for (int y = 0; y < NUM_ROWS; y++) {
  //       if (x + y == diag) {
  //         setPixel(x, y, 1);
  //       } else {
  //         setPixel(x, y, 0);
  //       }
  //     }
  //   }
  //   delay(250);
  // }
}