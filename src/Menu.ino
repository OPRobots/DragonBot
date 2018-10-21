void btn_cruceta_simple() {
  bool btnPulsado = false;
  switch (calcular_btn_cruceta(analogRead(BTN_CRUCETA))) {
  case CRUCETA_ARRIBA:
    velocidadMsIdealBase = 2.5f;
    velocidadSuccionBase = LIPO == LIPO_2S ? 120 : 85;
    kp = 0.15f;
    ki = 0;
    kd = 20.0f;
    kpFrontal = 0;
    kiFrontal = 0;
    kdFrontal = 0;
    set_color_RGB(255, 0, 0);
    btnPulsado = true;
    break;
  case CRUCETA_ABAJO:
    velocidadMsIdealBase = 3.0f;
    velocidadSuccionBase = 150;
    kp = 0.03f;
    ki = 0;
    kd = 4.0f;
    kpFrontal = 0;
    kiFrontal = 0;
    kdFrontal = 0;
    set_color_RGB(255, 0, 255);
    btnPulsado = true;
    break;
  case CRUCETA_DERECHA:
    velocidadMsIdealBase = 1.5f;
    velocidadSuccionBase = LIPO == LIPO_2S ? 25 : 17;
    kp = 0.15f;
    ki = 0;
    kd = 20.0f;
    kpFrontal = 0;
    kiFrontal = 0;
    kdFrontal = 0;
    set_color_RGB(0, 255, 0);
    btnPulsado = true;
    break;
  case CRUCETA_IZQUIERDA:
    velocidadMsIdealBase = 2.0f;
    velocidadSuccionBase = LIPO == LIPO_2S ? 80 : 55;
    kp = 0.15f;
    ki = 0;
    kd = 20.0f;
    kpFrontal = 0;
    kiFrontal = 0;
    kdFrontal = 0;
    set_color_RGB(255, 255, 0);
    btnPulsado = true;
    break;
  }
  if (btnPulsado) {
    delay(1000);
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