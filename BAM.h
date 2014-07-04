#ifndef BAM_H
#define BAM_H

#include <Arduino.h>

namespace BAMNamespace {
  
struct Frame {
  byte rows[8];
};

class BAMClass {
  public:
  volatile Frame frames[8];
  volatile byte tickpos;

  void begin();
  
  void set(char row, char col, byte value);
  byte get(char row, char col);
  byte linearize(byte value);
};

}

extern BAMNamespace::BAMClass BAM;

#endif
