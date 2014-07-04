#include <SPI.h>
#include "BAM.h"
#include <avr/pgmspace.h>


BAMNamespace::BAMClass BAM;

#define CIELPWM(a) (pgm_read_word_near(CIEL8 + a)) // CIE Lightness loopup table function

#define LED_MOSI 10
#define LED_CLK 11
#define LED_ENABLE 12

prog_uint8_t CIEL8[] PROGMEM = {
    0, 15, 31, 63, 70, 100, 127, 255
};

#define BIT(x, n) ((x >> n) & 0x01)

#define OP_NOOP   0
#define OP_DIGIT0 1
#define OP_DIGIT1 2
#define OP_DIGIT2 3
#define OP_DIGIT3 4
#define OP_DIGIT4 5
#define OP_DIGIT5 6
#define OP_DIGIT6 7
#define OP_DIGIT7 8
#define OP_DECODEMODE  9
#define OP_INTENSITY   10
#define OP_SCANLIMIT   11
#define OP_SHUTDOWN    12
#define OP_DISPLAYTEST 15
  
inline void ledTransfer(byte opcode, byte data) {
  PORTA &= ~(1 << 6);
  SPDR = opcode;
  while(!(SPSR & _BV(SPIF)));
  SPDR = data;
  while(!(SPSR & _BV(SPIF)));
  PORTA |= (1 << 6);
}

void initLED() {
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.begin();
  
  pinMode(LED_MOSI, OUTPUT);
  pinMode(LED_CLK, OUTPUT);
  pinMode(LED_ENABLE, OUTPUT);
  
  ledTransfer(OP_DISPLAYTEST, 0);
  ledTransfer(OP_SCANLIMIT, 7);
  ledTransfer(OP_DECODEMODE, 0);
  ledTransfer(OP_SHUTDOWN, 1);
  ledTransfer(OP_INTENSITY, 1);
}

namespace BAMNamespace {
  
void BAMClass::set(char row, char col, byte value) {
  for(char ii = 0; ii < 8; ++ii) {
    if(BIT(value, ii)) {
      frames[ii].rows[row] |= (1<<col);
    } else {
      frames[ii].rows[row] &= ~(1<<col);
    }
  }
}

byte BAMClass::get(char row, char col) {
  uint8_t value = 0;
  for(char ii = 0; ii < 8; ++ii) {
    if(BIT(frames[ii].rows[row], col)) {
      value |= (1 << ii);
    }
  }
  return value;
}

void BAMClass::begin() {
  memset((void*)frames, 0, sizeof(frames));
  initLED();
  
  tickpos = 0;
  
  TCCR1A = 0;
  TCCR1B = 0;

  TCCR1B |= (1 << WGM12); // CTC
  TCCR1B |= (1 << CS12) | (1 << CS10); // 1024 prescaler
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
  
  TCNT1 = 0;
}

byte BAMClass::linearize(byte value) {
  return CIELPWM(value);
}
}


#define TRANSFER_ROW(n) ledTransfer(n+1, BAM.frames[BAM.tickpos].rows[n])

ISR(TIMER1_COMPA_vect) {
  next_frame:
  BAM.tickpos = (BAM.tickpos + 1) & 7;
  OCR1A = (1 << BAM.tickpos);

  TRANSFER_ROW(0);
  TRANSFER_ROW(1);
  TRANSFER_ROW(2);
  TRANSFER_ROW(3);
  TRANSFER_ROW(4);
  TRANSFER_ROW(5);
  TRANSFER_ROW(6);
  TRANSFER_ROW(7);
}
