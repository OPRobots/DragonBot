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
