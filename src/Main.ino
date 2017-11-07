///////////////////
// PINES MOTORES //
///////////////////
#define MOTOR_IZQUIERDO_ADELANTE PB12
#define MOTOR_IZQUIERDO_ATRAS PB0
#define MOTOR_IZQUIERDO_PWM PB1
#define MOTOR_DERECHO_ADELANTE PB14
#define MOTOR_DERECHO_ATRAS PB15
#define MOTOR_DERECHO_PWM PA8
#define MOTOR_RUN PB13
#define MOTOR_DERECHO_ENCODER PB11
#define MOTOR_IZQUIERDO_ENCODER PB9
#define MOTOR_VENTILADOR PB8

//////////////////
// PINES SWITCH //
//////////////////
#define SW_1 PA11
#define SW_2 PA12

//////////////////
// PINES BTN/LED //
//////////////////
#define BTN PB5
#define LED PC13

////////////////////
// PINES SENSORES //
////////////////////
#define SENSOR_1 PA6
#define SENSOR_2 PA5
#define SENSOR_3 PA4
#define SENSOR_4 PA3
#define SENSOR_5 PA2
#define SENSOR_6 PA1
#define SENSOR_7 PA0
#define SENSOR_IZQUIERDO_XSLEEP PA15
#define SENSOR_FRONTAL_XSLEEP PB3
#define SENSOR_DERECHO_XSLEEP PB4

//////////////////
// SENSORES I2C //
//////////////////
#include <Wire.h>
#include <VL53L0X.h>

VL53L0X sensor_izquierdo, sensor_frontal, sensor_derecho;

/////////////
//CIRCUITO //
/////////////
#define MODO_LINEA 1
#define MODO_DEGRADADO 2
// Establece el tipo de circuito
#define MODO_CIRCUITO MODO_LINEA


//////////////////////
// SENSORES QRE1113 //
//////////////////////
const short NUM_SENSORS =  7;
short sensorPins[] = {SENSOR_1, SENSOR_2, SENSOR_3, SENSOR_4, SENSOR_5, SENSOR_6, SENSOR_7};


//////////////////////////
//VARIABLES DE LECTURAS //
//////////////////////////
#define REALIZAR_CALIBRACION 1
#define CALIBRATION_TIME 5000
int line_sensor_values[] = {0, 0, 0, 0, 0, 0, 0};
int minVal = 100; // Valor mínimo por debajo del cual se interpreta como negro
int maxVal = 3850; // Valor máximo por encima del cual se interpreta como blanco
int minVals[] = {4095, 4095, 4095, 4095, 4095, 4095, 4095};
int maxVals[] = {0, 0, 0, 0, 0, 0, 0};
int minMappedVals[] = {4095, 4095, 4095, 4095, 4095, 4095, 4095};
int maxMappedVals[] = {0, 0, 0, 0, 0, 0, 0};
float mediaMax = 0;
float mediaMin = 0;


void setup() {
  // Arranca todos los componentes
  init_all();
  delay(100);
  calibrate_line_sensors((bool)REALIZAR_CALIBRACION);

}

void loop() {
double get_position(double last_position){
  switch (MODO_CIRCUITO) {
    case MODO_LINEA:

    // Número de sensores detectando negro. Por defecto todos
    int lineSensors;
    lineSensors = NUM_SENSORS;

    for(int sensor = 0;sensor < NUM_SENSORS;sensor++){
      // Mapea la lectura de los sensores a 0-4095, en función de sus maximos y minimos de cada sensor
      line_sensor_values[sensor] = map(analogRead(sensorPins[sensor]), minVals[sensor], maxVals[sensor], 0, 4095);

      // Satura la lectura de los sensores a blanco y negro absoluto
      if (line_sensor_values[sensor] > maxVal) {
        line_sensor_values[sensor] = 4095;
        lineSensors--;
      } else if (line_sensor_values[sensor] < minVal) {
        line_sensor_values[sensor] = 0;
      }

      // Invierte los valores de forma que las lecturas sean en base a detectar línea.
      line_sensor_values[sensor] = 4095 - line_sensor_values[sensor];

    }

    /////////////////////////
    // Cálculo de Posición //
    /////////////////////////

    // Suma de todos los valores de los sensores pesados
    unsigned long linea;
    // Suma de todos los valores de los sensores sin pesar
    unsigned long valores;
    // Posición real en la que se encuentra el robot. Por defecto es la anterior.
    double posicion_real;

    linea = 0;
    valores = 0;
    posicion_real = 0;

    for (int sensor = 0; sensor < NUM_SENSORS; sensor++) {
      linea += (sensor + 1) * line_sensor_values[sensor] * 1000L;
      valores += (long)line_sensor_values[sensor];
    }
    if (lineSensors > 0) {
      // Calcula la posición real sobre la línea.
      posicion_real = ((linea / valores) - ((NUM_SENSORS + 1) * (double)(1000 / 2)));
      return posicion_real;
    } else {
      // Si no hay ningún sensor detectando la linea, simula un sensor extra en el lado adecuado.
      return (last_position > 0) ? (1000 * (NUM_SENSORS + 1) / 2) : -(1000 * (NUM_SENSORS + 1) / 2);
    }
    break;
    case MODO_DEGRADADO:
    break;
  }
}
