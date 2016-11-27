#include "pixelbit.h"

// lookup function
// TODO make inline
int index(display *dev, int x, int y) {
  return y * (dev->w >> 3) + (x >> 3);
}

// TODO make inline
int bit(int x) {
  return x % 8;
}

void pixelbit_point(display *dev, int x, int y) {
  dev->frame_buffer[index(dev, x, y)] |= 1 << bit(x);
}

