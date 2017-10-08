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
  for (int sensor = 0; sensor < NUM_SENSORS; sensor++) {
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

/**
 * Calibración inicial de los sensores de línea
 * @param doCalibrate Bandera para evitar calibración en caso de haber introducido anteriormente los datos.
 */
void calibrate_line_sensors(bool doCalibrate){
  if(doCalibrate){
    calibrate_minimum_maximum_value_line_sensors();
    if(MODO_CIRCUITO == MODO_DEGRADADO){
      calibrate_minimum_maximum_average_value_line_sensors();
    }
  }
}

/**
* Calibración de mínimos y máximos general de los sensores, para normalizar las futuras lecturas en base
* al rango de valores que puede tomar en la pista cada uno de los sensores.
*/
void calibrate_minimum_maximum_value_line_sensors(){
  long millis_inicial = millis();
  digitalWrite(LED, LOW);
  do{
    for (int sensor = 0; sensor < NUM_SENSORS; sensor++) {
      short value = read_raw_line_sensor(sensor);
      if (value < minVals[sensor]) {
        minVals[sensor] = value;
      } else if (value > maxVals[sensor]) {
        maxVals[sensor] = value;
      }
    }
  }while((millis()-millis_inicial)>CALIBRATION_TIME);
  digitalWrite(LED, HIGH);
  delay(100);
}

/**
* Calibración especial de los sensores de línea para circuitos de degradados:
*  A partir de la calibración general de máximos y mínimos, se obtiene el valor máximo y mínimo escalado de cada sensor de línea y,
*  posteriormente, obtiene la media mínima y máxima de todos los sensores de línea, las cuales coincidirían con el centro y los extremos
*  de la pista respectivamente.
*/
void calibrate_minimum_maximum_average_value_line_sensors(){
  long millis_inicial = millis();
  digitalWrite(LED, LOW);
  do{
    for (int sensor = 0; sensor < NUM_SENSORS; sensor++) {
      int value = 4095-read_raw_line_sensor(sensor);
      if (value < minVals[sensor]) {
        minMappedVals[sensor] = value;
      } else if (value > maxVals[sensor]) {
        maxMappedVals[sensor] = value;
      }
    }
  }while ((millis()-millis_inicial) < CALIBRATION_TIME);
  for (int sensor = 0; sensor < NUM_SENSORS; sensor++) {
    mediaMax += maxMappedVals[sensor];
    mediaMin += minMappedVals[sensor];
    if (sensor == NUM_SENSORS - 1) {
      mediaMax = (float)mediaMax / (float)NUM_SENSORS;
      mediaMin = (float)mediaMin / (float)NUM_SENSORS;
    }
  }
  digitalWrite(LED, HIGH);
  delay(100);
}
