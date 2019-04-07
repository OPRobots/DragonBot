//////////////////////////
// INCLUDE de LIBRERIAS //
//////////////////////////
/**
 * Calibración del PID desde la APP android PIDfromBT
 * https://github.com/robotaleh/PIDfromBT
 */
#include <Filter.h>
#include <MegunoLink.h>
#include <PIDfromBT.h>

////////////////////////
// SELECTOR DE ROBOT  //
////////////////////////
// #define DRAGON_A 1
#define DRAGON_B 1

#define MORRO_ANCHO 1
// #define MORRO_ESTRECHO 1

////////////
// MODOS  //
////////////
#define MODO_LINEA 1

#define LINEA_NEGRA 1
#define LINEA_BLANCA 2

#define LIPO_2S 1
#define LIPO_3S 2

#define MODO_MS 1
#define MODO_PWM 2

///////////////////
// CONFIGURACION //
///////////////////
#define MILLIS_INICIO 5000
#define PISTA MODO_LINEA
#define LINEA LINEA_NEGRA
#define LIPO LIPO_2S
#define VELOCIDAD MODO_PWM
#define TIEMPO_CALIBRADO 2500
#define CALIBRAR_SENSORES true

//////////////
// SENSORES //
//////////////
#define TIEMPO_SIN_PISTA 200

#ifdef MORRO_ANCHO
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

#elif MORRO_ESTRECHO
#define NUMERO_SENSORES 14
#define SENSOR_1 0
#define SENSOR_2 1
#define SENSOR_3 2
#define SENSOR_4 3
#define SENSOR_5 4
#define SENSOR_6 5
#define SENSOR_7 7
#define SENSOR_8 15
#define SENSOR_9 14
#define SENSOR_10 13
#define SENSOR_11 12
#define SENSOR_12 11
#define SENSOR_13 10
#define SENSOR_14 9
#endif
#define SENSOR_ROBOTRACER_DERECHO PA4
#define SENSOR_ROBOTRACER_IZQUIERDO PA5

#define LECTURA_MAXIMO_SENSORES_LINEA 4096
#define LECTURA_MINIMO_SENSORES_LINEA 0
#define CALIBRADO_MAXIMO_SENSORES_LINEA 2000
#define CALIBRADO_MINIMO_SENSORES_LINEA 0

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
#ifdef DRAGON_A
#define MOTOR_DERECHO_ADELANTE PB15
#define MOTOR_DERECHO_ATRAS PB14
#define MOTOR_IZQUIERDO_ADELANTE PB12
#define MOTOR_IZQUIERDO_ATRAS PB13
#endif

#ifdef DRAGON_B
#define MOTOR_DERECHO_ADELANTE PB14
#define MOTOR_DERECHO_ATRAS PB15
#define MOTOR_IZQUIERDO_ADELANTE PB12
#define MOTOR_IZQUIERDO_ATRAS PB13
#endif

#define MOTOR_DERECHO_PWM PA8
#define MOTOR_IZQUIERDO_PWM PB8
#define MOTOR_SUCCION PB9

//////////////
// ENCODERS //
//////////////
#define MOTOR_DERECHO_ENCODER_A PB5
#define MOTOR_DERECHO_ENCODER_B PB4
#define MOTOR_IZQUIERDO_ENCODER_A PA15
#define MOTOR_IZQUIERDO_ENCODER_B PB3

//////////
// LEDS //
//////////
#define RED_RGB_R PA1
#define RED_RGB_G PA2
#define RED_RGB_B PA3

///////////////////////////////////
// VARIABLES DE CONTROL DE LEDS  //
///////////////////////////////////
int colorRGB[] = {255, 0, 0};
int colorDesc = 0;
int colorAsc = 1;
long ultimoCambioRGB = 0;

/////////////
// BOTONES //
/////////////
#define BTN PC13
#define BTN_CRUCETA PA6

//////////
// MISC //
//////////
#define NIVEL_BATERIA PA0

///////////////
// VARIABLES //
///////////////
float anguloGiro = 0;
float anguloGiroR = 0;
float velocidadW = 0;
float velocidadMs = 0;
float posXm = 0;
float posYm = 0;
int velocidad = 0;
float velocidadMsIdeal = 0;
float velocidadMsIdealBase = 0;
int velocidadPercentBase = 0;
int velocidadSuccion = 0;
int velocidadSuccionBase = 50;
int velocidadMaxima = 255;
long ultimaLinea = 0;
long ultimaBateria = 0;
bool avisoBateria = false;
int intervaloAvisoBateria = 500;
long millisInitESC = -1;
bool ESCIniciado = false;

//////////////////////////
// VARIABLES DE CONTROL //
//////////////////////////
int posicionActual = 0;
int posicionIdeal = 0;
int posicionIdealObjetivo;
float errorAnterior = 0;
float integralErrores = 0;
float kp;
float ki;
float kd;
float kpVelocidad = 5;
float kdVelocidad = 10;
float ultimoErrorVelocidad = 0;
int correccion = 0;

////////////////////////////
// VARIABLES DE ENCODERS  //
////////////////////////////
#define ENCODER_PPR 119
#define RUEDAS_RADIO 0.01275
float ticksMm = 1.444f;
long ticksDerechoAnteriores = 0;
long ticksIzquierdoAnteriores = 0;
volatile long ticksDerecho = 0;
volatile long ticksIzquierdo = 0;

///////////////////////////
// VARIABLES DE SENSORES //
///////////////////////////
#ifdef MORRO_ANCHO
int pinesSensores[] = {SENSOR_1, SENSOR_2, SENSOR_3, SENSOR_4, SENSOR_5, SENSOR_6, SENSOR_7, SENSOR_8, SENSOR_9, SENSOR_10, SENSOR_11, SENSOR_12};
int valoresSensores[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int valoresSensoresRaw[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
#elif MORRO_ESTRECHO
int pinesSensores[] = {SENSOR_1, SENSOR_2, SENSOR_3, SENSOR_4, SENSOR_5, SENSOR_6, SENSOR_7, SENSOR_8, SENSOR_9, SENSOR_10, SENSOR_11, SENSOR_12, SENSOR_13, SENSOR_14};
int valoresSensores[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int valoresSensoresRaw[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
#endif

///////////////////////////////
// VARIABLES DE CALIBRACIÓN  //
///////////////////////////////
#ifdef MORRO_ANCHO
int valoresCalibracionMinimos[] = {4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096};
int valoresCalibracionMaximos[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
#elif MORRO_ESTRECHO
int valoresCalibracionMinimos[] = {4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096};
int valoresCalibracionMaximos[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
#endif
int umbralesCalibracionSensores[NUMERO_SENSORES];
int valorCalibradoMaximo;
int valorCalibradoMinimo;
int posicionMaxima;
int posicionMinima;

///////////////////////////////
// VARIABLES DE COMPETICIÓN  //
///////////////////////////////
bool competicionIniciada = false;

/////////////////////////////////////////
// VARIABLES DE MENÚ DE CONFIGURACIÓN  //
/////////////////////////////////////////
#define NUMERO_COMBINACIONES 10
#define CRUCETA_ARRIBA 1460
#define CRUCETA_ABAJO 475
#define CRUCETA_DERECHA 860
#define CRUCETA_IZQUIERDA 2040
#define CRUCETA_ARRIBA_ABAJO 1670
#define CRUCETA_DERECHA_IZQUIERDA 2280
#define CRUCETA_ARRIBA_DERECHA 1840
#define CRUCETA_DERECHA_ABAJO 1160
#define CRUCETA_ABAJO_IZQUIERDA 2170
#define CRUCETA_IZQUIERDA_ARRIBA 2490
int crucetaCombinaciones[] = {CRUCETA_ARRIBA,
                              CRUCETA_ABAJO,
                              CRUCETA_DERECHA,
                              CRUCETA_IZQUIERDA,
                              CRUCETA_ARRIBA_ABAJO,
                              CRUCETA_DERECHA_IZQUIERDA,
                              CRUCETA_ARRIBA_DERECHA,
                              CRUCETA_DERECHA_ABAJO,
                              CRUCETA_ABAJO_IZQUIERDA,
                              CRUCETA_IZQUIERDA_ARRIBA};
#define NUMERO_VELOCIDADES 9
int velocidad_menu = 0;
#define NUMERO_VELOCIDADES_SUCCION 9
int velocidad_succion_menu = 0;

//////////////////////////////
// INICIALIZACION LIBRERIAS //
//////////////////////////////
HardwareTimer TimerPID(2);
bool timerPID_pause = false;
HardwareTimer TimerBrushless(3);
PIDfromBT CalibracionPID(&kp, &ki, &kd, &velocidad, &posicionIdeal, &velocidadSuccion, DEBUG);
ExponentialFilter<long> filtroBateria(15, 0);

void setup() {
  inicia_todo();
  inicia_timer_Brushless();
  nivel_bateria(false);
  calibra_sensores();
  delay(100);
}

void loop() {
  // CalibracionPID.update();
  if (!competicionIniciada) {
    btn_cruceta();
    if (!btn_pulsado()) {
      delay(100);
      if (btn_pulsado()) {
        set_color_RGB(255, 0, 0);
        while (btn_pulsado()) {
        }
        long millisIniciales = millis();
        int tiempoPasado = 5;
        while (millis() < (millisIniciales + MILLIS_INICIO)) {
          tiempoPasado = millis() - millisIniciales;
          byte r = 0, g = 0;
          r = map(tiempoPasado, 0, MILLIS_INICIO, 255, 0);
          g = map(tiempoPasado, 0, 1000, 0, 255);
          set_color_RGB(r, g, 0);
          if ((tiempoPasado > MILLIS_INICIO * 0.75f || MILLIS_INICIO == 0) && velocidadSuccion == 0 && velocidadSuccionBase > 0) {
            velocidadSuccion = 50;
          }
        }
        ticksDerecho = 0;
        ticksIzquierdo = 0;
        ticksDerechoAnteriores = 0;
        ticksIzquierdoAnteriores = 0;
        velocidadMs = 0;
        inicia_timer_PID();
        competicionIniciada = true;
        set_color_RGB(0, 0, 0);
        if (VELOCIDAD == MODO_MS) {
          velocidadMsIdeal = velocidadMsIdealBase;
        } else {
          velocidad = map(velocidadPercentBase, 0, 100, 0, 255);
        }
        velocidadSuccion = velocidadSuccionBase;
      }
    }
  }
}
