//////////////////////////////////////////////////////////////////////
// Este fichero contendrá todos los procesos iniciales del programa //
// que se deberán ejecutar una sola vez al inicio del mismo.        //
//////////////////////////////////////////////////////////////////////

/**
* Función principal de inicialización de componentes.
*/
void init_all(){
  init_serial();
  init_motors();
  init_encoders();
  init_sensors();
  init_led_buttons();
}

/**
 * Inicialización del puerto Serial.
 */
void init_serial(){
  Serial.begin(115200);
  while (!Serial){}
}

/**
* Inicialización de los motores.
*/
void init_motors(){
  pinMode(MOTOR_DERECHO_ADELANTE, OUTPUT);
  pinMode(MOTOR_DERECHO_ATRAS, OUTPUT);
  pinMode(MOTOR_IZQUIERDO_ADELANTE, OUTPUT);
  pinMode(MOTOR_IZQUIERDO_ATRAS, OUTPUT);

  pinMode(MOTOR_DERECHO_PWM, OUTPUT);
  pinMode(MOTOR_IZQUIERDO_PWM, OUTPUT);
  pinMode(MOTOR_RUN, OUTPUT);

  digitalWrite(MOTOR_DERECHO_ADELANTE, LOW);
  digitalWrite(MOTOR_DERECHO_ATRAS, LOW);
  digitalWrite(MOTOR_IZQUIERDO_ADELANTE, LOW);
  digitalWrite(MOTOR_IZQUIERDO_ATRAS, LOW);
  digitalWrite(MOTOR_RUN, HIGH);

  digitalWrite(MOTOR_DERECHO_PWM, LOW);
  digitalWrite(MOTOR_IZQUIERDO_PWM, LOW);
}

/**
* Inicialización de los encoders.
*/
void init_encoders(){
  pinMode(MOTOR_DERECHO_ENCODER,INPUT_PULLDOWN);
  attachInterrupt(MOTOR_DERECHO_ENCODER, encoder_derecho, CHANGE);

  pinMode(MOTOR_IZQUIERDO_ENCODER,INPUT_PULLDOWN);
  attachInterrupt(MOTOR_IZQUIERDO_ENCODER, encoder_izquierdo, CHANGE);
}

/**
* Inicialización de los sensores.
*/
void init_sensors(){
  init_IR_sensors();
  init_laser_sensors();
}

/**
* Inizializa los sensores IR QRE1113
*/
void init_IR_sensors(){
  for (byte sensor = 0; sensor < NUM_SENSORS; sensor++) {
    pinMode(sensorPins[sensor], INPUT);
  }
}

/**
* Inicizaliza los sensores láser VL53L0X
*/
void init_laser_sensors(){
  pinMode(SENSOR_IZQUIERDO_XSLEEP, OUTPUT);
  pinMode(SENSOR_FRONTAL_XSLEEP, OUTPUT);
  pinMode(SENSOR_DERECHO_XSLEEP, OUTPUT);
  digitalWrite(SENSOR_IZQUIERDO_XSLEEP, LOW);
  digitalWrite(SENSOR_FRONTAL_XSLEEP, LOW);
  digitalWrite(SENSOR_DERECHO_XSLEEP, LOW);

  delay(500);
  Wire.begin();

  pinMode(SENSOR_IZQUIERDO_XSLEEP, INPUT);
  delay(150);
  sensor_izquierdo.init(true);
  delay(100);
  sensor_izquierdo.setAddress((uint8_t)22);

  pinMode(SENSOR_FRONTAL_XSLEEP, INPUT);
  delay(150);
  sensor_frontal.init(true);
  delay(100);
  sensor_frontal.setAddress((uint8_t)25);

  pinMode(SENSOR_DERECHO_XSLEEP, INPUT);
  delay(150);
  sensor_derecho.init(true);
  delay(100);
  sensor_derecho.setAddress((uint8_t)28);

  sensor_izquierdo.setTimeout(5);
  sensor_frontal.setTimeout(5);
  sensor_derecho.setTimeout(5);
  sensor_frontal.startContinuous(200);
}

/**
* Inicialización de leds, botones, ...
*/
void init_led_buttons(){
  pinMode(LED, OUTPUT);

  pinMode(BTN, INPUT_PULLUP);

  pinMode(SW_1, INPUT_PULLUP);
  pinMode(SW_2, INPUT_PULLUP);
}
