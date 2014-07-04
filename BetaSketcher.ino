#include <SPI.h>
#include "BAM.h"

#define JOYX A8
#define JOYY A5
#define BUTTON 1
#define LIGHT A9

#define LIVEZONE 256

int penx = 3;
int peny = 3;
int penstate = 0;
int light_neutral;

// rows, columns
byte screen[8][8];

void setup() {
  BAM.begin();
  memset(screen, 0, sizeof(screen));
  
  pinMode(LIGHT, INPUT);
  light_neutral = 0;
  for(char i = 0; i < 10; i++) {
    light_neutral += analogRead(LIGHT)/10;
  }
}

void drawScreen(char cc, char cr) {
  penstate = (penstate + 1) % 4;
  for(uint8_t row = 0; row < 8; ++row) {
    for(uint8_t col = 0; col < 8; ++col) {
      byte toggle = (screen[row][col] > 3 ? 1 : 7);
      byte i = 0;
      if(row == cr && col == cc) {
        if(penstate > 2 && screen[row][col] > 0) {
         i = toggle;
        } else if(screen[row][col] == 0) {
         i = 1;
        } else {
         i = screen[row][col];
        }
      } else {
        i = screen[row][col];
      }
      BAM.set(row, col, BAM.linearize(i));
    }
  }
}


void loop() {
  long start = millis();
  
  int dx = analogRead(JOYX);
  int dy = analogRead(JOYY);
  if(dx < LIVEZONE) {
    penx -= 1;
  } else if(dx > 1023 - LIVEZONE) {
    penx += 1;
  }
  if(dy < LIVEZONE) {
    peny += 1;
  } else if(dy > 1023 - LIVEZONE) {
    peny -= 1;
  }
  penx = constrain(penx, 0, 7);
  peny = constrain(peny, 0, 7);
  
  if(!digitalRead(BUTTON)) {
    screen[peny][penx] = min(7, screen[peny][penx] + 1);
  }
  
  // clear the screen on significant light change
  if(abs(light_neutral - analogRead(LIGHT)) > light_neutral / 2) {
    memset(screen, 0, sizeof(screen));
  }
  
  drawScreen(penx, peny);
  
  long dt = millis() - start;
  delay(max(0, 100 - dt));
}
