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
#define SENSOR_FRONTAL_XSLEEP PB4
#define SENSOR_DERECHO_XSLEEP PB3

//////////////////
// SENSORES I2C //
//////////////////
#include <Wire.h>
#include <VL53L0X.h>
const short NUM_ROBOT_SENSORS = 3;
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
double position = 0;
int robots_sensor_values[] = {0,0,0};

// VARIABLES DE VELOCIDADES
int velBase = 0;
int velVentilador = 0;
int velReal = 0;
short vel_ini = 0;
long millis_inicial_accel = 0;
float m;
bool cambiando_vel = false;
bool competicion = false;
bool competicion_iniciada = false;
long millis_stop = 0;
bool stop_emergencia = false;
#include "MegunoLink.h"
#include "Filter.h"
ExponentialFilter<long> filtro_sensor_izquierdo(5,0);
ExponentialFilter<long> filtro_sensor_frontal(5,0);
ExponentialFilter<long> filtro_sensor_derecho(5,0);
// ATDelay lectura_sensor_robot(500);

// VARIABLES DE PID
#include <PIDfromBT.h>
double ideal = 0;
long last_pid_calc = 0;
double last_error = 0;
double sum_error = 0;
double kp = 0, ki = 0, kd = 0;
PIDfromBT pid_calibrate(&kp, &ki, &kd, &velBase, &ideal, MIN_IDEAL, MAX_IDEAL, DEBUG);
ExponentialFilter<long> filtroDegradado(30, 0);

volatile long ticks_derecho = 0;
volatile long ticks_izquierdo = 0;

void setup() {
  init_all();
  if (!digitalRead(BTN)) {
		competicion = true;
	}
  delay(100);
  calibrate_line_sensors((bool)REALIZAR_CALIBRACION);
  delay(2000);
}

void loop() {
  if (!competicion || (competicion && competicion_iniciada)) {
  		if (!competicion) {
  			pid_calibrate.update();
        velVentilador = ki;
  		}
      pid_calibrate.update();
      position = get_position(position);
      set_speed(calc_PID(position));
      analogWrite(MOTOR_VENTILADOR, velVentilador);
  	} else if (competicion) {
  		if (digitalRead(BTN)) {
  			delay(100);
  			if (!digitalRead(BTN)) {
          set_settings(!digitalRead(SW_1), !digitalRead(SW_2));
  				while (!digitalRead(BTN)) {
  					digitalWrite(LED, LOW);
  				}
  				long millis_pre_start = millis();
  				bool led_state = true;
  				while (millis() < (millis_pre_start + 5000)) {
  					if ((millis() - millis_pre_start) % 500 == 0) {
  						led_state = !led_state;
  						digitalWrite(LED, led_state);
  					}
  				}
  				digitalWrite(LED, LOW);
  				competicion_iniciada = true;
  			}
  		}
  	}
}

void set_settings(bool sw1, bool sw2){
    // Configuración normal super segura
    if(!sw1 && !sw2){
      kp = 6;
      kd = 1200;
      velBase = 120;
      velVentilador = 190;
    }
    // Configuración rapida segura
    if(sw1 && !sw2){
      kp = 6;
      kd = 1200;
      velBase = 150;
      velVentilador = 230;
    }
    // Configuración rapida arriesgada
    if(sw1 && sw2){
      kp = 7;
      kd = 1500;
      velBase = 180;
      velVentilador = 250;
    }
    // // Configuración rapida super arriesgada
    // if(sw1 && sw2){
    //   kp = 7;
    //   kd = 400;
    //   velBase = 200;
    //   velVentilador = 255;
    //   // aceleraciones(?)
    // }
}
