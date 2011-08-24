#include <math.h>
#include <fftw3.h>

#include "monarch.h"

struct FFTinput{
  int in_size;
  int out_size;
  float step_size;
  double *in;
  fftw_complex *out;
  fftw_plan *plan;
};

struct FFTinput *setUp(struct egg *, struct FFTinput *);
struct FFTinput *createPlan(struct FFTinput *);
void inputData(struct egg *, struct FFTinput *);
void executePlan(struct FFTinput *);
void displayResults(struct FFTinput *);
void createOutputFile(struct FFTinput *);
void cleanUp(struct FFTinput *);
