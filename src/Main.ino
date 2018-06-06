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
#include <Wire.h>

////////////
// MODOS  //
////////////
#define MODO_LINEA 1
#define MODO_DEGRADADO 2

#define LINEA_NEGRA 1
#define LINEA_BLANCA 2

#define LIPO_2S 1
#define LIPO_3S 2

#define CALIBRADO_MAXIMO_SENSORES_LINEA 4000
#define CALIBRADO_MINIMO_SENSORES_LINEA 0
#define SATURACION_MAXIMO_SENSORES_LINEA 3000
#define SATURACION_MINIMO_SENSORES_LINEA 1000

#define CALIBRADO_MAXIMO_SENSORES_DEGRADADO 4000
#define CALIBRADO_MINIMO_SENSORES_DEGRADADO 0
#define SATURACION_MAXIMO_SENSORES_DEGRADADO 4000
#define SATURACION_MINIMO_SENSORES_DEGRADADO 5

///////////////////
// CONFIGURACION //
///////////////////
#define MILLIS_INICIO 5000
#define PISTA MODO_LINEA
#define LINEA LINEA_NEGRA
#define LIPO LIPO_2S
#define TIEMPO_CALIBRADO 5000
#define NUMERO_SECTORES 2
#define CALIBRAR_SENSORES true
#define MAPEO_CIRCUITO false

//////////////
// SENSORES //
//////////////
#define NUMERO_SENSORES 12
#define NUMERO_SENSORES_LATERALES 2
#define TIEMPO_SIN_PISTA 150
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
#define MOTOR_IZQUIERDO_ADELANTE PB13
#define MOTOR_IZQUIERDO_ATRAS PB12
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
#define MPU9250_ADDRESS 0x68
#define MAG_ADDRESS 0x0C

#define GYRO_FULL_SCALE_250_DPS 0x00
#define GYRO_FULL_SCALE_500_DPS 0x08
#define GYRO_FULL_SCALE_1000_DPS 0x10
#define GYRO_FULL_SCALE_2000_DPS 0x18

#define ACC_FULL_SCALE_2_G 0x00
#define ACC_FULL_SCALE_4_G 0x08
#define ACC_FULL_SCALE_8_G 0x10
#define ACC_FULL_SCALE_16_G 0x18

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
int velocidad = 0;
int velocidadSuccion = 0;
int velocidadMaxima = 255;
float velocidadDerecha = 0;
float velocidadIzquierda = 0;
long ultimaLinea = 0;
long ultimaBateria = 0;
bool avisoBateria = false;
int intervaloAviso = 500;
int velocidadBase = 70;
int velocidadCurvas = 80;
int velocidadRectas = 120;
int velocidadSuccionBase = 50;
int velocidadSuccionCurvas = 100;
int velocidadSuccionRectas = 30;

//////////////////////////
// VARIABLES DE CONTROL //
//////////////////////////
int posicionActual = 0;
int posicionIdeal = 0;
int posicionIdealObjetivo;
int posicionIdealStep = 0;
float errorAnterior = 0;
float errorFrontalAnterior = 0;
float integralErrores = 0;
float kp = 0.03f;
float ki = 0;
float kd = 4.0f;
float kpFrontal = 0.15f;
float kiFrontal = 0;
float kdFrontal = 4.0f;
int correccion = 0;
int correccionFrontal = 0;
long ultimoControlBrushless = 0;
int lineaPrincipal[] = {-1, -1};
bool mantenerCorreccion = false;
int valoresSensoresLaterales[NUMERO_SENSORES_LATERALES];
long ultimaDeteccionLateral[NUMERO_SENSORES_LATERALES];
long ultimaDeteccionFrontal = 0;
bool cambioCarril = false;
long ultimoCambioCarril;

////////////////////////////
// VARIABLES DE ENCODERS  //
////////////////////////////
float ticksMm = 1.444f;
float velocidadMs = 0;
long ultimaVelocidad = 0;
long ticksDerechoAnteriores = 0;
long ticksIzquierdoAnteriores = 0;
volatile long ticksDerecho = 0;
volatile long ticksIzquierdo = 0;
int contMapeo = 5;

//////////////////////
// VARIABLES MAPEO  //
//////////////////////
#define TIPO_SECTOR_RECTA 1
#define TIPO_SECTOR_CURVA 2
#define SECTOR_TICKS 0
#define SECTOR_TIPO 1
int tipoSector = TIPO_SECTOR_RECTA;
int sectorActual = 0;
bool mapeoRealizado = false;
bool mapeoIniciado = false;
int sectoresPista[NUMERO_SECTORES][2];
long ticksMapeoDerechoAnteriores = 0;
long ticksMapeoIzquierdoAnteriores = 0;
bool ticksReseteados = true;
int mediaDiferenciaRecta = 0;

///////////////////////////
// VARIABLES DE SENSORES //
///////////////////////////
int valorSaturacionBajo;
int valorSaturacionAlto;
int pinesSensores[] = {SENSOR_1, SENSOR_2, SENSOR_3, SENSOR_4, SENSOR_5, SENSOR_6, SENSOR_7, SENSOR_8, SENSOR_9, SENSOR_10, SENSOR_11, SENSOR_12};
int pinesSensoresLaterales[] = {SENSOR_IZQUIERDO, SENSOR_DERECHO};
int posicionMaxima = 6500;
int posicionMinima = -6500;
int posicionDegradadoMaxima = 0;
int valoresSensores[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int valoresSensoresRaw[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
#define SENSOR_FRONTAL_IDEAL 750;
int valorSensorFrontal = 0;
int inicioLineaAnterior = 0;
int finLineaAnterior = 0;
#define NUMERO_CALIBRACIONES_DEGRADADO 21
int calibradoDegradado[NUMERO_CALIBRACIONES_DEGRADADO][NUMERO_SENSORES];

///////////////////////////////
// VARIABLES DE CALIBRACIÓN  //
///////////////////////////////
int valoresCalibracionMinimos[] = {4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096};
int valoresCalibracionMaximos[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int valorCalibradoMaximo;
int valorCalibradoMinimo;
// int valoresCalibracionMaximos[] = {2263, 2429, 2312, 2333, 2399, 2592, 2257, 2385, 2912, 3512, 3471, 3972};
// int valoresCalibracionMinimos[] = {197, 201, 199, 198, 201, 204, 196, 202, 202, 208, 195, 207};

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

#define MENU_PRINCIPAL 0
#define MENU_PID 1
#define MENU_VELOCIDAD 2
#define MENU_SUCCION 3
#define MENU_PISTA 4
#define MENU_FINAL 5

#define NUMERO_MENU_PRINCIPAL 4
#define NUMERO_MENU_PID 3
#define NUMERO_MENU_VELOCIDAD 3
#define NUMERO_MENU_SUCCION 3
#define NUMERO_MENU_PISTA 1
#define NUMERO_MENU_MAXIMO 4

int menuActual = MENU_PRINCIPAL;
int menuSeleccion[NUMERO_MENU_PRINCIPAL + 1];
bool modoMenu = false;
bool menuSeleccionVolver = false;
bool menuSeleccionModificar = false;

String menuPrincipal[] = {"PID", "Velocidad", "Succion", "Pista"};
String menuPID[] = {"Proporcional", "Integral", "Derivativa"};
float *menuConfigPID[] = {&kp, &ki, &kd};
float menuConfigCambioPID[] = {0.001f, 0.001f, 0.05f};
String menuVelocidad[] = {"Base", "Rectas", "Curvas"};
int *menuConfigVelocidad[] = {&velocidadBase, &velocidadCurvas, &velocidadRectas};
int menuConfigCambioVelocidad[] = {5, 5, 5};
String menuSuccion[] = {"Base", "Rectas", "Curvas"};
int *menuConfigSuccion[] = {&velocidadSuccionBase, &velocidadSuccionCurvas, &velocidadSuccionRectas};
int menuConfigCambioSuccion[] = {5, 5, 5};
String menuPista[] = {"Sectores"};
int menuConfigPista[] = {NUMERO_SECTORES};
int menuConfigCambioPista[] = {0, 5, 5};

//////////////////////////////
// INICIALIZACION LIBRERIAS //
//////////////////////////////
HardwareTimer TimerPID(2);
HardwareTimer TimerBrushless(3);
PIDfromBT CalibracionPID(&kp, &ki, &kd, &velocidad, &posicionIdeal, &velocidadSuccion, DEBUG);
ExponentialFilter<long> filtroBateria(15, 0);
ExponentialFilter<long> filtroMapeo(50, 0);
ExponentialFilter<long> filtroPosicion(12, 0);

void setup() {
  inicia_todo();
  nivel_bateria(false);
  calibra_sensores();
  delay(100);
  inicia_timers();
  delay(100);
}
long tiempo = 0;
int numeroSensoresPista;
void loop() {

  ////////////////////////////////////////
  // LOG DE SENSORES Y CÁLCULO DE LÍNEA //
  ////////////////////////////////////////

  /*  for (int sensor = 0; sensor < NUMERO_SENSORES; sensor++) {
    // int lectura = mux_analog_read(pinesSensores[sensor]);
    Serial.print(valoresSensores[sensor]);
    // Serial.print(" (");
    // Serial.print(valoresSensores[sensor]);
    // Serial.print(")");
    Serial.print("\t");
  }
  Serial.print("=>");
  // posicionActual = calcular_posicion(posicionActual);
  Serial.print(posicionActual);
    Serial.print("\t(");
    Serial.print(lineaPrincipal[0]);
    Serial.print(" - ");
    Serial.print(lineaPrincipal[1]);
    Serial.print(" - ");
    Serial.print(sensoresDetectando);
    Serial.print(")\n");
  delay(100); */

  ////////////////////////////////////////////
  // LOG DE SENSORES Y CÁLCULO DE DEGRADADO //
  ////////////////////////////////////////////
  // lectura_sensores_calibrados();
   for (int sensor = 0; sensor < NUMERO_SENSORES; sensor++) {
    Serial.print(valoresSensores[sensor]);
    Serial.print("\t");
  }
  // Serial.print(posicionIdealObjetivo);
  // Serial.print("\t");
  // Serial.print(posicionIdealStep);
  // Serial.print(posicionIdeal);
  // Serial.print("\t");
  // Serial.print(numeroSensoresPista);
  // Serial.print("\t");
  // Serial.print(posicionActual);
  // Serial.print("\t");
  // Serial.print(correccion);
  // Serial.print("\n");
  // Serial.print(valoresSensoresLaterales[0]);
  // Serial.print("\t");
  // Serial.print(valoresSensoresLaterales[1]);
  // Serial.print("\t");
  // Serial.print(valorSensorFrontal);
  Serial.print("\n");
  // //   // Serial.println(analogRead(SENSOR_FRONTAL));
  delay(100);

  // //   // set_color_RGB(0, 0, 255);

  return;

  // CalibracionPID.update();
  delay(100);
  if (!competicionIniciada) {
    btn_cruceta_simple();
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
          if ((tiempoPasado > MILLIS_INICIO * 0.75f || MILLIS_INICIO == 0) && velocidadSuccion == 0) {
            velocidadSuccion = velocidadSuccionBase;
          }
        }
        ticksDerecho = 0;
        ticksIzquierdo = 0;
        ticksMapeoDerechoAnteriores = 0;
        ticksMapeoIzquierdoAnteriores = 0;
        competicionIniciada = true;
        set_color_RGB(0, 0, 0);
        velocidad = velocidadBase;
      }
    }
  }
}
