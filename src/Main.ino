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
#define SATURACION_MINIMO_SENSORES_LINEA 2000

#define CALIBRADO_MAXIMO_SENSORES_DEGRADADO 2000
#define CALIBRADO_MINIMO_SENSORES_DEGRADADO 0
#define SATURACION_MAXIMO_SENSORES_DEGRADADO 2000
#define SATURACION_MINIMO_SENSORES_DEGRADADO 5

///////////////////
// CONFIGURACION //
///////////////////
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
int lineaPrincipal[] = {-1,-1};

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
int posicionMaxima = 6500;
int posicionMinima = -6500;
int posicionDegradadoMaxima = 0;
int valoresSensores[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int valoresSensoresRaw[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
#define SENSOR_FRONTAL_IDEAL 800;
int valorSensorFrontal = 0;
int inicioLineaAnterior = 0;
int finLineaAnterior = 0;
#define NUMERO_CALIBRACIONES_DEGRADADO 21
int calibradoDegradado[NUMERO_CALIBRACIONES_DEGRADADO][NUMERO_SENSORES] = {
    {3649, 3628, 3883, 3736, 3814, 3893, 3768, 3785, 4000, 3732, 3448, 3468},
    {3530, 3781, 3790, 3803, 3866, 3831, 3678, 3835, 3990, 3828, 3738, 3477},
    {3834, 3664, 3789, 3804, 3770, 3782, 3693, 3800, 3949, 3866, 3666, 3398},
    {3510, 3527, 3682, 3690, 3636, 3582, 3632, 3906, 3980, 3765, 3528, 3340},
    {3385, 3539, 3596, 3667, 3680, 3585, 3600, 3669, 3865, 3589, 3364, 3239},
    {3285, 3270, 3447, 3396, 3391, 3424, 3406, 3389, 3725, 3350, 3185, 3189},
    {3085, 3144, 3188, 3100, 3249, 3257, 3213, 3226, 3285, 3073, 3103, 3100},
    {2860, 2904, 2811, 2886, 2956, 3016, 2937, 3021, 3121, 2829, 2843, 2963},
    {2629, 2513, 2525, 2558, 2774, 2711, 2609, 2628, 2696, 2458, 2539, 2688},
    {2275, 2314, 2336, 2318, 2270, 2467, 2415, 2460, 2238, 2357, 2433, 2421},
    {2164, 2034, 2063, 1879, 2226, 2118, 2111, 1995, 2193, 1942, 2101, 2165},
    {1846, 1791, 1875, 1710, 1905, 1973, 1753, 1856, 1914, 1742, 1822, 1873},
    {1555, 1508, 1621, 1458, 1622, 1759, 1592, 1615, 1670, 1488, 1491, 1629},
    {1373, 1235, 1412, 1302, 1446, 1473, 1301, 1266, 1126, 1206, 1306, 1212},
    {1094, 1031, 1116, 1140, 1269, 1224, 1114, 1104, 1042, 997, 1078, 1066},
    {834, 828, 941, 602, 793, 1041, 908, 1010, 905, 764, 843, 1043},
    {748, 553, 623, 578, 646, 678, 701, 766, 964, 577, 687, 835},
    {618, 412, 417, 158, 394, 553, 361, 609, 871, 486, 447, 642},
    {326, 330, 256, 0, 372, 387, 254, 431, 357, 317, 402, 389},
    {152, 121, 132, 0, 0, 198, 238, 190, 167, 0, 141, 360},
    {0, 0, 248, 0, 225, 498, 379, 211, 285, 254, 148, 156},
};

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

void setup() {
  inicia_todo();
  nivel_bateria(false);
  calibra_sensores();
  delay(100);
  inicia_timers();
  delay(100);
}

void loop() {

  /*
  ////////////////////////////////////////
  // LOG DE SENSORES Y CÁLCULO DE LÍNEA //
  ////////////////////////////////////////

   for (int sensor = 0; sensor < NUMERO_SENSORES; sensor++) {
    int lectura = mux_analog_read(pinesSensores[sensor]);
    Serial.print(lectura);
    Serial.print(" (");
    Serial.print(map(lectura, valoresCalibracionMinimos[sensor], valoresCalibracionMaximos[sensor], valorCalibradoMinimo, valorCalibradoMaximo););
    Serial.print(")");
    Serial.print("\t");
  }
  Serial.print("=>");
  posicionActual = calcular_posicion(posicionActual);
  Serial.println(posicionActual); 
  */

  // return;

  CalibracionPID.update();
  delay(50);
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
        while (millis() < (millisIniciales + 5000)) {
          tiempoPasado = millis() - millisIniciales;
          byte r = 0, g = 0;
          r = map(tiempoPasado, 0, 5000, 255, 0);
          g = map(tiempoPasado, 0, 1000, 0, 255);
          set_color_RGB(r, g, 0);
          if (tiempoPasado / 1000 > 2 && velocidadSuccion == 0) {
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
