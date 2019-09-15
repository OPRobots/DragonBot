/**
 * Función para controlar nivel de carga de la LiPo durante el funcionamiento
 * @param  enLoop Indica si se ha llamado a la función desde el loop, para efectuar la comprobación con intervalo
 */
void nivel_bateria(bool enLoop) {
  int carga;
  byte r, g;
  if (enLoop) {
    if (millis() > (ultimaBateria + intervaloAvisoBateria)) {
      set_color_RGB(0, 0, 0);
      switch (LIPO) {
      case LIPO_2S:
        filtroBateria.Filter(map(analogRead(NIVEL_BATERIA), 2447, 2247, 100, 0));
        break;
      case LIPO_3S:
        filtroBateria.Filter(map(analogRead(NIVEL_BATERIA), 4083, 2718, 100, 0));
        break;
      }
      carga = filtroBateria.Current();
      if (carga <= 15) {
        if (carga <= 5) {
          intervaloAvisoBateria = 100;
        } else {
          intervaloAvisoBateria = 500;
        }
        avisoBateria = !avisoBateria;
        if (avisoBateria) {
          set_color_RGB(255, 0, 0);
        } else {
          set_color_RGB(0, 0, 0);
        }
      }
      ultimaBateria = millis();
    } else {
      set_color_RGB(0, 0, 0);
      /* if (velocidad >= 200) {
        if (!avisoBateria) {
          set_color_RGB(0, 255, 255);
        }
      }
      else {
        if (!avisoBateria) {
          set_color_RGB(0, 0, 0);
        }
      }
      */
    }
  } else {
    filtroBateria.Filter(0);
    long millisInicial = millis();
    do {
      switch (LIPO) {
      case LIPO_2S:
        filtroBateria.Filter(map(analogRead(NIVEL_BATERIA), 2500, 2300, 100, 0));
        break;
      case LIPO_3S:
        filtroBateria.Filter(map(analogRead(NIVEL_BATERIA), 4083, 2718, 100, 0));
        break;
      }
      carga = filtroBateria.Current();
      if (carga >= 75) {
        r = 0;
        g = 255;
      } else if (carga > 50) {
        r = map(carga, 51, 100, 255, 0);
        g = 255;
      } else if (carga < 50) {
        r = 255;
        g = map(carga, 49, 0, 255, 0);
      } else {
        r = 255;
        g = 255;
      }
      delay(1);
    } while ((millis() - millisInicial) < 500);
    set_color_RGB(r, g, 0);
    delay(2000);
    set_color_RGB(0, 0, 0);
  }
}

/**
 * Función para calcular a posición para cada tipo de pista.
 * @param  ultimaPosicion Última posición calculada para usar en caso de pérdida de pista
 * @return [int]          Posición actual sobre la línea
 */
int calcular_posicion(int ultimaPosicion) {
  switch (PISTA) {
  case MODO_LINEA:
    return calcula_posicion_linea(ultimaPosicion);
    break;
  }
  return 0;
}

/**
 * Función para realizar el cálculo de la posición en base a la lectura de los sensores.
 * @param  ultimaPosicion Última posición calculada para usar en caso de pérdida de línea
 * @return [int]           Posición actual sobre la línea
 */
int calcula_posicion_linea(int ultimaPosicion) {
  lectura_sensores_calibrados();

  unsigned long sumaSensoresPonderados = 0;
  unsigned long sumaSensores = 0;
  int sensoresDetectando = 0;
  bool detectandoAnterior = false;

  for (int sensor = 0; sensor < NUMERO_SENSORES; sensor++) {
    if (valoresSensores[sensor] > umbralesCalibracionSensores[sensor]) {
      sensoresDetectando++;
    }
    // Realiza los cálculos para la posición
    sumaSensoresPonderados += (sensor + 1) * valoresSensores[sensor] * 1000L;
    sumaSensores += (long)valoresSensores[sensor];
  }

  // Comprueba el tiempo sin pista y se para automáticamente para evitar daños
  if (sensoresDetectando > 0 && sensoresDetectando < NUMERO_SENSORES) {
    ultimaLinea = millis();
  } else if (millis() > (ultimaLinea + TIEMPO_SIN_PISTA)) {
    competicionIniciada = false;
    set_color_RGB(0, 0, 0);
    pausa_timer_PID();
  }
  int pos;
  if (sensoresDetectando > 0) {
    pos = ((sumaSensoresPonderados / sumaSensores) - (NUMERO_SENSORES + 1) * (float)(1000 / 2));
  } else {
    pos = (ultimaPosicion > 0) ? (1000 * (NUMERO_SENSORES + 1) / 2) : -(1000 * (NUMERO_SENSORES + 1) / 2);
  }
  return map(pos, posicionMinima, posicionMaxima, -1000, 1000);
}

/**
 * Función para calcular la corrección a aplicar a los motores para mantenerse en la posición deseada de la pista.
 * @param  posicionActual Posición actual sobre la pista.
 * @return [int]          Corrección que se debe aplicar al control de la velocidad.
 */
int calcular_PID(int posicionActual) {
  float p = 0;
  float i = 0;
  float d = 0;
  int error = 0;
  error = posicionIdeal - posicionActual;

  p = kp * error;
  if (error < 100) {
    integralErrores += error;
    i = ki * integralErrores;
  } else {
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

  int velocidadIzquierda = velocidad;
  int velocidadDerecha = velocidad;
  if (!timerPID_pause) {
    velocidadIzquierda = velocidadIzquierda - correccion;
    velocidadDerecha = velocidadDerecha + correccion;
  }

  if (velocidadDerecha > velocidadMaxima) {
    velocidadDerecha = velocidadMaxima;
  } else if (velocidadDerecha < -velocidadMaxima) {
    velocidadDerecha = -velocidadMaxima;
  }
  if (velocidadIzquierda > velocidadMaxima) {
    velocidadIzquierda = velocidadMaxima;
  } else if (velocidadIzquierda < -velocidadMaxima) {
    velocidadIzquierda = velocidadMaxima;
  }

  if (velocidadDerecha < 0) {
    velocidadDerecha = 0;
  }
  if (velocidadIzquierda < 0) {
    velocidadIzquierda = 0;
  }
  if (velocidad > 0) {
    velocidadDerecha = constrain(velocidadDerecha, 200, 1000);
    velocidadIzquierda = constrain(velocidadIzquierda, 200, 1000);
  }

  pwmWrite(MOTOR_DERECHO_PWM, map(velocidadDerecha, velocidadMaxima, -velocidadMaxima, MIN_ESC_VELOCIDAD, MAX_ESC_VELOCIDAD));
  pwmWrite(MOTOR_IZQUIERDO_PWM, map(velocidadIzquierda, velocidadMaxima, -velocidadMaxima, MIN_ESC_VELOCIDAD, MAX_ESC_VELOCIDAD));
}

/**
 * Función de interrupción para el canal A del encoder derecho
 */
void encoder_derecho_A() {
  if (digitalRead(MOTOR_DERECHO_ENCODER_A)) {
    if (!digitalRead(MOTOR_DERECHO_ENCODER_B)) {
      ticksDerecho++;
    } else {
      ticksDerecho--;
    }
  } else {
    if (!digitalRead(MOTOR_DERECHO_ENCODER_B)) {
      ticksDerecho--;
    } else {
      ticksDerecho++;
    }
  }
}

/**
 * Función de interrupción para el canal B del encoder derecho
 */
void encoder_derecho_B() {
  if (digitalRead(MOTOR_DERECHO_ENCODER_B)) {
    if (!digitalRead(MOTOR_DERECHO_ENCODER_A)) {
      ticksDerecho--;
    } else {
      ticksDerecho++;
    }
  } else {
    if (!digitalRead(MOTOR_DERECHO_ENCODER_A)) {
      ticksDerecho++;
    } else {
      ticksDerecho--;
    }
  }
}

/**
 * Función de interrupción para el canal A del encoder izquierdo
 */
void encoder_izquierdo_A() {
  if (digitalRead(MOTOR_IZQUIERDO_ENCODER_A)) {
    if (digitalRead(MOTOR_IZQUIERDO_ENCODER_B)) {
      ticksIzquierdo--;
    } else {
      ticksIzquierdo++;
    }
  } else {
    if (digitalRead(MOTOR_IZQUIERDO_ENCODER_B)) {
      ticksIzquierdo++;
    } else {
      ticksIzquierdo--;
    }
  }
}

/**
 * Función de interrupción para el canal B del encoder izquierdo
 */
void encoder_izquierdo_B() {
  if (digitalRead(MOTOR_IZQUIERDO_ENCODER_B)) {
    if (digitalRead(MOTOR_IZQUIERDO_ENCODER_A)) {
      ticksIzquierdo++;
    } else {
      ticksIzquierdo--;
    }
  } else {
    if (digitalRead(MOTOR_IZQUIERDO_ENCODER_A)) {
      ticksIzquierdo--;
    } else {
      ticksIzquierdo++;
    }
  }
}

/**
 * Función para calcular la velocidad del robot en m/s
 * @return Velocidad en m/s del robot, actualizada cada 20ms
 */
float calcular_velocidad() {
  // 0.02 porque se ejecuta cada 20ms
  float velocidadLinealDerecha = ((2 * 3.1416f * (ticksDerecho - ticksDerechoAnteriores)) / (ENCODER_PPR * 0.02f)) * (float)RUEDAS_RADIO;
  float velocidadLinealIzquierda = ((2 * 3.1416f * (ticksIzquierdo - ticksIzquierdoAnteriores)) / (ENCODER_PPR * 0.02f)) * (float)RUEDAS_RADIO;
  float velocidadLinealMedia = (velocidadLinealIzquierda + velocidadLinealDerecha) / 2.0f;
  velocidadW = (velocidadLinealIzquierda - velocidadLinealDerecha) / 0.120f;
  anguloGiroR += velocidadW * 0.02f;
  anguloGiro = anguloGiroR * 57.2958f;
  float avance = velocidadLinealMedia * 0.02f;
  posXm += avance * cos(anguloGiroR);
  posYm += avance * sin(anguloGiroR);
  ticksDerechoAnteriores = ticksDerecho;
  ticksIzquierdoAnteriores = ticksIzquierdo;
  return velocidadLinealMedia;
}

/**
 * Función para calcular la velocidad del robot en PWM para asignar a los motores
 * @return Velocidad en PWM del robot, actualizada cada 20ms a partir de los m/s de consigna
 */
int ajustar_velocidad_encoders() {
  float errorVelocidad = velocidadMsIdeal - velocidadMs;
  float p = kpVelocidad * errorVelocidad;
  float d = kdVelocidad * (errorVelocidad - ultimoErrorVelocidad);
  ultimoErrorVelocidad = errorVelocidad;
  return p + d;
}
