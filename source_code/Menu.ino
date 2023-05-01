/**
 * Función para modificar la velocidad y la velocidad de succión en función del botón pulsado.
 */
void btn_cruceta() {
  bool btnPulsado = true;
  int CRUCETA = calcular_btn_cruceta(analogRead(BTN_CRUCETA));
  switch (CRUCETA) {
  case CRUCETA_ARRIBA:
    velocidad_menu++;
    break;
  case CRUCETA_ABAJO:
    velocidad_menu--;
    break;
  case CRUCETA_DERECHA:
    velocidad_succion_menu++;
    break;
  case CRUCETA_IZQUIERDA:
    velocidad_succion_menu--;
    break;
  default:
    btnPulsado = false;
    break;
  }
  velocidad_menu = constrain(velocidad_menu, 0, NUMERO_VELOCIDADES - 1);
  velocidad_succion_menu = constrain(velocidad_succion_menu, 0, NUMERO_VELOCIDADES_SUCCION - 1);
  if (btnPulsado) {
    switch (CRUCETA) {
    case CRUCETA_ARRIBA:
    case CRUCETA_ABAJO:
      switch (velocidad_menu) {
      case 0:
        set_color_RGB(0, 255, 0);
        velocidadMsIdealBase = 1.0;
        kp = 0.15f;
        ki = 0;
        kd = 20.0f;
        break;
      case 1:
        set_color_RGB(63, 189, 0);
        velocidadMsIdealBase = 1.25;
        kp = 0.15f;
        ki = 0;
        kd = 20.0f;
        break;
      case 2:
        set_color_RGB(126, 126, 0);
        velocidadMsIdealBase = 1.5;
        kp = 0.15f;
        ki = 0;
        kd = 20.0f;
        break;
      case 3:
        set_color_RGB(189, 63, 0);
        velocidadMsIdealBase = 1.75;
        kp = 0.15f;
        ki = 0;
        kd = 20.0f;
        break;
      case 4:
        set_color_RGB(255, 0, 0);
        velocidadMsIdealBase = 2.0;
        kp = 0.15f;
        ki = 0;
        kd = 20.0f;
        break;
      case 5:
        set_color_RGB(255, 0, 63);
        velocidadMsIdealBase = 2.5;
        kp = 0.15f;
        ki = 0;
        kd = 20.0f;
        break;
      case 6:
        set_color_RGB(255, 0, 126);
        velocidadMsIdealBase = 2.75;
        kp = 0.15f;
        ki = 0;
        kd = 20.0f;
        break;
      case 7:
        set_color_RGB(255, 0, 189);
        velocidadMsIdealBase = 3.0;
        kp = 0.15f;
        ki = 0;
        kd = 20.0f;
        break;
      case 8:
        set_color_RGB(255, 0, 255);
        velocidadMsIdealBase = 3.25;
        kp = 0.15f;
        ki = 0;
        kd = 20.0f;
        break;
      }
      break;
    case CRUCETA_DERECHA:
    case CRUCETA_IZQUIERDA:
      switch (velocidad_succion_menu) {
      case 0:
        velocidadSuccionBase = LIPO == LIPO_2S ? 0 : 0;
        set_color_RGB(0, 255, 0);
        break;
      case 1:
        velocidadSuccionBase = LIPO == LIPO_2S ? 0 : 0;
        set_color_RGB(63, 189, 0);
        break;
      case 2:
        velocidadSuccionBase = LIPO == LIPO_2S ? 25 : 17;
        set_color_RGB(126, 126, 0);
        break;
      case 3:
        velocidadSuccionBase = LIPO == LIPO_2S ? 30 : 20;
        set_color_RGB(189, 63, 0);
        break;
      case 4:
        velocidadSuccionBase = LIPO == LIPO_2S ? 50 : 34;
        set_color_RGB(255, 0, 0);
        break;
      case 5:
        velocidadSuccionBase = LIPO == LIPO_2S ? 80 : 54;
        set_color_RGB(255, 0, 63);
        break;
      case 6:
        velocidadSuccionBase = LIPO == LIPO_2S ? 95 : 64;
        set_color_RGB(255, 0, 126);
        break;
      case 7:
        velocidadSuccionBase = LIPO == LIPO_2S ? 110 : 74;
        set_color_RGB(255, 0, 189);
        break;
      case 8:
        velocidadSuccionBase = LIPO == LIPO_2S ? 130 : 87;
        set_color_RGB(255, 0, 255);
        break;
      }
      break;
    }
    do {
      delay(150);
    } while (calcular_btn_cruceta(analogRead(BTN_CRUCETA)) > 0);
    set_color_RGB(0, 0, 0);
  }
}

/**
 * Función para encontrar el botón pulsado de la cruceta con un +-50 de error en las mediciones
 */
int calcular_btn_cruceta(int valorBtn) {
  for (int combinacion = 0; combinacion < NUMERO_COMBINACIONES; combinacion++) {
    if (valorBtn > (crucetaCombinaciones[combinacion] - 50) && valorBtn < (crucetaCombinaciones[combinacion] + 50)) {
      return crucetaCombinaciones[combinacion];
    }
  }
  return 0;
}