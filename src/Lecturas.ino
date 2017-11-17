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
* Lectura escalada de todos los sensores de linea consecutivamente guardandolo en el array de valores
* "line_sensor_values" para su posterior uso.
* NOTA: Se usa la funcion de lectura individual de sensor, pasándole el índice del sensor a leer.
* @param line_sensor_values Array de valores de todos los sensores.
*/
void read_scaled_line_sensors(int line_sensor_values[]){
  for (int sensor = 0; sensor < NUM_SENSORS; sensor++) {
    line_sensor_values[sensor] = read_scaled_line_sensor(sensor);
  }
}

/**
* Lectura escalada e individual de un sensor de línea.
* @param  sensor Índice del array de pines "sensorPins" cuyo sensor se va a leer
* @return        Lectura del sensor.
*/
int read_scaled_line_sensor(int sensor){
  return map(analogRead(sensorPins[sensor]), minVals[sensor], maxVals[sensor], 0, 4095);
}

/**
* Calibración inicial de los sensores de línea
* @param doCalibrate Bandera para evitar calibración en caso de haber introducido anteriormente los datos.
*/
void calibrate_line_sensors(bool doCalibrate){
  if(doCalibrate){
    calibrate_minimum_maximum_value_line_sensors();
    if(MODO_CIRCUITO == MODO_DEGRADADO){
      while (digitalRead(BTN)) {};
      delay(500);
      calibrate_minimum_maximum_average_value_line_sensors();
    }
  }
}

/**
* Calibración de mínimos y máximos general de los sensores, para normalizar las futuras lecturas en base
* al rango de valores que puede tomar en la pista cada uno de los sensores.
*/
void calibrate_minimum_maximum_value_line_sensors(){
  long millis_inicial = 0;
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
  }while((millis()-millis_inicial)<CALIBRATION_TIME);
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

/**
* Lectura filtrada de los sensores de robots.
*/
void read_robots_sensors(bool filtrado){
  if(filtrado){
    filtro_sensor_izquierdo.Filter(sensor_izquierdo.readRangeSingleMillimeters());
    filtro_sensor_frontal.Filter(sensor_frontal.readRangeSingleMillimeters());
    filtro_sensor_derecho.Filter(sensor_derecho.readRangeSingleMillimeters());

    robots_sensor_values[0] = filtro_sensor_izquierdo.Current();
    robots_sensor_values[1] = filtro_sensor_frontal.Current();
    robots_sensor_values[2] = filtro_sensor_derecho.Current();
  }else{
    robots_sensor_values[0] = sensor_izquierdo.readRangeSingleMillimeters();
    robots_sensor_values[1] = sensor_frontal.readRangeSingleMillimeters();
    robots_sensor_values[2] = sensor_derecho.readRangeSingleMillimeters();
  }

}


/**
* Obtiene la posición del robot sobre la línea o el degradado, retornando un valor analogico desde -4000 hasta +4000.
* @param  last_position Última posición conocida del robot.
* @return               Posición del robot.
*/
double get_position(double last_position){
  switch (MODO_CIRCUITO) {
    case MODO_LINEA:

    // Número de sensores detectando negro. Por defecto todos
    int sensores_negro;
    sensores_negro = NUM_SENSORS;

    for(int sensor = 0;sensor < NUM_SENSORS;sensor++){
      // Mapea la lectura de los sensores a 0-4095, en función de sus maximos y minimos de cada sensor
      line_sensor_values[sensor] = map(analogRead(sensorPins[sensor]), minVals[sensor], maxVals[sensor], 0, 4000);

      // Satura la lectura de los sensores a blanco y negro absoluto
      if (line_sensor_values[sensor] > maxVal) {
        line_sensor_values[sensor] = 4000;
      } else if (line_sensor_values[sensor] < minVal) {
        line_sensor_values[sensor] = 0;
        sensores_negro--;
      }

    }

    /////////////////////////
    // Cálculo de Posición //
    /////////////////////////

    // Suma de todos los valores de los sensores pesados
    unsigned long linea;
    // Suma de todos los valores de los sensores sin pesar
    unsigned long valores;
    // Posición real en la que se encuentra el robot. Por defecto es la anterior.
    double posicion_real;

    linea = 0;
    valores = 0;
    posicion_real = 0;

    for (int sensor = 0; sensor < NUM_SENSORS; sensor++) {
      linea += (sensor + 1) * line_sensor_values[sensor] * 1000L;
      valores += (long)line_sensor_values[sensor];
    }

    if(!stop_emergencia && (sensores_negro == 0 ||sensores_negro == NUM_SENSORS) && millis_stop == 0){
      millis_stop = millis();
    }else if (!stop_emergencia && !(sensores_negro == 0 ||sensores_negro == NUM_SENSORS)){
      millis_stop = 0;
    }
    if(millis_stop> 0 && millis()-millis_stop > 500 && velBase > 0){
      stop_emergencia = true;
    }


    if (sensores_negro > 0) {

      // Calcula la posición real sobre la línea.
      posicion_real = ((linea / valores) - ((NUM_SENSORS + 1) * (double)(1000 / 2)));
      return posicion_real;
    } else {
      // Si no hay ningún sensor detectando la linea, simula un sensor extra en el lado adecuado.
      return (last_position > 0) ? (1000 * (NUM_SENSORS + 1) / 2) : -(1000 * (NUM_SENSORS + 1) / 2);
    }
    break;
    case MODO_DEGRADADO:
    break;
  }
}


void encoder_derecho(){
  ticks_derecho++;
}

void encoder_izquierdo(){
  ticks_izquierdo++;
}
