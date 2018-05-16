/**
 * Función para controlar nivel de carga de la LiPo durante el funcionamiento
 * @param  enLoop Indica si se ha llamado a la función desde el loop, para efectuar la comprobación con intervalo
 */
void nivel_bateria(bool enLoop) {
  int carga;
  byte r, g;
  if (enLoop) {
    if (millis() > (ultimaBateria + intervaloAviso)) {
      set_color_RGB(0, 0, 0);
      switch (LIPO) {
      case LIPO_2S:
        filtroBateria.Filter(map(analogRead(NIVEL_BATERIA), 2500, 2300, 100, 0));
        break;
      case LIPO_3S:
        filtroBateria.Filter(map(analogRead(NIVEL_BATERIA), 4083, 2718, 100, 0));
        break;
      }
      carga = filtroBateria.Current();
      if (carga <= 15) {
        if (carga <= 5) {
          intervaloAviso = 100;
        } else {
          intervaloAviso = 500;
        }
        avisoBateria = !avisoBateria;
        if (avisoBateria) {
          set_color_RGB(255, 0, 0);
        } else {
          set_color_RGB(0, 0, 0);
        }
      }
      ultimaBateria = millis();
    }
  } else {
    filtroBateria.Filter(0);
    delay(500);
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
      if (carga > 50) {
        r = map(carga, 51, 100, 255, 0);
        g = 255;
      } else if (carga < 50) {
        r = 255;
        g = map(carga, 49, 0, 255, 0);
      } else {
        r = 255;
        g = 255;
      }
      set_color_RGB(r, g, 0);
      delay(50);
    } while ((millis() - millisInicial) < 2000);
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
  case MODO_DEGRADADO:
    return calcula_posicion_degradado(ultimaPosicion);
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
  int numLineas = 0;
  bool detectandoAnterior = false;

  for (int sensor = 0; (sensor < NUMERO_SENSORES && numLineas < 2); sensor++) {
    if (!((sensor >= lineaPrincipal[0] && sensor <= lineaPrincipal[1]) || abs(sensor - lineaPrincipal[0]) <= 1 || abs(sensor - lineaPrincipal[1]) <= 1)) {
      if (LINEA == LINEA_NEGRA) {
          valoresSensores[sensor] = 0;
        } else {
        valoresSensores[sensor] = valorCalibradoMaximo;
      }
    }else if(valoresSensores[sensor] > valorSaturacionBajo){
      sensoresDetectando++;
        }
    if (!detectandoAnterior && valoresSensores[sensor] > valorSaturacionBajo) {
      detectandoAnterior = true;
      lineaPrincipal[0] = sensor;
      numLineas++;
    } else if (detectandoAnterior && valoresSensores[sensor] <= valorSaturacionBajo) {
      lineaPrincipal[1] = sensor - 1;
      detectandoAnterior = false;
      }
    if (sensor == NUMERO_SENSORES - 1 && detectandoAnterior && valoresSensores[sensor] > valorSaturacionBajo) {
      lineaPrincipal[1] = sensor;
    }
    sumaSensoresPonderados += (sensor + 1) * valoresSensores[sensor] * 1000L;
    sumaSensores += (long)valoresSensores[sensor];
    }
  if (lineaPrincipal[1] == -1) {
    lineaPrincipal[1] = NUMERO_SENSORES - 1;
  }

  if (sensoresDetectando > 0 && sensoresDetectando < NUMERO_SENSORES) {
    ultimaLinea = millis();
  } else if (millis() > (ultimaLinea + TIEMPO_SIN_PISTA)) {
    // kp = 0;
    // ki = 0;
    // kd = 0;
    velocidad = 0;
    velocidadSuccion = 0;
    competicionIniciada = false;
    set_color_RGB(0, 0, 0);
  }

  if (numLineas <= 1) {
  if (sensoresDetectando > 0) {
    return ((sumaSensoresPonderados / sumaSensores) - (NUMERO_SENSORES + 1) * (float)(1000 / 2));
  } else {
    return (ultimaPosicion > 0) ? (1000 * (NUMERO_SENSORES + 1) / 2) : -(1000 * (NUMERO_SENSORES + 1) / 2);
  }
  } else {
    return ultimaPosicion;
  }
}

/**
 * Función para realizar el cálculo de la posición en base a la lectura de los sensores.
 * @param  ultimaPosicion Última posición calculada para usar en caso de pérdida de degradado
 * @return [int]           Posición actual sobre el degradado
 */
int calcula_posicion_degradado(int ultimaPosicion) {
  lectura_sensores_calibrados();
  int posicionesSensores[NUMERO_SENSORES];
  int posicionSensores = 0;
  int numeroSensoresPista = 0;
  int sensorPistaInicial = -1;
  int sensorPistaFinal = -1;
  int posicion = 0;

  for (int sensor = 0; sensor < NUMERO_SENSORES; sensor++) {
    bool rangoEncontrado = false;
    bool sensorEnPista = false;
    int rangoInicio = 0;
    int rangoFinal = 0;

    // BUSCAR SENSORES EN PISTA
    if (valoresSensoresRaw[sensor] >= valoresCalibracionMinimos[sensor]) {
      if (sensorPistaInicial < 0) {
        sensorPistaInicial = sensor;
      } else {
        sensorPistaFinal = sensor;
      }
      sensorEnPista = true;
      numeroSensoresPista++;
    }

    if (sensorEnPista) {
      // BUSCAR RANGO DE CALIBRADO
      for (int calibrado = 0; (calibrado < NUMERO_CALIBRACIONES_DEGRADADO && !rangoEncontrado); calibrado++) {
        if (calibrado != NUMERO_CALIBRACIONES_DEGRADADO - 1) {
          if (valoresSensores[sensor] <= calibradoDegradado[calibrado][sensor] && valoresSensores[sensor] >= calibradoDegradado[calibrado + 1][sensor]) {
            rangoEncontrado = true;
            rangoInicio = calibrado;
            rangoFinal = calibrado + 1;
          }
        }
      }

      // CALCULAR DISTANCIA
      if (rangoEncontrado) {
        posicionesSensores[sensor] = map(valoresSensores[sensor], calibradoDegradado[rangoInicio][sensor], calibradoDegradado[rangoFinal][sensor], rangoInicio * 1000, rangoFinal * 1000);
      } else {
        if (valoresSensores[sensor] > calibradoDegradado[0][sensor]) {
          posicionesSensores[sensor] = 0;
        } else {
          posicionesSensores[sensor] = 20000;
        }
      }

      // ASIGNAR OFFSET EN FUCIÓN DE NUMERO DE SENSOR
      posicionesSensores[sensor] += (((numeroSensoresPista + 1) / 2.0f) - (sensor + 1)) * 1000;
      posicionSensores += posicionesSensores[sensor];
    }
  }

  if (sensorPistaFinal < 0) {
    sensorPistaFinal = NUMERO_SENSORES - 1;
  }
  if (numeroSensoresPista > 1) {
    posicion = posicionDegradadoMaxima - (posicionSensores / numeroSensoresPista);
    posicion = posicion / 100;
  } else {
    posicion = 200;
  }
  // for (int sensor = 0; sensor < NUMERO_SENSORES; sensor++) {
  //   Serial.print(posicionesSensores[sensor]);
  //   Serial.print("\t");
  // }

  // DAR SIGNO EN FUNCIÓN DE SENSORES DE EXTREMO

  if (numeroSensoresPista > 0) {
    ultimaLinea = millis();
  } else if (millis() > (ultimaLinea + TIEMPO_SIN_PISTA)) {
    // kp = 0;
    // ki = 0;
    // kd = 0;
    velocidad = 0;
    velocidadSuccion = 0;
    competicionIniciada = false;
    set_color_RGB(0, 0, 0);
  }

  if (numeroSensoresPista > 1 && abs(ultimaPosicion) < 50) {
    if (valoresSensores[sensorPistaInicial] > valoresSensores[sensorPistaFinal]) {
      posicion = -posicion;
    }
  } else {
    if (ultimaPosicion < 0) {
      posicion = -posicion;
    }
  }
  return posicion;
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
  if (PISTA == MODO_DEGRADADO) {
    error = -error;
  }
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
  if (true || velocidad > 0) {
    return p + i + d;
  } else {
    errorAnterior = 0;
    integralErrores = 0;
    return 0;
  }
}

int calcular_PID_frontal(int valorSensorFrontal) {
  float p = 0;
  float i = 0;
  float d = 0;
  int errorFrontal = 0;
  errorFrontal = 1500 - valorSensorFrontal;
  if (errorFrontal < 0 && velocidad > 0) {
    p = kpFrontal * errorFrontal;
    if (errorFrontal < 100) {
      integralErrores += errorFrontal;
      i = kiFrontal * integralErrores;
    } else {
      i = 0;
      integralErrores = 0;
    }
    d = kdFrontal * (errorFrontal - errorFrontalAnterior);
    errorFrontalAnterior = errorFrontal;
    if (velocidad > 0) {
      return p + i + d;
    } else {
      errorFrontalAnterior = 0;
      integralErrores = 0;
      return 0;
    }
  } else {
    errorFrontalAnterior = 0;
    return 0;
  }
}

/**
 * Función para asignar velocidad a los motores teniendo en cuenta la corrección calculada por el PID
 * @param correccion Parámetro calculado por el PID para seguir la posición deseada en la pista
 */
void dar_velocidad(int correccion, int correccionFrontal) {
  velocidadIzquierda = velocidad - correccion + correccionFrontal;
  velocidadDerecha = velocidad + correccion + correccionFrontal;

  if (velocidad > 0) {
    velocidadIzquierda += COMPENSACION_IZQUIERDO;
    velocidadDerecha += COMPENSACION_DERECHO;
  }

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

  analogWrite(MOTOR_DERECHO_PWM, velocidadDerecha);
  analogWrite(MOTOR_IZQUIERDO_PWM, velocidadIzquierda);
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
 * @return Velocidad en m/s del robot, actualizada cada segundo
 */
float calcular_velocidad() {
  float velocidad = ((((ticksDerecho - ticksDerechoAnteriores) / (float)(millis() - ultimaVelocidad)) * ticksMm) + (((ticksDerecho - ticksDerechoAnteriores) / (float)(millis() - ultimaVelocidad)) * ticksMm)) / 2.0f;
  ultimaVelocidad = millis();
  ticksDerechoAnteriores = ticksDerecho;
  ticksIzquierdoAnteriores = ticksIzquierdo;
  return velocidad;
}

/**
 * Función para mapeado del circuito en busca de rectas para acelerar.
 */
void mapeado_circuito() {
  // 5213 1598
  int diferencia;
  if (mapeoRealizado) {
    diferencia = abs((ticksDerecho - ticksMapeoDerechoAnteriores) - (ticksIzquierdo - ticksMapeoIzquierdoAnteriores));
    filtroMapeo.Filter(diferencia);

    switch (sectoresPista[sectorActual][SECTOR_TIPO]) {
    case TIPO_SECTOR_RECTA:
      velocidad = velocidadRectas;
      velocidadSuccionBase = velocidadSuccionRectas;
      if (mediaDiferenciaRecta == 0) {
        mediaDiferenciaRecta = filtroMapeo.Current();
      } else {
        mediaDiferenciaRecta = (mediaDiferenciaRecta + diferencia) / 2;
      }
      break;
    case TIPO_SECTOR_CURVA:
      mediaDiferenciaRecta = 0;
      velocidad = velocidadCurvas;
      velocidadSuccionBase = velocidadSuccionCurvas;
      break;
    }

    if (ticksDerecho >= sectoresPista[sectorActual][SECTOR_TICKS]) {
      ticksDerecho = 0;
      ticksIzquierdo = 0;
      sectorActual++;
      filtroMapeo.SetCurrent(0);
      if (sectorActual == NUMERO_SECTORES) {
        sectorActual = 0;
        ticksReseteados = false;
      }
    }
    if (sectorActual == 0 && !ticksReseteados && filtroMapeo.Current() > (mediaDiferenciaRecta + 10)) {
      ticksReseteados = true;
      ticksDerecho = 700;
      ticksIzquierdo = 700;
      mediaDiferenciaRecta = 0;
      Serial.println("reset");
    }
    ticksMapeoDerechoAnteriores = ticksDerecho;
    ticksMapeoIzquierdoAnteriores = ticksIzquierdo;
  } else {
    diferencia = abs((ticksDerecho - ticksMapeoDerechoAnteriores) - (ticksIzquierdo - ticksMapeoIzquierdoAnteriores));
    filtroMapeo.Filter(diferencia);

    if (ticksDerecho > 200 && ticksIzquierdo > 200) {
      if (filtroMapeo.Current() <= 25) {
        if (tipoSector == TIPO_SECTOR_CURVA && mapeoIniciado) {
          Serial.println(filtroMapeo.Current());
          sectoresPista[sectorActual][SECTOR_TICKS] = ticksDerecho;
          sectoresPista[sectorActual][SECTOR_TIPO] = tipoSector;
          sectorActual++;
          tipoSector = TIPO_SECTOR_RECTA;
          ticksDerecho = 0;
          ticksIzquierdo = 0;
          set_color_RGB(0, 255, 0);
        }
      } else {
        if (tipoSector == TIPO_SECTOR_RECTA && mapeoIniciado) {
          sectoresPista[sectorActual][SECTOR_TICKS] = ticksDerecho - 700;
          sectoresPista[sectorActual][SECTOR_TIPO] = tipoSector;
          sectorActual++;
          tipoSector = TIPO_SECTOR_CURVA;
          ticksDerecho = 0;
          ticksIzquierdo = 0;
          set_color_RGB(0, 0, 255);
        } else if (!mapeoIniciado) {
          mapeoIniciado = true;
          ticksDerecho = 0;
          ticksIzquierdo = 0;
          sectorActual = 0;
          tipoSector = TIPO_SECTOR_CURVA;
          set_color_RGB(0, 0, 255);
        }
      }
      if (sectorActual == NUMERO_SECTORES) {
        sectorActual = 0;
        for (int i = 0; i < NUMERO_SECTORES; i++) {
          Serial.print(sectoresPista[i][SECTOR_TICKS]);
          Serial.print("\t");
        }
        Serial.println();
        mapeoRealizado = true;
        ticksIzquierdo = 700;
        ticksDerecho = 700;
        set_color_RGB(0, 0, 0);
      }
    }
    ticksMapeoDerechoAnteriores = ticksDerecho;
    ticksMapeoIzquierdoAnteriores = ticksIzquierdo;
  }
}
