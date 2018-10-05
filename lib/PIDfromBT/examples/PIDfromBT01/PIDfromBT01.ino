///////////////////////////////////////////////////////
// Programa de ejemplo para la librería PIDfromBT.h  //
//                                                   //
// APP: https://github.com/robotaleh/PIDfromBT       //
// Lib: https://github.com/robotaleh/PIDfromBTlib    //
//                                                   //
// Creado por Alex Santos aka @robotaleh de OPRobots //
///////////////////////////////////////////////////////

//Inclusión de librería
#include <PIDfromBT.h>

#define MAX_IDEAL 1000
#define MIN_IDEAL -1000

// Definición de constantes
float kp = 0, ki = 0, kd = 0;
int ideal = 0;
int vel = 0;
int suc = 0;

// Creación del Objeto PIDfromBT
PIDfromBT pid_calibrate(&kp, &ki, &kd, &vel, &ideal, &suc, DEBUG);

void setup() {
  // Inicia el Serial
  Serial.begin(9600);

  // Establece los valores Máximo y Mínimo del Ideal
  pid_calibrate.setMinIdeal(MIN_IDEAL);
  pid_calibrate.setMaxIdeal(MAX_IDEAL);

}

void loop() {
  // Ejecutar el update() para revisar si hay instrucciones en el Buffer
  pid_calibrate.update();
}
