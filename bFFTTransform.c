#include "bFFTTransform.h"

struct FFTinput *setUp(struct egg *current, struct FFTinput *input) {

  printf("Setting up...\n");
  input->in_size = current->data->record_size;
  input->out_size = current->data->record_size / 2 + 1;
  input->step_size = (float)1 / current->data->sample_rate;
  input->in = (double*) malloc(input->in_size * sizeof(double));
  input->out = (fftw_complex*) malloc(input->out_size *sizeof(fftw_complex));
  input->plan = (fftw_plan*) fftw_malloc(sizeof(fftw_plan));
  printf("Set up.\n");

}

struct FFTinput *createPlan(struct FFTinput *input) {

  printf("Fomenting plan...\n");
  *input->plan = fftw_plan_dft_r2c_1d(input->in_size, input->in, input->out,
		 FFTW_MEASURE);
  printf("Fomented!\n");
 
  return;

}


void inputData(struct egg *current, struct FFTinput *input) {

  printf("Inputing data...\n");
  int i;
  for(i = 0; i < current->data->record_size; i++) {

    *(input->in + i) = (double)*(current->data->record + i);

  }
  printf("Data inputed.\n");
  return;

}


void executePlan(struct FFTinput *input) {

  printf("Calculating transform...\n");

  fftw_execute(*input->plan);

  return;
}

void displayResults(struct FFTinput *input) {
  
  printf("Power Spectrum:\n");
  printf("frequency\t");
  printf("Power\n");

  int i;
  for(i = 0; i < input->out_size; i++) {

    float f_val;
    f_val = ( (float)i / input->in_size) * ( (float)1 / (input->step_size));

    double a;
    double b;
    
    double aSquared;
    double bSquared;
    double Power;

    a = input->out[i][0];
    b = input->out[i][1];

    aSquared = a * a;
    bSquared = b * b;

    Power = aSquared + bSquared;

    printf("%f\t",f_val);
    printf("%f\n",Power);

  }

  return;

}

void createOutputFile(struct FFTinput *input) {
  
  FILE *fp;  
  fp = fopen("results_bFFT", "w");

  double maxf = 0.0;
  double max = 0.0;
 
  int i;
  for(i = 0; i < input->out_size; i++) {

    float f_val;
    f_val = ( (float)i / input->in_size) * (1 / (input->step_size));

    double a;
    double b;
    
    double aSquared;
    double bSquared;
    double Power;

    a = input->out[i][0];
    b = input->out[i][1];

    aSquared = a * a;
    bSquared = b * b;

    Power = aSquared + bSquared;

    if(Power > max) {

      max = Power;
      maxf = f_val;

    }

    fprintf(fp,"%f\t",f_val);
    fprintf(fp,"%f\n",Power);
  
  }
  
  printf("%f\t",maxf);
  printf("%f\n",max);
  
  return;

 }

void cleanUp(struct FFTinput *input) {
  
  free(input->in);
  fftw_free(input->out);
  fftw_destroy_plan(*(input->plan));

  printf("Clean up successful.\n");

  return;
}
