//////////////////////////
// INCLUDE de LIBRERIAS //
//////////////////////////
/**
 * Calibración del PID desde la APP android PIDfromBT
 * https://github.com/robotaleh/PIDfromBT
 */
 #include <PIDfromBT.h>

////////////
// MODOS  //
////////////
#define MODO_LINEA        1
#define MODO_DEGRADADO    2

#define LINEA_NEGRA       1
#define LINEA_BLANCA      2

///////////////////
// CONFIGURACION //
///////////////////
#define PISTA             MODO_LINEA
#define LINEA             LINEA_NEGRA
#define TIEMPO_CALIBRADO  5000


//////////////
// SENSORES //
//////////////
#define NUMERO_SENSORES 12
#define SENSOR_1 0
#define SENSOR_2 1
#define SENSOR_3 2
#define SENSOR_4 3
#define SENSOR_5 4
#define SENSOR_6 5
#define SENSOR_7 14
#define SENSOR_8 13
#define SENSOR_9 12
#define SENSOR_10 11
#define SENSOR_11 10
#define SENSOR_12 9
#define SENSOR_FRONTAL PA7
#define SENSOR_DERECHO 7
#define SENSOR_IZQUIERDO 15
#define SENSOR_ROBOTRACER_DERECHO PA4
#define SENSOR_ROBOTRACER_IZQUIERDO PA5

///////////////////
// MULTIPLEXADOR //
///////////////////
#define MUX_LECTURA PB1
#define MUX_CANAL_0 PA12
#define MUX_CANAL_1 PA11
#define MUX_CANAL_2 PB10
#define MUX_CANAL_3 PB11

/////////////
// MOTORES //
/////////////
#define MOTOR_DERECHO_ADELANTE PB15
#define MOTOR_DERECHO_ATRAS PB14
#define MOTOR_DERECHO_PWM PA8
#define MOTOR_IZQUIERDO_ADELANTE PB12
#define MOTOR_IZQUIERDO_ATRAS PB13
#define MOTOR_IZQUIERDO_PWM PB8
#define MOTOR_SUCCION PB9

//////////////
// ENCODERS //
//////////////
#define MOTOR_DERECHO_ENCODER_A PB5
#define MOTOR_DERECHO_ENCODER_B PB4
#define MOTOR_IZQUIERDO_ENCODER_A PB3
#define MOTOR_IZQUIERDO_ENCODER_B PA15

//////////
// LEDS //
//////////
#define RED_RGB_R PA1
#define RED_RGB_G PA2
#define RED_RGB_B PA3

/////////////
// BOTONES //
/////////////
#define BTN PC13
#define BTN_CRUCETA PA6

//////////
// MISC //
//////////
#define CHOP_PIN PB0
#define NIVEL_BATERIA PA0

///////////////
// VARIABLES //
///////////////
int velocidadBase         = 0;
int velocidadMaxima       = 255;
float velocidadDerecha    = 0;
float velocidadIzquierda  = 0;
bool motoresHabilitados   = false;
bool enCompeticion        = false;
bool competicionIniciada  = false;
long ultimaLinea          = 0;

//////////////////////////
// VARIABLES DE CONTROL //
//////////////////////////
int posicionActual = 0;
int posicionIdeal = 0;
float errorAnterior = 0;
float integralErrores = 0;
float kp = 0, ki = 0, kd = 0;

///////////////////////////
// VARIABLES DE SENSORES //
///////////////////////////
int pinesSensores[] = {SENSOR_1, SENSOR_2, SENSOR_3, SENSOR_4, SENSOR_5, SENSOR_6, SENSOR_7, SENSOR_8, SENSOR_9, SENSOR_10, SENSOR_11, SENSOR_12};
int posicionMaxima = 7000;
int posiciconMinima = -7000;
int valoresSensores[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int valorSaturacionBajo = 40;
int valorSaturacionAlto = 3960;

///////////////////////////////
// VARIABLES DE CALIBRACIÓN  //
///////////////////////////////
int valoresCalibracionMinimos[] = {4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096};
int valoresCalibracionMaximos[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const int valorCalibradoMinimo = 0;
const int valorCalibradoMaximo = 4000;

//////////////////////////////
// INICIALIZACION LIBRERIAS //
//////////////////////////////
PIDfromBT CalibracionPID(&kp, &ki, &kd, &velocidadBase, &posicionIdeal, DEBUG);

void setup(){

}

void loop(){

}
