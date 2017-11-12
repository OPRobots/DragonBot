////////////////////////////////////////////////////////////////
//Este fichero contendrá todas las funciones de velocidades y //
//aceleraciones que tendrá el robot.                          //
////////////////////////////////////////////////////////////////

/**
 * Realiza el cálculo de la corrección de los motores mediante el PID
 * @param  position Posición del cálculo anterior
 * @return          Posición actual del robot
 */
double calc_PID(double position) {
  double p = 0;
  double i = 0;
  double d = 0;
  double error = ideal - position;

  // Proporcional: constante · desviación
  p = kp/100.0 * error;

  // Derivada: constante · derivada del error
  if(last_pid_calc>0)
  // d = kd/100.0 * ((error - last_error) / (micros() - last_pid_calc));
  d = kd/100.0 * (error - last_error);

  if((error < 100 && ki!=0) && (abs(sum_error) < 3000 || ((sum_error > 0 && error < 0) || (sum_error < 0 && error > 0)))){
    sum_error += error;
  }
  if(error > 100){
    sum_error = 0;
  }

  // i = ki/100.0 * sum_error;
  
  last_pid_calc = micros();
  last_error = error;

  return p + i + d;
}
