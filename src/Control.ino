/**
 * Función para controlar nivel de carga de la LiPo durante el funcionamiento
 * @param  enLoop Indica si se ha llamado a la función desde el loop, para efectuar la comprobación con intervalo
 */
void nivel_bateria(bool enLoop){
  int carga;
  byte r, g;
  if(enLoop){
    if(millis()> (ultimaBateria+intervaloAviso)){
      set_color_RGB(0, 0, 0);
      switch (LIPO) {
        case LIPO_2S:
          filtroBateria.Filter(map(analogRead(NIVEL_BATERIA), 2500, 2300, 100,0));
        break;
        case LIPO_3S:
          filtroBateria.Filter(map(analogRead(NIVEL_BATERIA), 4083, 2718, 100,0));
        break;
      }
      carga = filtroBateria.Current();
      if(carga <= 15){
        if (carga <= 5){
          intervaloAviso = 100;
        }else{
          intervaloAviso = 500;
        }
        avisoBateria = !avisoBateria;
        if(avisoBateria){
          set_color_RGB(255, 0, 0);
        }else{
          set_color_RGB(0, 0, 0);
        }
      }
      ultimaBateria = millis();
    }
  }else{
    filtroBateria.Filter(0);
    delay(500);
    long millisInicial = millis();
    do{
      switch (LIPO) {
        case LIPO_2S:
          filtroBateria.Filter(map(analogRead(NIVEL_BATERIA), 2500, 2300, 100,0));
        break;
        case LIPO_3S:
          filtroBateria.Filter(map(analogRead(NIVEL_BATERIA), 4083, 2718, 100,0));
        break;
      }
      carga = filtroBateria.Current();
      if(carga > 50){
        r = map(carga, 51,100, 255,0);
        g = 255;
      }else if (carga < 50){
        r = 255;
        g = map(carga, 49,0, 255,0);
      }else{
        r = 255;
        g = 255;
      }
      set_color_RGB(r, g, 0);
      delay(50);
    }while((millis()-millisInicial) < 2000);
    set_color_RGB(0,0,0);
  }

}

/**
 * Función para calcular a posición para cada tipo de pista.
 * @param  ultimaPosicion Última posición calculada para usar en caso de pérdida de pista
 * @return [int]          Posición actual sobre la línea
 */
int calcular_posicion(int ultimaPosicion){
  switch (PISTA) {
    case MODO_LINEA:
      return calcula_posicion_linea(ultimaPosicion);
    break;
    case MODO_DEGRADADO:
      // TODO: añadir función de cálculo de posición para degradados.
    break;
  }
  return 0;
}

/**
 * Función para realizar el cálculo de la posición en base a la lectura de los sensores.
 * @param  ultimaPosicion Última posición calculada para usar en caso de pérdida de línea
 * @return [int]           Posición actual sobre la línea
 */
int calcula_posicion_linea(int ultimaPosicion){
  lectura_sensores_calibrados();

  unsigned long sumaSensoresPonderados = 0;
  unsigned long sumaSensores = 0;
  int sensoresDetectando = 0;

  for (int sensor = 0; sensor < NUMERO_SENSORES; sensor++) {
		sumaSensoresPonderados += (sensor + 1) * valoresSensores[sensor] * 1000L;
		sumaSensores += (long)valoresSensores[sensor];
    if(valoresSensores[sensor] > ((valorCalibradoMaximo-valorCalibradoMinimo)/3*2.0f)){
      sensoresDetectando++;
    }
	}

  if(sensoresDetectando > 0){
    ultimaLinea = millis();
  }else if(millis()>(ultimaLinea+TIEMPO_SIN_PISTA)){
    kp = 0;
    ki = 0;
    kd = 0;
    velocidadBase = 0;
  }

  if(sensoresDetectando > 0){
    return ((sumaSensoresPonderados/sumaSensores)-(NUMERO_SENSORES+1)*(float)(1000/2));
  }else{
    return (ultimaPosicion > 0)? (1000 * (NUMERO_SENSORES +1) /2) : -(1000 * (NUMERO_SENSORES +1) /2);
  }
}

/**
 * Función para calcular la corrección a aplicar a los motores para mantenerse en la posición deseada de la pista.
 * @param  posicionActual Posición actual sobre la pista.
 * @return [int]          Corrección que se debe aplicar al control de la velocidad.
 */
int calcular_PID(int posicionActual){
  float p = 0;
  float i = 0;
  float d = 0;
  int error = 0;
  error = posicionIdeal - posicionActual;
  p = kp * error;
  if (error < 100){
    integralErrores += error;
    i = ki * integralErrores;
  }else{
    i = 0;
    integralErrores = 0;
  }
  d = kd * (error - errorAnterior);
  errorAnterior = error;
  return p + i + d;
}

/**
 * Función para asignar velocidad a los motores teniendo en cuenta la corrección calculada por el PID
 * @param correccion Parámetro calculado por el PID para seguir la posición deseada en la pista
 */
void dar_velocidad(int correccion) {
	velocidadIzquierda = velocidadBase - correccion;
	velocidadDerecha = velocidadBase + correccion;

	int pin_motor_derecho = MOTOR_DERECHO_ADELANTE;
	int pin_motor_izquierdo = MOTOR_IZQUIERDO_ADELANTE;

	if (velocidadDerecha > velocidadMaxima) {
		velocidadDerecha = velocidadMaxima;
		pin_motor_derecho = MOTOR_DERECHO_ADELANTE;
	} else if (velocidadDerecha < 0) {
		velocidadDerecha = abs(velocidadDerecha);
		if (velocidadDerecha > velocidadMaxima) {
			velocidadDerecha = velocidadMaxima;
		}
		pin_motor_derecho = MOTOR_DERECHO_ATRAS;
	}
	if (velocidadIzquierda > velocidadMaxima) {
		velocidadIzquierda = velocidadMaxima;
		pin_motor_izquierdo = MOTOR_IZQUIERDO_ADELANTE;
	} else if (velocidadIzquierda < 0) {
		velocidadIzquierda = abs(velocidadIzquierda);
		if (velocidadIzquierda > velocidadMaxima) {
			velocidadIzquierda = velocidadMaxima;
		}
		pin_motor_izquierdo = MOTOR_IZQUIERDO_ATRAS;
	}

	digitalWrite(MOTOR_DERECHO_ADELANTE, LOW);
	digitalWrite(MOTOR_DERECHO_ATRAS, LOW);
	digitalWrite(MOTOR_IZQUIERDO_ADELANTE, LOW);
	digitalWrite(MOTOR_IZQUIERDO_ATRAS, LOW);

	digitalWrite(pin_motor_derecho, HIGH);
	digitalWrite(pin_motor_izquierdo, HIGH);

	analogWrite(MOTOR_DERECHO_PWM   , velocidadDerecha);
	analogWrite(MOTOR_IZQUIERDO_PWM , velocidadIzquierda);
}

/**
 * Función de interrupción para el canal A del encoder derecho
 */
void encoder_derecho_A(){
  ticksDerecho++;
}

/**
 * Función de interrupción para el canal A del encoder izquierdo
 */
void encoder_izquierdo_A(){
  ticksIzquierdo++;
}

/**
 * Función para calcular la velocidad del robot en m/s
 * @return Velocidad en m/s del robot, actualizada cada segundo
 */
float calcular_velocidad(){
  // if(millis()>=(ultimaVelocidad + 1000)){
    float velocidad = ((((ticksDerecho-ticksDerechoAnteriores)/(float)(millis()-ultimaVelocidad))*ticksMm) + (((ticksDerecho-ticksDerechoAnteriores)/(float)(millis()-ultimaVelocidad))*ticksMm))/2.0f;
    ultimaVelocidad = millis();
    ticksDerechoAnteriores = ticksDerecho;
    ticksIzquierdoAnteriores = ticksIzquierdo;
    return velocidad;
  // }else{
  //   return velocidadActual;
  // }
}
