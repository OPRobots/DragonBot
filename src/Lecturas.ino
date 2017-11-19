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
  return map(analogRead(sensorPins[sensor]), minVals[sensor], maxVals[sensor], 0, 4000);
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
  digitalWrite(LED, HIGH);
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
      int value = read_scaled_line_sensor(sensor);
      if (value < minMappedVals[sensor]) {
        minMappedVals[sensor] = value;
      } else if (value > maxMappedVals[sensor]) {
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
* Lectura de los sensores láser de robots.
*/
void read_laser_sensor(byte sensor){
  switch (sensor) {
    case LASER_IZQUIERDO:
    robots_sensor_values[LASER_IZQUIERDO] = sensor_izquierdo.readRangeSingleMillimeters();
    break;
    case LASER_FRONTAL:
    if(lectura_frontal_laser.check()){
      lectura_frontal_laser.reset();
      robots_sensor_values[LASER_FRONTAL] = sensor_frontal.readRangeContinuousMillimeters();
    }
    break;
    case LASER_DERECHO:
    robots_sensor_values[LASER_DERECHO] = sensor_derecho.readRangeSingleMillimeters();
    break;
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
      // Mapea la lectura de los sensores a 0-4000, en función de sus maximos y minimos de cada sensor
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

    for(int sensor = 0;sensor < NUM_SENSORS;sensor++){
      // Mapea la lectura de los sensores a 0-4000, en función de sus maximos y minimos de cada sensor
      line_sensor_values[sensor] = read_scaled_line_sensor(sensor);
    }

    long posicion_real_deg = 0;
    int num_sensors_deg = 0;
    float sensor_average = 0.0f;
    int inicio_for;
    int fin_for;
    int first_deg_sensor = -1;
    int last_deg_sensor = -1;
    if(last_position>=0){
      inicio_for = 0;
      fin_for = NUM_SENSORS;
    }else{

    }
    for(int sensor = 0;sensor < NUM_SENSORS;sensor++){
      // if(line_sensor_values[sensor] >= minMappedVals[sensor] && line_sensor_values[sensor] <= maxMappedVals[sensor]){
      if(abs(last_position)<400 || sensorEnPista(sensor, last_position, line_sensor_values)){
        num_sensors_deg++;
        sensor_average+=line_sensor_values[sensor];
        if(first_deg_sensor < 0){
          first_deg_sensor = sensor;
        }else{
          last_deg_sensor = sensor;
        }
      }
    }
    if(last_deg_sensor < 0){
      last_deg_sensor = first_deg_sensor;
    }

    if(num_sensors_deg>0){
      sensor_average = sensor_average/(float)num_sensors_deg;
      sensor_average = map(sensor_average, mediaMin, mediaMax, 0, 750);
      pid_calibrate.setMinIdeal(-750);
      pid_calibrate.setMaxIdeal(750);
      if(num_sensors_deg>1){
        if (line_sensor_values[first_deg_sensor] < line_sensor_values[last_deg_sensor]) {
          posicion_real_deg = sensor_average;
        }else{
          posicion_real_deg = -sensor_average;
        }
      }else{
        if(last_position>=0){
          posicion_real_deg = sensor_average;
        }else{
          posicion_real_deg = -sensor_average;
        }
      }
    }else{
      posicion_real_deg = last_position;
    }



    if(num_sensors_deg != NUM_SENSORS && abs(last_position)>400 && abs(last_position-posicion_real_deg) > 400){
      posicion_real_deg = last_position;
    }

    if(idealInicial==0){
      idealInicial = posicion_real_deg;
      id_ob = idealInicial;
      ideal = id_ob;
    }

    filtroDegradado.Filter(posicion_real_deg);

    return filtroDegradado.Current();

    break;
  }
}

bool sensorEnPista(int sensor, int last_position, int line_sensor_values[] ){
  if(line_sensor_values[sensor] < minMappedVals[sensor] && line_sensor_values[sensor] > maxMappedVals[sensor]){
    return false;
  }
  // Está por la derecha
  if(last_position >= 0){
    // No es el primer sensor
    if(sensor > 0){
      if(line_sensor_values[sensor] > line_sensor_values[sensor-1]){
        return true;
      }else{
        return false;
      }
      // Es el primer sensor
    }else{
      if(line_sensor_values[sensor]< line_sensor_values[sensor+1]){
        return true;
      }else{
        return false;
      }
    }
    // Está por la izquierda
  }else{
    // No es el primer sensor
    if(sensor > 0){
      if(line_sensor_values[sensor] < line_sensor_values[sensor-1]){
        return true;
      }else{
        return false;
      }
      // Es el primer sensor
    }else{
      if(line_sensor_values[sensor] > line_sensor_values[sensor+1]){
        return true;
      }else{
        return false;
      }
    }
  }


}


void encoder_derecho(){
  ticks_derecho++;
}

void encoder_izquierdo(){
  ticks_izquierdo++;
}
