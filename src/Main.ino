//////////////////////////
// INCLUDE de LIBRERIAS //
//////////////////////////
/**
 * Calibración del PID desde la APP android PIDfromBT
 * https://github.com/robotaleh/PIDfromBT
 */
#include <PIDfromBT.h>
#include <Servo.h>
#include <Wire.h>
#include <MegunoLink.h>
#include <Filter.h>

////////////
// MODOS  //
////////////
#define MODO_LINEA        1
#define MODO_DEGRADADO    2

#define LINEA_NEGRA       1
#define LINEA_BLANCA      2

#define LIPO_2S           1
#define LIPO_3S           2

///////////////////
// CONFIGURACION //
///////////////////
#define PISTA             MODO_LINEA
#define LINEA             LINEA_NEGRA
#define LIPO              LIPO_2S
#define TIEMPO_CALIBRADO  5000


//////////////
// SENSORES //
//////////////
#define NUMERO_SENSORES 12
#define TIEMPO_SIN_PISTA 100
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
#define COMPENSACION_DERECHO 0
#define COMPENSACION_IZQUIERDO 14

//////////////
// ENCODERS //
//////////////
#define MOTOR_DERECHO_ENCODER_A PB5
#define MOTOR_DERECHO_ENCODER_B PB4
#define MOTOR_IZQUIERDO_ENCODER_A PA15
#define MOTOR_IZQUIERDO_ENCODER_B PB3

//////////////
// MPU9250  //
//////////////
#define    MPU9250_ADDRESS            0x68
#define    MAG_ADDRESS                0x0C

#define    GYRO_FULL_SCALE_250_DPS    0x00
#define    GYRO_FULL_SCALE_500_DPS    0x08
#define    GYRO_FULL_SCALE_1000_DPS   0x10
#define    GYRO_FULL_SCALE_2000_DPS   0x18

#define    ACC_FULL_SCALE_2_G         0x00
#define    ACC_FULL_SCALE_4_G         0x08
#define    ACC_FULL_SCALE_8_G         0x10
#define    ACC_FULL_SCALE_16_G        0x18

//////////
// LEDS //
//////////
#define RED_RGB_R PA1
#define RED_RGB_G PA2
#define RED_RGB_B PA3

/////////////
// BOTONES //
/////////////
#define BTN         PC13
#define BTN_CRUCETA PA6

//////////
// MISC //
//////////
#define CHOP_PIN      PB0
#define NIVEL_BATERIA PA0

///////////////
// VARIABLES //
///////////////
int velocidadBase         = 0;
int velocidadMaxima       = 255;
float velocidadDerecha    = 0;
float velocidadIzquierda  = 0;
long ultimaLinea          = 0;
long ultimaBateria        = 0;
bool avisoBateria         = false;
int intervaloAviso        = 500;
int velocidadSuccion      = 0;

//////////////////////////
// VARIABLES DE CONTROL //
//////////////////////////
int posicionActual        = 0;
int posicionIdeal         = 0;
float errorAnterior       = 0;
float integralErrores     = 0;
float kp                  = 0;
float ki                  = 0;
float kd                  = 0;
int correccion            = 0;
long ultimoControlBrushless = 0;

////////////////////////////
// VARIABLES DE ENCODERS  //
////////////////////////////
float ticksMm                           = 0.6819f;
float velocidadActual                   = 0;
long ultimaVelocidad                    = 0;
volatile bool estadoEncoderDerechoA     = false;
volatile bool estadoEncoderDerechoB     = false;
volatile bool estadoEncoderIzquierdoA   = false;
volatile bool estadoEncoderIzquierdoB   = false;
long ticksDerechoAnteriores             = 0;
long ticksIzquierdoAnteriores           = 0;
volatile long ticksDerecho              = 0;
volatile long ticksIzquierdo            = 0;
int contMapeo                           = 5;

//////////////////////
// VARIABLES MAPEO  //
//////////////////////
#define NUMERO_SECTORES       2
#define TIPO_SECTOR_RECTA     1
#define TIPO_SECTOR_CURVA     2
#define SECTOR_TICKS          0
#define SECTOR_TIPO           1
int tipoSector                = TIPO_SECTOR_RECTA;
int sectorActual              = 0;
bool mapeoRealizado           = false;
bool mapeoIniciado            = false;
int sectoresPista[NUMERO_SECTORES][2];
long ticksMapeoDerechoAnteriores      = 0;
long ticksMapeoIzquierdoAnteriores    = 0;
bool ticksReseteados                  = true;

///////////////////////////
// VARIABLES DE SENSORES //
///////////////////////////
int pinesSensores[]         = {SENSOR_1, SENSOR_2, SENSOR_3, SENSOR_4, SENSOR_5, SENSOR_6, SENSOR_7, SENSOR_8, SENSOR_9, SENSOR_10, SENSOR_11, SENSOR_12};
int posicionMaxima          = 6500;
int posicionMinima          = -6500;
int valoresSensores[]       = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int valorSaturacionBajo     = 80;
int valorSaturacionAlto     = 3000;

///////////////////////////////
// VARIABLES DE CALIBRACIÓN  //
///////////////////////////////
int valoresCalibracionMinimos[]   = {4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096};
int valoresCalibracionMaximos[]   = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const int valorCalibradoMinimo    = 0;
const int valorCalibradoMaximo    = 4000;

///////////////////////////////
// VARIABLES DE COMPETICIÓN  //
///////////////////////////////
bool enCompeticion        = false;
bool competicionIniciada  = false;

//////////////////////////////
// INICIALIZACION LIBRERIAS //
//////////////////////////////
Servo Brushless;
HardwareTimer TimerPID(2);
HardwareTimer TimerBrushless(3);
PIDfromBT CalibracionPID(&kp, &ki, &kd, &velocidadBase, &posicionIdeal, &velocidadSuccion, DEBUG);
ExponentialFilter<long> filtroBateria(15, 0);
ExponentialFilter<long> filtroMapeo(50, 0);

void setup(){
    inicia_todo();
    enCompeticion = btn_pulsado();
    nivel_bateria(false);
    calibra_sensores();
    inicia_timers();
    delay(100);
}

void loop(){
  CalibracionPID.update();
  delay(20);
  if(!enCompeticion || (enCompeticion && competicionIniciada)){
    if(!enCompeticion){
      if(btn_pulsado()){
        Serial.println(analogRead(NIVEL_BATERIA));
      }
    }
  }else if(enCompeticion){
    if(!btn_pulsado()){
      delay(100);
      if(btn_pulsado()){
        // TODO: Asignación de configuraciones.
        kp = 0.03f;
        kd = 4.0f;
        while(btn_pulsado()){
          set_color_RGB(random(200,255),0,0);
        }
        long millisIniciales = millis();
        int tiempoPasado = 5;
        while(millis()<(millisIniciales + 5000)){
            tiempoPasado = millis()-millisIniciales;
            byte r = 0, g =0;
            r = map(tiempoPasado, 0,5000, 255,0);
            g = map(tiempoPasado, 0,1000, 0,255);
            set_color_RGB(r,g,0);
        }
        ticksDerecho = 0;
        ticksIzquierdo = 0;
        competicionIniciada = true;
        set_color_RGB(0,0,0);
        velocidadBase = 70;
      }
    }
  }
}
