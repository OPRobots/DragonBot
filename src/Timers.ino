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
  TimerPID.setPeriod(500);
  TimerPID.setMode(TIMER_CH1, TIMER_OUTPUT_COMPARE);
  TimerPID.setCompare(TIMER_CH1, 1);
  TimerPID.attachInterrupt(1, handler_timer_PID);
  timerPID_pause = false;
  TimerPID.refresh();
  TimerPID.resume();
}

/**
 * Función a la que llama el Timer 2 para realizar el cálculo de posición y PID
 */
void handler_timer_PID() {
  if (timerPID_pause)
    return;
  posicionActual = calcular_posicion(posicionActual);
  correccion = calcular_PID(posicionActual);
  dar_velocidad(correccion);
}

/**
 * Función para pausar el Timer encargado del PID
 */
void pausa_timer_PID() {
  // TimerPID.pause();
  // TimerPID.refresh();
  timerPID_pause = true;
  errorAnterior = 0;
  correccion = 0;
  integralErrores = 0;
  velocidad = 0;
  velocidadSuccion = 0;
  velocidadMs = 0;
  velocidadMsIdealBase = 0;
  velocidadMsIdeal = 0;
  velocidadSuccionBase = 0;
  velocidadSuccion = 0;
  dar_velocidad(0);
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
  if (!ESCIniciado) {
    if (millisInitESC < 0) {
      pinMode(MOTOR_SUCCION, OUTPUT);
      millisInitESC = millis();
    }
    if (millis() < millisInitESC + 5000) {
      pinMode(MOTOR_SUCCION, OUTPUT);
      digitalWrite(MOTOR_SUCCION, HIGH);
      delayMicroseconds(2000);
      digitalWrite(MOTOR_SUCCION, LOW);
    } else if (millis() < millisInitESC + 10000) {
      digitalWrite(MOTOR_SUCCION, HIGH);
      delayMicroseconds(1000);
      digitalWrite(MOTOR_SUCCION, LOW);
    }
    if (millis() >= millisInitESC + 10000) {
      ESCIniciado = true;
    }
  } else {
    digitalWrite(MOTOR_SUCCION, HIGH);
    delayMicroseconds(map(velocidadSuccion, 0, 255, 1000, 2000));
    digitalWrite(MOTOR_SUCCION, LOW);

    if (competicionIniciada || velocidadSuccion > 0) {
      nivel_bateria(true);
    }

    if (competicionIniciada) {
      velocidadMs = calcular_velocidad();
      if (velocidadMsIdeal > 0 || velocidadMs > 0) {
        velocidad += ajustar_velocidad_encoders();
        if (velocidadMsIdeal == 0 && velocidadMs < 0.25) {
          velocidad = 0;
        }
      }
    }
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
