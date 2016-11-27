#ifndef pixelbit_h
#define pixelbit_h

//int pixelbit_init();


typedef struct {
  int w;
  int h;

  char *frame_buffer;
} display;



extern "C" {
// use short unsigned int
void pixelbit_point(display *dev, int x, int y);
}


extern "C" {
int pixelbit_circle(display *dev, int x, int y, int r, int w);
}

extern "C" {
int pixelbit_parallelogram(int x_1, int y_1, int x_2, int y_2, int w);
}

#endif
