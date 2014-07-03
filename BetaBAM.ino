#include <LedControl.h>
#include <avr/pgmspace.h>

#define CIELPWM(a) (pgm_read_word_near(CIEL8 + a)) // CIE Lightness loopup table function

#define JOYX A8
#define JOYY A5
#define BUTTON 1
#define LIGHT A9

const uint8_t delays[] = {15, 31, 63, 127, 255};
prog_uint8_t CIEL8[] PROGMEM = {
    0, 1, 9, 31
};

struct Frame {
  uint8_t rows[8];
};

volatile Frame frames[8];
volatile uint8_t tickpos;
volatile uint8_t ticksleft;

#define BIT(x, n) ((x >> n) & 0x01)



void setOutput(uint8_t row, uint8_t col, uint8_t value) {
  for(uint8_t ii = 0; ii < 8; ++ii) {
    if(BIT(value, ii)) {
      frames[ii].rows[row] |= (1<<col);
    } else {
      frames[ii].rows[row] &= ~(1<<col);
    }
  }
}

uint8_t getOutput(uint8_t row, uint8_t col) {
  uint8_t value = 0;
  for(uint8_t ii = 0; ii < 8; ++ii) {
    if(BIT(frames[ii].rows[row], col)) {
      value |= (1 << ii);
    }
  }
  return value;
}

LedControl lc = LedControl(10,11,12,1);

void setup() {
  memset((void*)frames, 0, sizeof(frames));
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);
  
  for(uint8_t row = 0; row < 8; ++row) {
    for(uint8_t col = 0; col < 8; ++col) {
      setOutput(row, col, CIELPWM(row/2));
    }
  }
  
  tickpos = 0;
  
  noInterrupts();
  TCCR0A = 0;
  TCCR0B = 0;
  TCNT0 = 0;
  
  OCR0A = 1; // (16 Mhz / N) / 256 = F Hz
  TCCR0A |= (1 << WGM01); // CTC
  TCCR0B |= (1 << CS02) | (1 << CS01);// | (1 << CS00); // 128 prescaler
  TIMSK0 |= (1 << OCIE0A); // enable timer compare interrupt
  interrupts();  
}

ISR(TIMER0_COMPA_vect) {
  tickpos = (tickpos + 1) % 5;
  OCR0A = delays[tickpos];
  

  for(uint8_t row = 0; row < 8; row++) {
    lc.setRow(0, row, frames[tickpos].rows[row]);
  }
  
  TCNT0 = 0;
}

void loop() {

}
