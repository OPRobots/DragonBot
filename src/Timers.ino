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
  if (timerPID_pause) {
    return;
  }
  posicionActual = calcular_posicion(posicionActual);
  correccion = calcular_PID(posicionActual);
  dar_velocidad(correccion);
}

/**
 * Función para pausar el Timer encargado del PID
 */
void pausa_timer_PID() {
  timerPID_pause = true;
  errorAnterior = 0;
  correccion = 0;
  integralErrores = 0;
  velocidad = 0;
  velocidadSuccion = 0;
  velocidadMs = 0;
  velocidadMsIdealBase = 0;
  velocidadMsIdeal = 0;
  velocidadPercentBase = 0;
  velocidadSuccionBase = 0;
  velocidadSuccion = 0;
  dar_velocidad(0);
}

/**
 * Función para configurar el Timer 3 para la creación de señal servo para la Succion
 */
void inicia_timer_Succion() {
  TimerSuccion.pause();
  TimerSuccion.setPeriod(20000);
  TimerSuccion.setMode(TIMER_CH1, TIMER_OUTPUT_COMPARE);
  TimerSuccion.setCompare(TIMER_CH1, 1);
  TimerSuccion.attachInterrupt(1, handler_timer_Succion);
  TimerSuccion.refresh();
  TimerSuccion.resume();
}

void inicia_timer_Brushless() {
  TimerBrushless.pause();
  TimerBrushless.setPrescaleFactor(3);
  TimerBrushless.setPeriod(250);
  TimerBrushless.resume();
}

/**
 * Función a la que llama el Timer 3 ajustar la configuración via Serial-BT
 */
void handler_timer_Succion() {
  if (!ESCIniciado) {
    if (millisInitESC < 0) {
      pinMode(MOTOR_SUCCION, OUTPUT);
      millisInitESC = millis();

    }
    if (millis() < millisInitESC + 5000) {
      digitalWrite(MOTOR_SUCCION, HIGH);
      delayMicroseconds(1713);
      digitalWrite(MOTOR_SUCCION, LOW);

      velocidad = velocidadMaxima;
      dar_velocidad(0);

    }  else if (millis() < millisInitESC + 10000) {
      digitalWrite(MOTOR_SUCCION, HIGH);
      delayMicroseconds(854);
      digitalWrite(MOTOR_SUCCION, LOW);
      
      velocidad = -velocidadMaxima;
      dar_velocidad(0);
    } 
    if (millis() >= millisInitESC + 10000) {
      ESCIniciado = true;
      velocidad = 0;
      dar_velocidad(0);
    }
  } else {
    digitalWrite(MOTOR_SUCCION, HIGH);
    delayMicroseconds(map(velocidadSuccion, 0, 255, 854, 1713));
    digitalWrite(MOTOR_SUCCION, LOW);

    if (competicionIniciada || velocidadSuccion > 0) {
      nivel_bateria(true);
    }

    if (competicionIniciada && VELOCIDAD == MODO_MS) {
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
