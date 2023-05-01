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
    calculo_umbrales_sensores();
    break;
  }
  CalibracionPID.setMaxIdeal(posicionMaxima);
  CalibracionPID.setMinIdeal(posicionMinima);
}

/**
 * Función de calibrado de sensores para línea.
 * Realiza una calibración simple de valores máximos y mínimos durante el tiempo de calibración establecido.
 */
void calibrado_sensores_linea() {
  if (CALIBRAR_SENSORES) {
    long tiempoInicial = millis();
    while (millis() < (tiempoInicial + TIEMPO_CALIBRADO)) {
      for (int sensor = 0; sensor < NUMERO_SENSORES; sensor++) {
        int lectura = mux_analog_read(pinesSensores[sensor]);
        if (lectura < valoresCalibracionMinimos[sensor]) {
          valoresCalibracionMinimos[sensor] = lectura;
        } else if (lectura > valoresCalibracionMaximos[sensor]) {
          valoresCalibracionMaximos[sensor] = lectura;
        }
        set_rainbow_color(5);
      }
    }

    set_color_RGB(0, 0, 0);

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
  }
}

/**
 * Función de calibrado de sensores para línea.
 * Realiza una calibración simple de valores máximos y mínimos durante el tiempo de calibración establecido y calcula el umbral de detección en base a ello.
 */
void calculo_umbrales_sensores() {
  for (int sensor = 0; sensor < NUMERO_SENSORES; sensor++) {
    umbralesCalibracionSensores[sensor] = (valoresCalibracionMinimos[sensor] + valoresCalibracionMaximos[sensor]) / 2.0f;
    umbralesCalibracionSensores[sensor] = map(umbralesCalibracionSensores[sensor], valoresCalibracionMinimos[sensor], valoresCalibracionMaximos[sensor], valorCalibradoMinimo, valorCalibradoMaximo);

    Serial.print("umbralesCalibracionSensores[");
    Serial.print(sensor);
    Serial.print("] = ");
    Serial.print(umbralesCalibracionSensores[sensor]);
    Serial.print(";\n");
  }
}
