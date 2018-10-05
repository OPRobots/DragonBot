#include <ATDelay.h>
//TestDelay.ino

#define PIN_LED 13
bool state = HIGH;


// Creación del objeto de la clase ATDelay, 
// pasando como parametro los millis del delay.
ATDelay custom_delay(500);


void setup() {
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, state);


  // Reseteo inicial del contador interno del objeto:
  // Empieza a contar los millis desde este momento.
  custom_delay.reset();
}

void loop() {
  
  // Comprueba si se ha cumplido el tiempo de espera con la funcion "check()"
  // que retorna un true si el tiempo desde el último reseteo ha superado o 
  // igualado al establecido al inicio
  if(custom_delay.check()){
  	state = !state;
  	digitalWrite(PIN_LED, state);

  	// Reinicia la cuenta una vez se haya realizado el proceso:
  	// Desde este momento, volverá a contar los millis para la comprobación.
  	custom_delay.reset();
  }
    
}
