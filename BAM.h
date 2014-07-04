/*
  Copyright 2014 Brian Taylor
  This file is part of BAM.

  BAM is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  
  BAM is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
*/

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
