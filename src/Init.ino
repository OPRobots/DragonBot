/**
 * Función para iniciar todos los componentes del robot.
 */
void inicia_todo(){
  CalibracionPID.setMaxIdeal(posicionMaxima);
  CalibracionPID.setMinIdeal(posiciconMinima);
  Serial.begin(115200);
  while(!Serial){}
  inicia_sensores();
  delay(100);
  inicia_motores();
  delay(100);
  inicia_leds();
  delay(100);
  inicia_switch_boton();
  delay(100);
}

/**
 * Registra los pines del Multiplexador para los sensores
 */
void inicia_sensores(){
  pinMode(MUX_CANAL_0   , OUTPUT);
	pinMode(MUX_CANAL_1   , OUTPUT);
	pinMode(MUX_CANAL_2   , OUTPUT);
	pinMode(MUX_CANAL_3   , OUTPUT);
	pinMode(MUX_LECTURA   , INPUT);
	pinMode(SENSOR_FRONTAL   , INPUT);
	pinMode(SENSOR_ROBOTRACER_DERECHO   , INPUT);
	pinMode(SENSOR_ROBOTRACER_IZQUIERDO   , INPUT);
}

/**
 * Registra los pines de los Puente-H para los motores y los inicia en parado.
 */
void inicia_motores(){
  pinMode(MOTOR_DERECHO_ADELANTE   , OUTPUT);
	pinMode(MOTOR_DERECHO_ATRAS      , OUTPUT);
	pinMode(MOTOR_DERECHO_PWM        , OUTPUT);
	pinMode(MOTOR_IZQUIERDO_ADELANTE , OUTPUT);
	pinMode(MOTOR_IZQUIERDO_ATRAS	   , OUTPUT);
	pinMode(MOTOR_IZQUIERDO_PWM	     , OUTPUT);

  //	Inicializa los motores a estado parado
	digitalWrite(MOTOR_DERECHO_ADELANTE    , LOW);
	digitalWrite(MOTOR_DERECHO_ATRAS       , LOW);
	digitalWrite(MOTOR_IZQUIERDO_ADELANTE  , LOW);
	digitalWrite(MOTOR_IZQUIERDO_ATRAS     , LOW);
}

/**
 * Regitra los pines de los Leds
 */
void inicia_leds(){
	pinMode(RED_RGB_R  , OUTPUT);
	pinMode(RED_RGB_G  , OUTPUT);
	pinMode(RED_RGB_B  , OUTPUT);
}

/**
 * Registra los pines del Switch y Botón
 */
void inicia_switch_boton(){
	pinMode(BTN , INPUT_PULLUP);
	pinMode(BTN_CRUCETA , INPUT);
}

/**
 * Registra los demás componentes
 */
void inicia_misc(){
  pinMode(CHOP_PIN, OUTPUT);
	pinMode(NIVEL_BATERIA , INPUT);
}
