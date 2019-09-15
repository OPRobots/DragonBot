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
  asigna_prioridades_timers();
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
  pinMode(SENSOR_ROBOTRACER_DERECHO, INPUT);
  pinMode(SENSOR_ROBOTRACER_IZQUIERDO, INPUT);
}

/**
 * Registra los pines de los Puente-H para los motores y los inicia en parado.
 */
void inicia_motores() {
  pinMode(MOTOR_DERECHO_PWM, PWM);
  pinMode(MOTOR_IZQUIERDO_PWM, PWM);
}

/**
 * Configura las interrupciones para lectrua de los encoders
 */
void inicia_encoders() {
  if (VELOCIDAD == MODO_MS) {
    pinMode(MOTOR_DERECHO_ENCODER_A, INPUT_PULLUP);
    pinMode(MOTOR_DERECHO_ENCODER_B, INPUT_PULLUP);
    pinMode(MOTOR_IZQUIERDO_ENCODER_A, INPUT_PULLUP);
    pinMode(MOTOR_IZQUIERDO_ENCODER_B, INPUT_PULLUP);

    attachInterrupt(MOTOR_DERECHO_ENCODER_A, encoder_derecho_A, CHANGE);
    attachInterrupt(MOTOR_DERECHO_ENCODER_B, encoder_derecho_B, CHANGE);
    attachInterrupt(MOTOR_IZQUIERDO_ENCODER_A, encoder_izquierdo_A, CHANGE);
    attachInterrupt(MOTOR_IZQUIERDO_ENCODER_B, encoder_izquierdo_B, CHANGE);
  }
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
  pinMode(NIVEL_BATERIA, INPUT);
}

/**
 * Asigna las prioridades a los timers para asegurarse de que no se solapan las ejecuciones.
 */
void asigna_prioridades_timers() {
  // Prioridades máximas a Timers PWM
  nvic_irq_set_priority(NVIC_TIMER1_CC, 0);
  nvic_irq_set_priority(NVIC_TIMER4, 16);

  // Prioridad Timer PID
  nvic_irq_set_priority(NVIC_TIMER2, 24);

  // Prioridad Timer Succion
  nvic_irq_set_priority(NVIC_TIMER3, 32);
}
