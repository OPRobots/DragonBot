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
        velocidadMsIdealBase = 1.25;
        #ifdef DRAGON_A
        kp = 0.10f;
        ki = 0;
        kd = 5.0f;
        #elif DRAGON_B
        kp = 0.2f;
        ki = 0;
        kd = 25.0f;
        #endif
        break;
      case 1:
        set_color_RGB(63, 189, 0);
        velocidadMsIdealBase = 1.5;
        #ifdef DRAGON_A
        kp = 0.10f;
        ki = 0;
        kd = 5.0f;
        #elif DRAGON_B
        kp = 0.2f;
        ki = 0;
        kd = 25.0f;
        #endif
        break;
      case 2:
        set_color_RGB(126, 126, 0);
        velocidadMsIdealBase = 1.75;
        #ifdef DRAGON_A
        kp = 0.10f;
        ki = 0;
        kd = 5.0f;
        #elif DRAGON_B
        kp = 0.2f;
        ki = 0;
        kd = 25.0f;
        #endif
        break;
      case 3:
        set_color_RGB(189, 63, 0);
        velocidadMsIdealBase = 2.0;
        #ifdef DRAGON_A
        kp = 0.10f;
        ki = 0;
        kd = 5.0f;
        #elif DRAGON_B
        kp = 0.2f;
        ki = 0;
        kd = 25.0f;
        #endif
        break;
      case 4:
        set_color_RGB(255, 0, 0);
        velocidadMsIdealBase = 2.25;
        #ifdef DRAGON_A
        kp = 0.10f;
        ki = 0;
        kd = 5.0f;
        #elif DRAGON_B
        kp = 0.2f;
        ki = 0;
        kd = 25.0f;
        #endif
        break;
      case 5:
        set_color_RGB(255, 0, 63);
        velocidadMsIdealBase = 2.5;
        #ifdef DRAGON_A
        kp = 0.10f;
        ki = 0;
        kd = 5.0f;
        #elif DRAGON_B
        kp = 0.2f;
        ki = 0;
        kd = 25.0f;
        #endif
        break;
      case 6:
        set_color_RGB(255, 0, 126);
        velocidadMsIdealBase = 2.75;
        #ifdef DRAGON_A
        kp = 0.10f;
        ki = 0;
        kd = 5.0f;
        #elif DRAGON_B
        kp = 0.2f;
        ki = 0;
        kd = 25.0f;
        #endif
        break;
      case 7:
        set_color_RGB(255, 0, 189);
        velocidadMsIdealBase = 3.0;
        #ifdef DRAGON_A
        kp = 0.10f;
        ki = 0;
        kd = 5.0f;
        #elif DRAGON_B
        kp = 0.2f;
        ki = 0;
        kd = 25.0f;
        #endif
        break;
      case 8:
        set_color_RGB(255, 0, 255);
        velocidadMsIdealBase = 3.25;
        #ifdef DRAGON_A
        kp = 0.10f;
        ki = 0;
        kd = 5.0f;
        #elif DRAGON_B
        kp = 0.2f;
        ki = 0;
        kd = 25.0f;
        #endif
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
        velocidadSuccionBase = LIPO == LIPO_2S ? 30 : 15;
        set_color_RGB(63, 189, 0);
        break;
      case 2:
        velocidadSuccionBase = LIPO == LIPO_2S ? 35 : 20;
        set_color_RGB(126, 126, 0);
        break;
      case 3:
        velocidadSuccionBase = LIPO == LIPO_2S ? 40 : 25;
        set_color_RGB(189, 63, 0);
        break;
      case 4:
        velocidadSuccionBase = LIPO == LIPO_2S ? 45 : 30;
        set_color_RGB(255, 0, 0);
        break;
      case 5:
        velocidadSuccionBase = LIPO == LIPO_2S ? 50 : 35;
        set_color_RGB(255, 0, 63);
        break;
      case 6:
        velocidadSuccionBase = LIPO == LIPO_2S ? 55 : 40;
        set_color_RGB(255, 0, 126);
        break;
      case 7:
        velocidadSuccionBase = LIPO == LIPO_2S ? 60 : 45;
        set_color_RGB(255, 0, 189);
        break;
      case 8:
        velocidadSuccionBase = LIPO == LIPO_2S ? 65 : 50;
        set_color_RGB(255, 0, 255);
        break;
      }
      velocidadSuccionBase = map(velocidadSuccionBase, 0, 100, 0,255);
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