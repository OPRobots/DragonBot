///////////////////////////////////////////////////////////////////////
//Este fichero contendrá todos los procesos de lecturas de sensores, //
//así como la calibración inicial y las comprobaciones de rivales.   //
///////////////////////////////////////////////////////////////////////

/**
 * Lectura en crudo de todos los sensores de linea consecutivamente guardandolo en el array de valores
 * "line_sensor_values" para su posterior uso.
 * NOTA: Se usa la funcion de lectura individual de sensor, pasándole el índice del sensor a leer.
 * @param line_sensor_values Array de valores de todos los sensores.
 */
void read_raw_line_sensors(int line_sensor_values[]){
  for (byte sensor = 0; sensor < NUM_SENSORS; sensor++) {
    line_sensor_values[sensor] = read_raw_line_sensor(sensor);
  }
}

/**
 * Lectura en crudo e individual de un sensor de línea.
 * @param  sensor Índice del array de pines "sensorPins" cuyo sensor se va a leer
 * @return        Lectura del sensor.
 */
int read_raw_line_sensor(int sensor){
  return analogRead(sensorPins[sensor]);
}
