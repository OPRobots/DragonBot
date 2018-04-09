/**
 * Función para pausar todos los timers
 */
void pausa_timers() {
  pausa_timer_PID();
  pausa_timer_Brushless();
}

/**
 * Función para reanudar todos los timers
 */
void reanuda_timers() {
  reanuda_timer_PID();
  reanuda_timer_Brushless();
}

/**
 * Función para configurar el Timer 2 para cálculo de posición y PID
 */
void inicia_timer_PID() {
  TimerPID.pause();
  TimerPID.setPeriod(250);
  TimerPID.setMode(TIMER_CH1, TIMER_OUTPUT_COMPARE);
  TimerPID.setCompare(TIMER_CH1, 1);
  TimerPID.attachInterrupt(1, handler_timer_PID);
  TimerPID.refresh();
  TimerPID.resume();
}

/**
 * Función a la que llama el Timer 2 para realizar el cálculo de posición y PID
 */
void handler_timer_PID() {
  posicionActual = calcular_posicion(posicionActual);
  valorSensorFrontal =  analogRead(SENSOR_FRONTAL);
  correccion = calcular_PID(posicionActual);
  correccionFrontal = calcular_PID_frontal(valorSensorFrontal);

  if (!enCompeticion || competicionIniciada) {
    dar_velocidad(correccion, correccionFrontal);
  }
}

/**
 * Función para pausar el Timer encargado del PID
 */
void pausa_timer_PID() {
  TimerPID.pause();
}

/**
 * Función para reanudar el Timer encargado del PID. También lo refresca para iniciar limpio
 */
void reanuda_timer_PID() {
  TimerPID.refresh();
  TimerPID.resume();
}

/**
 * Función para configurar el Timer 3 para la creación de señal servo para el Brushless
 */
void inicia_timer_Brushless() {
  inicia_brushless();
  TimerBrushless.pause();
  TimerBrushless.setPeriod(20000);
  TimerBrushless.setMode(TIMER_CH1, TIMER_OUTPUT_COMPARE);
  TimerBrushless.setCompare(TIMER_CH1, 1);
  TimerBrushless.attachInterrupt(1, handler_timer_Brushless);
  TimerBrushless.refresh();
  TimerBrushless.resume();
}

/**
 * Función a la que llama el Timer 3 ajustar la configuración via Serial-BT
 */
void handler_timer_Brushless() {
  digitalWrite(MOTOR_SUCCION, HIGH);
  delayMicroseconds(map(velocidadSuccion, 0, 255, 1000, 2000));
  digitalWrite(MOTOR_SUCCION, LOW);

  contMapeo--;

  if (mapeoRealizado || velocidadSuccion > 0) {
    nivel_bateria(true);
  }
  if (contMapeo <= 0 && (!enCompeticion || competicionIniciada)) {
    contMapeo = 5;
    mapeado_circuito();
  }
  if (velocidadBase > 0) {
    velocidadActual = calcular_velocidad();
  } else {
    velocidadActual = 0;
  }
}

/**
 * Función para pausar el Timer encargado del Brushless
 */
void pausa_timer_Brushless() {
  TimerBrushless.pause();
}

/**
 * Función para reanudar el Timer encargado del Brushless. También lo refresca para iniciar limpio
 */
void reanuda_timer_Brushless() {
  TimerBrushless.refresh();
  TimerBrushless.resume();
}
