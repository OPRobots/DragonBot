/**
 * Función para iniciar todos los componentes del robot.
 */
void inicia_todo() {
  Serial.begin(9600);
  while (!Serial) {
  }
  inicia_sensores();
  delay(100);
  inicia_motores();
  delay(100);
  inicia_encoders();
  delay(100);
  inicia_leds();
  delay(100);
  inicia_switch_boton();
  delay(100);
  inicia_I2C();
  delay(100);
}

/**
 * Registra los pines del Multiplexador para los sensores
 */
void inicia_sensores() {
  pinMode(MUX_CANAL_0, OUTPUT);
  pinMode(MUX_CANAL_1, OUTPUT);
  pinMode(MUX_CANAL_2, OUTPUT);
  pinMode(MUX_CANAL_3, OUTPUT);
  pinMode(MUX_LECTURA, INPUT);
  pinMode(SENSOR_FRONTAL, INPUT);
  pinMode(SENSOR_ROBOTRACER_DERECHO, INPUT);
  pinMode(SENSOR_ROBOTRACER_IZQUIERDO, INPUT);
}

/**
 * Registra los pines de los Puente-H para los motores y los inicia en parado.
 */
void inicia_motores() {
  pinMode(MOTOR_DERECHO_ADELANTE, OUTPUT);
  pinMode(MOTOR_DERECHO_ATRAS, OUTPUT);
  pinMode(MOTOR_DERECHO_PWM, OUTPUT);
  pinMode(MOTOR_IZQUIERDO_ADELANTE, OUTPUT);
  pinMode(MOTOR_IZQUIERDO_ATRAS, OUTPUT);
  pinMode(MOTOR_IZQUIERDO_PWM, OUTPUT);

  //	Inicializa los motores a estado parado
  digitalWrite(MOTOR_DERECHO_ADELANTE, LOW);
  digitalWrite(MOTOR_DERECHO_ATRAS, LOW);
  digitalWrite(MOTOR_IZQUIERDO_ADELANTE, LOW);
  digitalWrite(MOTOR_IZQUIERDO_ATRAS, LOW);
}

/**
 * Configura las interrupciones para lectrua de los encoders
 */
void inicia_encoders() {
  pinMode(MOTOR_DERECHO_ENCODER_A, INPUT_PULLUP);
  pinMode(MOTOR_DERECHO_ENCODER_B, INPUT_PULLUP);
  pinMode(MOTOR_IZQUIERDO_ENCODER_A, INPUT_PULLUP);
  pinMode(MOTOR_IZQUIERDO_ENCODER_B, INPUT_PULLUP);

  attachInterrupt(MOTOR_DERECHO_ENCODER_A, encoder_derecho_A, CHANGE);
  attachInterrupt(MOTOR_DERECHO_ENCODER_B, encoder_derecho_B, CHANGE);
  attachInterrupt(MOTOR_IZQUIERDO_ENCODER_A, encoder_izquierdo_A, CHANGE);
  attachInterrupt(MOTOR_IZQUIERDO_ENCODER_B, encoder_izquierdo_B, CHANGE);
}

/**
 * Inicia el protocolo I2C mediante la librería Wire.h y el MPU9250
 */
void inicia_I2C() {
  Wire.begin();
  delay(100);
  inicia_MPU9250();
}

/**
 * Regitra los pines de los Leds
 */
void inicia_leds() {
  pinMode(RED_RGB_R, OUTPUT);
  pinMode(RED_RGB_G, OUTPUT);
  pinMode(RED_RGB_B, OUTPUT);
  set_color_RGB(0, 0, 0);
}

/**
 * Registra los pines del Switch y Botón
 */
void inicia_switch_boton() {
  pinMode(BTN, INPUT_PULLUP);
  pinMode(BTN_CRUCETA, INPUT);
}

/**
 * Registra los demás componentes
 */
void inicia_misc() {
  pinMode(CHOP_PIN, OUTPUT);
  digitalWrite(CHOP_PIN, HIGH);
  pinMode(NIVEL_BATERIA, INPUT);
}

/**
 * Configura el ESC del Brushless para la succión
 */
void inicia_brushless() {
  set_color_RGB(0, 0, 25);
  pinMode(MOTOR_SUCCION, OUTPUT);
  long millisIni = millis();
  do {
    digitalWrite(MOTOR_SUCCION, HIGH);
    delay(2);
    digitalWrite(MOTOR_SUCCION, LOW);
    delay(18);
  } while (millis() < millisIni + 5000);
  millisIni = millis();
  do {
    digitalWrite(MOTOR_SUCCION, HIGH);
    delay(1);
    digitalWrite(MOTOR_SUCCION, LOW);
    delay(19);
  } while (millis() < millisIni + 5000);
  set_color_RGB(0, 0, 0);
}

/**
 * Configuración de todos los Timers en uso
 */
void inicia_timers() {
  inicia_timer_PID();
  inicia_timer_Brushless();
}
