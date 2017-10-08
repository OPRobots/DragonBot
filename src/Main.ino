///////////////////
// PINES MOTORES //
///////////////////
#define MOTOR_IZQUIERDO_ADELANTE PB12
#define MOTOR_IZQUIERDO_ATRAS PB13
#define MOTOR_IZQUIERDO_PWM PB1
#define MOTOR_DERECHO_ADELANTE PB14
#define MOTOR_DERECHO_ATRAS PB15
#define MOTOR_DERECHO_PWM PA8
#define MOTOR_RUN PB10
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
#define LED PB0

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

//////////////////////
// SENSORES QRE1113 //
//////////////////////
const short NUM_SENSORS =  7;
short sensorPins[] = {SENSOR_1, SENSOR_2, SENSOR_3, SENSOR_4, SENSOR_5, SENSOR_6, SENSOR_7};


/////////////
//CIRCUITO //
/////////////
#define MODO_LINEA 1
#define MODO_DEGRADADO 2

// Establece el tipo de circuito
#define MODO_CIRCUITO MODO_LINEA


//////////////////////////
//VARIABLES DE LECTURAS //
//////////////////////////

int line_sensor_values[7];






void setup() {
  // Arranca todos los componentes
  init_all();
  delay(500);
}

void loop() {
}
