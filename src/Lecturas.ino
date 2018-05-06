/**
 * Función de lectura de los sensores de pista.
 * @param  canalMux Canal del Multiplexador correspondiente al sensor que se desea leer.
 * @return [int]    Lectura analógica del sensor indicado.
 */
int mux_analog_read(byte canalMux) {
  digitalWrite(MUX_CANAL_0, bitRead(canalMux, 0));
  digitalWrite(MUX_CANAL_1, bitRead(canalMux, 1));
  digitalWrite(MUX_CANAL_2, bitRead(canalMux, 2));
  digitalWrite(MUX_CANAL_3, bitRead(canalMux, 3));
  valoresSensoresRaw[canalMux] = analogRead(MUX_LECTURA);
  return valoresSensoresRaw[canalMux];
}

/**
 * Función de lectura del sensor indicado mapeando los máximos y mínimos del mismo
 * @param  canalMux Canal del Multiplexador correspondiente al sensor que se desea leer.
 * @param  sensor 	Índice del array de sensores correspondiente al sensor leído actualmente.
 * @return [int]    Lectura analógica del sensor indicado.
 */
int mux_analog_read_map(byte canalMux, int sensor) {
  return map(mux_analog_read(canalMux), valoresCalibracionMinimos[sensor], valoresCalibracionMaximos[sensor], valorCalibradoMinimo, valorCalibradoMaximo);
}

/**
 * Función de lectura de todos los sensores, mapeando el resultado a los valores máximos y mínimos de la calibración.
 */
void lectura_sensores_calibrados() {
  for (int sensor = 0; sensor < NUMERO_SENSORES; sensor++) {
    valoresSensores[sensor] = mux_analog_read_map(pinesSensores[sensor], sensor);
    if (valoresSensores[sensor] >= valorSaturacionAlto) {
      valoresSensores[sensor] = valorCalibradoMaximo;
    }
    if (valoresSensores[sensor] <= valorSaturacionBajo) {
      valoresSensores[sensor] = valorCalibradoMinimo;
    }
    if (LINEA == LINEA_BLANCA) {
      valoresSensores[sensor] = valorCalibradoMaximo - valoresSensores[sensor];
    }
  }
}

/**
 * Función de lectura del botón, pasando el valor a lógica positiva.
 * @return [bool]		Indica si el botón está pulsado.
 */
bool btn_pulsado() {
  return digitalRead(BTN);
}
