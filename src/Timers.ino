/**
 * Función para configurar el Timer 2 para cálculo de posición y PID
 */
void inicia_timer_PID(){
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
  correccion = calcular_PID(posicionActual);
  if(!enCompeticion || competicionIniciada){
    dar_velocidad(correccion);
  }
}
