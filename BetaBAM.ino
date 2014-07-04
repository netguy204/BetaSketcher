#include <SPI.h>
#include <avr/pgmspace.h>

#define CIELPWM(a) (pgm_read_word_near(CIEL8 + a)) // CIE Lightness loopup table function

#define JOYX A8
#define JOYY A5
#define BUTTON 1
#define LIGHT A9

#define LED_MOSI 10
#define LED_CLK 11
#define LED_ENABLE 12

//const uint8_t delays[] = {15, 31, 63, 127, 255};
const uint8_t delays[] = {1, 2, 4, 8, 16, 32, 64, 128};
prog_uint8_t CIEL8[] PROGMEM = {
    0, 15, 31, 63, 70, 100, 127, 255
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


void setup() {
  memset((void*)frames, 0, sizeof(frames));
  initLED();
  
  for(uint8_t row = 0; row < 8; ++row) {
    for(uint8_t col = 0; col < 8; ++col) {
      char dx = 3 - col;
      char dy = 3 - row;
      char d2 = dx*dx + dy*dy;
      setOutput(row, col, CIELPWM(7-min(7,d2/2)));
    }
  }
  
  tickpos = 0;
  
  noInterrupts();
  TCCR0A = 0;
  TCCR0B = 0;
  TCNT0 = 0;

  TCCR0A |= (1 << WGM01); // CTC
  TCCR0B |= (1 << CS02) | (1 << CS01) | (1 << CS00); // 128 prescaler
  TIMSK0 |= (1 << OCIE0A); // enable timer compare interrupt
  interrupts();  
}

#define TRANSFER_ROW(n) ledTransfer(n+1, frames[tickpos].rows[n])

ISR(TIMER0_COMPA_vect) {
  next_frame:
  tickpos = (tickpos + 1) & 7;
  OCR0A = delays[tickpos];
  
  //ledTransfer(OP_SHUTDOWN, 0);
  TRANSFER_ROW(0);
  TRANSFER_ROW(1);
  TRANSFER_ROW(2);
  TRANSFER_ROW(3);
  TRANSFER_ROW(4);
  TRANSFER_ROW(5);
  TRANSFER_ROW(6);
  TRANSFER_ROW(7);
  //ledTransfer(OP_SHUTDOWN, 1);

  
  /*
  for(uint8_t row = 0; row < 8; row++) {
    digitalWrite(LED_ENABLE, LOW);
    SPI.transfer(row + 1);
    SPI.transfer(frames[tickpos].rows[row]);
    digitalWrite(LED_ENABLE, HIGH);
  }
  */
 
  if(TCNT0 >= delays[tickpos]) {
    TCNT0 -= delays[tickpos];
    goto next_frame;
  }
  
  //TCNT0 = 0;
}

void loop() {
  for(char i = 0; i < 8; i++) {
    for(char r = 0; r < 8; r++) {
      char d = abs(i - r);
      char i = 7 - d;
      for(char c = 0; c < 8; c++) {
        //setOutput(r, c, i);
      }
    }
  }
}
