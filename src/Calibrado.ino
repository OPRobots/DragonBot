/**
 * Función principal de calibrado de sensores.
 * Selecciona la función de calibrado adecuada para cada tipo de pista.
 */
void calibra_sensores() {
  switch (PISTA) {
  case MODO_LINEA:
    valorCalibradoMaximo = CALIBRADO_MAXIMO_SENSORES_LINEA;
    valorCalibradoMinimo = CALIBRADO_MINIMO_SENSORES_LINEA;
    posicionMaxima = (NUMERO_SENSORES + 1) * 500;
    posicionMinima = -(NUMERO_SENSORES + 1) * 500;
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

  bool calibracion_ok = true;
  for (int sensor = 0; sensor < NUMERO_SENSORES; sensor++) {

    if (abs(valoresCalibracionMaximos[sensor] - valoresCalibracionMinimos[sensor]) < 1000) {
      calibracion_ok = false;
    }

    umbralesCalibracionSensores[sensor] = (valoresCalibracionMinimos[sensor] + valoresCalibracionMaximos[sensor]) / 2.0f;

    Serial.print("umbralesCalibracionSensores[");
    Serial.print(sensor);
    Serial.print("] = ");
    Serial.print(umbralesCalibracionSensores[sensor]);
    Serial.print(";\n");
  }

  if (calibracion_ok) {
    set_color_RGB(0, 5, 0);
  } else {
    set_color_RGB(5, 0, 0);
  }
  while (!btn_pulsado()) {
    delay(100);
    while (btn_pulsado()) {
      set_color_RGB(0, 0, 0);
      return;
    }
  }
}
