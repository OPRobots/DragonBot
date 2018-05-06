/**
 * Función principal de calibrado de sensores.
 * Selecciona la función de calibrado adecuada para cada tipo de pista.
 */
void calibra_sensores() {
  switch (PISTA) {
  case MODO_LINEA:
    valorSaturacionBajo = SATURACION_MINIMO_SENSORES_LINEA;
    valorSaturacionAlto = SATURACION_MAXIMO_SENSORES_LINEA;
    valorCalibradoMaximo = CALIBRADO_MAXIMO_SENSORES_LINEA;
    valorCalibradoMinimo = CALIBRADO_MINIMO_SENSORES_LINEA;
    posicionMaxima = 6500;
    posicionMinima = -6500;
    calibrado_sensores_linea();
    break;
  case MODO_DEGRADADO:
    valorSaturacionBajo = SATURACION_MINIMO_SENSORES_DEGRADADO;
    valorSaturacionAlto = SATURACION_MAXIMO_SENSORES_DEGRADADO;
    valorCalibradoMaximo = CALIBRADO_MAXIMO_SENSORES_DEGRADADO;
    valorCalibradoMinimo = CALIBRADO_MINIMO_SENSORES_DEGRADADO;
    posicionMaxima = 200;
    posicionMinima = -200;
    calibrado_sensores_degradado();
    break;
  }
  CalibracionPID.setMaxIdeal(posicionMaxima);
  CalibracionPID.setMinIdeal(posicionMinima);
  delay(300);
}

/**
 * Función de calibrado de sensores para línea.
 * Realiza una calibración simple de valores máximos y mínimos durante el tiempo de calibración establecido.
 */
void calibrado_sensores_linea() {
  long tiempoInicial = millis();

  set_color_RGB(0, 0, 0);

  while (millis() < (tiempoInicial + TIEMPO_CALIBRADO)) {
    for (int sensor = 0; sensor < NUMERO_SENSORES; sensor++) {
      int lectura = mux_analog_read(pinesSensores[sensor]);
      if (lectura < valoresCalibracionMinimos[sensor]) {
        valoresCalibracionMinimos[sensor] = lectura;
      } else if (lectura > valoresCalibracionMaximos[sensor]) {
        valoresCalibracionMaximos[sensor] = lectura;
      }
      set_rainbow_color();
    }
  }

  for (int sensor = 0; sensor < NUMERO_SENSORES; sensor++) {

    Serial.print("valoresCalibracionMaximos[");
    Serial.print(sensor);
    Serial.print("] = ");
    Serial.print(valoresCalibracionMaximos[sensor]);
    Serial.print(";\n");
  }

  for (int sensor = 0; sensor < NUMERO_SENSORES; sensor++) {

    Serial.print("valoresCalibracionMinimos[");
    Serial.print(sensor);
    Serial.print("] = ");
    Serial.print(valoresCalibracionMinimos[sensor]);
    Serial.print(";\n");
  }

  set_color_RGB(0, 0, 0);
}

/**
 * Función de calibrado de sensores para degradado.
 * Realiza una doble calibración para obtener los valores máximos y mínimos de cada sensor y el máximo y mínimo de la posición.
 */
void calibrado_sensores_degradado() {
  calibrado_sensores_linea();

  set_color_RGB(0, 0, 0);
}
