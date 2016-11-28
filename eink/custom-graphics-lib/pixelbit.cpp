#include "pixelbit.h"

// lookup function
// TODO make inline
//
void _pixelbit_steep_line(display *dev, int x_1, int y_1, int x_2, int y_2);
void _pixelbit_shallow_line(display *dev, int x_1, int y_1, int x_2, int y_2);

#define DEVICE_BYTES_PER_LINE (dev->w >> 3)

int index(display *dev, int x, int y) {
  return y * DEVICE_BYTES_PER_LINE + (x >> 3);
}

int bit(int x) {
  return x % 8;
}

void pixelbit_point(display *dev, int x, int y, bool c) {
  dev->frame_buffer[index(dev, x, y)] |= 1 << bit(x);
}

void pixelbit_line(display *dev, int x_1, int y_1, int x_2, int y_2) {
  int temp;

  if (y_1 == y_2) {
    pixelbit_hline(dev, x_1, x_2, y_1);
    return;
  }

  if (x_1 == x_2) {
    pixelbit_vline(dev, y_1, y_2, x_1);
    return;
  }

  if (y_2 < y_1) {
    temp = y_2;
    y_2 = y_1;
    y_1 = temp;

    temp = x_2;
    x_2 = x_1;
    x_1 = temp;
  }

  float m = (float)(y_2 - y_1) / (float)(x_2 - x_1);

  if (m <= 1.0 || m >= -1.0) {
    _pixelbit_steep_line(dev, x_1, y_1, x_2, y_2);
  } else {
    _pixelbit_shallow_line(dev, x_1, y_1, x_2, y_2);
  }
}

void pixelbit_hline(display *dev, int x_1, int x_2, int y) {
  //int start_index = index(dev, x_1, y);
  //int end_index = index(dev, x_2, y);
 
  int x;
  for(x = x_1; x <= x_2; x++) {
    pixelbit_point(dev, x, y, 1);
  }
}

void pixelbit_vline(display *dev, int y_1, int y_2, int x) {
  int temp;

  if (y_2 < y_1) {
    temp = y_2;
    y_2 = y_1;
    y_1 = temp;
  }
 
  int start_index = index(dev, x, y_1);
  int end_index = index(dev, x, y_2);
  int jump = DEVICE_BYTES_PER_LINE; 
  int index = 0;

  for (index = start_index; index <= end_index; index += jump) {
    dev->frame_buffer[index] |= 1 << bit(x);
  }
}

void pixelbit_rectangle(display *dev, int x_1, int y_1, 
                        int x_2, int y_2, int w) {
    
  if(!w) {
    int y;
    for(y = y_1; y <= y_2; y++) {
      pixelbit_hline(dev, x_1, x_2, y);
    }
  } else {
    pixelbit_hline(dev, x_1, x_2, y_1);
    pixelbit_hline(dev, x_1, x_2, y_2);

    pixelbit_vline(dev, y_1, y_2, x_1);
    pixelbit_vline(dev, y_1, y_2, x_2);
  }
}

void pixelbit_circle(display *dev, int x, int y, int r, int w) {
}

void _pixelbit_shallow_line(display *dev, int x_1, int y_1, int x_2, int y_2) {
  float m = (float)(y_2 - y_1) / (float)(x_2 - x_1);

  int x, y;

  for(x = x_1; x <= x_2; x++) {
    y = round ( m * ( x - x_1) + y_1 );
    pixelbit_point(dev, x, y, 1);
  }
}

void _pixelbit_steep_line(display *dev, int x_1, int y_1, int x_2, int y_2) {
  int temp;

  float m = (float)(y_2 - y_1) / (float)(x_2 - x_1);
  
  /*if (y_2 < y_1) {*/
    //temp = y_2;
    //y_2 = y_1;
    //y_1 = temp;

    //temp = x_2;
    //x_2 = x_1;
    //x_1 = temp;
  /*}*/

  int x, y;
  for(y = y_1; y <= y_2; y++) {
    x = round( m * ( y - y_1) + x_1 ); 
    pixelbit_point(dev, x, y, 1);
  }
}
