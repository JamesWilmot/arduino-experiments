#ifndef pixelbit_h
#define pixelbit_h

//int pixelbit_init();
#include <math.h>

typedef struct {
  int w;
  int h;

  char *frame_buffer;
} display;

extern "C" {
void pixelbit_point(display *dev, int x, int y, bool c);
}

extern "C" {
void pixelbit_line(display *dev, int x_1, int y_1, int x_2, int y_2);
}


extern "C" {
void pixelbit_circle(display *dev, int x, int y, int r, int w);
}

extern "C" {
void pixelbit_parallelogram(int x_1, int y_1, int x_2, int y_2, int w);
}

extern "C" {
void pixelbit_hline(display *dev, int x_1, int x_2, int y);
}

extern "C" {
void pixelbit_vline(display *dev, int y_1, int y_2, int x);
}

extern "C" {
void pixelbit_rectangle(display *dev, int x_1, int y_1, 
                        int x_2, int y_2, int w);
}

#endif
