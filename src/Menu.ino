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
        set_color_RGB(0, 5, 0);
        if (VELOCIDAD == MODO_MS) {
          velocidadMsIdealBase = 1.25;
        } else {
          velocidadPercentBase = 20;
        }
        #ifdef MORRO_ESTRECHO
                kp = 0.12f;
                ki = 0;
                kd = 5.0f;
        #elif MORRO_ANCHO
                kp = 0.2f;
                ki = 0;
                kd = 25.0f;
        #endif
        break;
      case 1:
        set_color_RGB(0, 10, 0);
        if (VELOCIDAD == MODO_MS) {
          velocidadMsIdealBase = 1.5;
        } else {
          velocidadPercentBase = 38;
        }
        #ifdef MORRO_ESTRECHO
                kp = 0.12f;
                ki = 0;
                kd = 5.0f;
        #elif MORRO_ANCHO
                kp = 0.2f;
                ki = 0;
                kd = 25.0f;
        #endif
        break;
      case 2:
        set_color_RGB(0, 255, 0);
        if (VELOCIDAD == MODO_MS) {
          velocidadMsIdealBase = 1.75;
        } else {
          velocidadPercentBase = 44;
        }
        #ifdef MORRO_ESTRECHO
                kp = 0.12f;
                ki = 0;
                kd = 5.0f;
        #elif MORRO_ANCHO
                kp = 0.2f;
                ki = 0;
                kd = 25.0f;
        #endif
        break;
      case 3:
        set_color_RGB(10, 10, 0);
        if (VELOCIDAD == MODO_MS) {
          velocidadMsIdealBase = 2.0;
        } else {
          velocidadPercentBase = 50;
        }
        #ifdef MORRO_ESTRECHO
                kp = 0.12f;
                ki = 0;
                kd = 5.0f;
        #elif MORRO_ANCHO
                kp = 0.2f;
                ki = 0;
                kd = 25.0f;
        #endif
        break;
      case 4:
        set_color_RGB(255, 225, 0);
        if (VELOCIDAD == MODO_MS) {
          velocidadMsIdealBase = 2.25;
        } else {
          velocidadPercentBase = 56;
        }
        #ifdef MORRO_ESTRECHO
                kp = 0.12f;
                ki = 0;
                kd = 5.0f;
        #elif MORRO_ANCHO
                kp = 0.2f;
                ki = 0;
                kd = 25.0f;
        #endif
        break;
      case 5:
        set_color_RGB(10, 0, 0);
        if (VELOCIDAD == MODO_MS) {
          velocidadMsIdealBase = 2.5;
        } else {
          velocidadPercentBase = 58;
        }
        #ifdef MORRO_ESTRECHO
                kp = 0.12f;
                ki = 0;
                kd = 5.0f;
        #elif MORRO_ANCHO
                kp = 0.2f;
                ki = 0;
                kd = 25.0f;
        #endif
        break;
      case 6:
        set_color_RGB(255, 0, 0);
        if (VELOCIDAD == MODO_MS) {
          velocidadMsIdealBase = 2.75;
        } else {
          velocidadPercentBase = 66;
        }
        #ifdef MORRO_ESTRECHO
                kp = 0.12f;
                ki = 0;
                kd = 5.0f;
        #elif MORRO_ANCHO
                kp = 0.2f;
                ki = 0;
                kd = 25.0f;
        #endif
        break;
      case 7:
        set_color_RGB(10, 0, 10);
        if (VELOCIDAD == MODO_MS) {
          velocidadMsIdealBase = 3.0;
        } else {
          velocidadPercentBase = 74;
        }
        #ifdef MORRO_ESTRECHO
                kp = 0.12f;
                ki = 0;
                kd = 5.0f;
        #elif MORRO_ANCHO
                kp = 0.2f;
                ki = 0;
                kd = 25.0f;
        #endif
        break;
      case 8:
        set_color_RGB(255, 0, 255);
        if (VELOCIDAD == MODO_MS) {
          velocidadMsIdealBase = 3.25;
        } else {
          velocidadPercentBase = 82;
        }
        #ifdef MORRO_ESTRECHO
                kp = 0.12f;
                ki = 0;
                kd = 5.0f;
        #elif MORRO_ANCHO
                kp = 0.2f;
                ki = 0;
                kd = 25.0f;
        #endif
        break;
      case 9:
        set_color_RGB(255, 255, 255);
        if (VELOCIDAD == MODO_MS) {
          velocidadMsIdealBase = 3.5;
        } else {
          velocidadPercentBase = 90;
        }
        #ifdef MORRO_ESTRECHO
                kp = 0.12f;
                ki = 0;
                kd = 5.0f;
        #elif MORRO_ANCHO
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
        set_color_RGB(0, 10, 0);
        break;
      case 1:
        velocidadSuccionBase = LIPO == LIPO_2S ? 30 : 15;
        set_color_RGB(0, 225, 0);
        break;
      case 2:
        velocidadSuccionBase = LIPO == LIPO_2S ? 40 : 20;
        set_color_RGB(10, 10, 0);
        break;
      case 3:
        velocidadSuccionBase = LIPO == LIPO_2S ? 50 : 25;
        set_color_RGB(225, 225, 0);
        break;
      case 4:
        velocidadSuccionBase = LIPO == LIPO_2S ? 60 : 30;
        set_color_RGB(10, 0, 0);
        break;
      case 5:
        velocidadSuccionBase = LIPO == LIPO_2S ? 70 : 35;
        set_color_RGB(225, 0, 0);
        break;
      case 6:
        velocidadSuccionBase = LIPO == LIPO_2S ? 80 : 40;
        set_color_RGB(10, 0, 10);
        break;
      case 7:
        velocidadSuccionBase = LIPO == LIPO_2S ? 90 : 45;
        set_color_RGB(225, 0, 225);
        break;
      case 8:
        velocidadSuccionBase = LIPO == LIPO_2S ? 100 : 50;
        set_color_RGB(225, 225, 225);
        break;
      }
      velocidadSuccionBase = map(velocidadSuccionBase, 0, 100, 0, 255);
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
    if (valorBtn > (crucetaCombinaciones[combinacion] - 200) && valorBtn < (crucetaCombinaciones[combinacion] + 200)) {
      return crucetaCombinaciones[combinacion];
    }
  }
  return 0;
}