#ifndef PIDfromBT_h
#define PIDfromBT_h
#define LIBRARY_VERSION 1
#include <SoftwareSerial.h>
class PIDfromBT
{

#define DEBUG 1
#define NO_DEBUG 0

#define MIN_IDEAL 0
#define MAX_IDEAL 0

public:

  PIDfromBT(double* kp, double* kd, double* ki, int* vel, bool* acelerar, float* aumentoP, float* aumentoD, float* maxAumento, float* velIncremento, float* maxError, bool debug);
  PIDfromBT(double* kp, double* kd, double* ki, int* vel, bool debug);
  PIDfromBT(double* kp, double* kd, double* ki, int* vel, double* ideal, int minIdeal, int maxIdeal, bool debug);

  void setMinIdeal(int minIdeal);
  void setMaxIdeal(int maxIdeal);
  bool update();

private:
  String valor;
  char letra;
  long last_millis;
  bool print;
  double *kp;
  double *ki;
  double *kd;
  int *vel;
  bool* acelerar;
  float* aumentoP;
  float* aumentoD;
  float* maxAumento;
  float* velIncremento;
  float* maxError;
  double* ideal;
  int maxIdeal;
  int minIdeal;
  short acc_activada;
};
#endif
