#include <SPI.h>
#include "BAM.h"

#define JOYX A8
#define JOYY A5
#define BUTTON 1
#define LIGHT A9

void setup() {
  BAM.begin();
}

void drawBall(char cr, char cc) {   
  for(uint8_t row = 0; row < 8; ++row) {
    for(uint8_t col = 0; col < 8; ++col) {
      int dx = cr - col*16;
      int dy = cc - row*16;
      int d2 = dx*dx + dy*dy;
      BAM.set(row, col, BAM.linearize(7-min(7,d2/512)));
    }
  }
}

int ballx = 3;
int bally = 4;
int balldx = 3;
int balldy = 5;

void loop() {
  long start = millis();
  ballx += balldx;
  bally += balldy;
  
  if(ballx >= 128 || ballx < 0) {
    ballx = constrain(ballx, 0, 127);
    balldx = -balldx;
  }
  if(bally >= 128 || bally < 0) {
    bally = constrain(bally, 0, 127);
    balldy = -balldy;
  }
  
  drawBall(bally, ballx);
  
  long dt = millis() - start;
  delay(max(0, 30 - dt));
}
