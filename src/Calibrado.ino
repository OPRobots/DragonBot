/**
 * Función principal de calibrado de sensores.
 * Selecciona la función de calibrado adecuada para cada tipo de pista.
 */
void calibra_sensores() {
  switch (PISTA) {
  case MODO_LINEA: {
    valorSaturacionBajo = SATURACION_MINIMO_SENSORES_LINEA;
    valorSaturacionAlto = SATURACION_MAXIMO_SENSORES_LINEA;
    valorCalibradoMaximo = CALIBRADO_MAXIMO_SENSORES_LINEA;
    valorCalibradoMinimo = CALIBRADO_MINIMO_SENSORES_LINEA;
    posicionMaxima = 6500;
    posicionMinima = -6500;
    // calibrado_sensores_linea();
    calibrado_sensores_linea_digital();
    buscar_linea_principal();

    // TODO: Ajustar las saturaciones a partir de los valores de lectura de los sensores.

  } break;
  case MODO_DEGRADADO:
    valorSaturacionBajo = SATURACION_MINIMO_SENSORES_DEGRADADO;
    valorSaturacionAlto = SATURACION_MAXIMO_SENSORES_DEGRADADO;
    valorCalibradoMaximo = CALIBRADO_MAXIMO_SENSORES_DEGRADADO;
    valorCalibradoMinimo = CALIBRADO_MINIMO_SENSORES_DEGRADADO;
    posicionMaxima = 200;
    posicionMinima = -200;
    calibrado_sensores_degradado();
    break;
  }
  CalibracionPID.setMaxIdeal(posicionMaxima);
  CalibracionPID.setMinIdeal(posicionMinima);
  delay(300);
  ticksDerecho = 0;
  ticksIzquierdo = 0;
}

/**
 * Función de calibrado de sensores para línea.
 * Realiza una calibración simple de valores máximos y mínimos durante el tiempo de calibración establecido.
 */
void calibrado_sensores_linea() {
  if (CALIBRAR_SENSORES) {
    long tiempoInicial = millis();

    set_color_RGB(0, 0, 0);

    while (millis() < (tiempoInicial + TIEMPO_CALIBRADO)) {
      for (int sensor = 0; sensor < NUMERO_SENSORES; sensor++) {
        int lectura = mux_analog_read(pinesSensores[sensor]);
        if (lectura < valoresCalibracionMinimos[sensor]) {
          valoresCalibracionMinimos[sensor] = lectura;
        } else if (lectura > valoresCalibracionMaximos[sensor]) {
          valoresCalibracionMaximos[sensor] = lectura;
        }
        set_rainbow_color();
      }
    }

    set_color_RGB(0, 0, 0);

    for (int sensor = 0; sensor < NUMERO_SENSORES; sensor++) {
      Serial.print("valoresCalibracionMaximos[");
      Serial.print(sensor);
      Serial.print("] = ");
      Serial.print(valoresCalibracionMaximos[sensor]);
      Serial.print(";\n");
    }

    for (int sensor = 0; sensor < NUMERO_SENSORES; sensor++) {
      Serial.print("valoresCalibracionMinimos[");
      Serial.print(sensor);
      Serial.print("] = ");
      Serial.print(valoresCalibracionMinimos[sensor]);
      Serial.print(";\n");
    }
  }

  set_color_RGB(0, 0, 0);
}

/**
 * Función de calibrado de sensores para línea.
 * Realiza una calibración simple de valores máximos y mínimos durante el tiempo de calibración establecido y calcula el umbral de detección en base a ello.
 */
void calibrado_sensores_linea_digital() {
  calibrado_sensores_linea();
  for (int sensor = 0; sensor < NUMERO_SENSORES; sensor++) {
    umbralesCalibracionSensores[sensor] = valoresCalibracionMinimos[sensor] + (valoresCalibracionMaximos[sensor] - valoresCalibracionMinimos[sensor]) / 2.0f;
    umbralesCalibracionSensores[sensor] = map(umbralesCalibracionSensores[sensor], valoresCalibracionMinimos[sensor], valoresCalibracionMaximos[sensor], valorCalibradoMinimo, valorCalibradoMaximo);

    Serial.print("umbralesCalibracionSensores[");
    Serial.print(sensor);
    Serial.print("] = ");
    Serial.print(umbralesCalibracionSensores[sensor]);
    Serial.print(";\n");
  }
}

/**
 * Función complementaria de calibrado de sensores para línea.
 * Realiza una comprobación de los sensores buscando la línea principal para descartar lecturas aisladas.
 */
void buscar_linea_principal() {
  while (!btn_pulsado()) {
    set_color_RGB(0, 0, 255);
  }
  int numLineas = 0;
  bool detectandoAnterior = false;
  do {
    set_color_RGB(255, 0, 0);
    numLineas = 0;
    detectandoAnterior = false;
    lectura_sensores_calibrados();
    for (int sensor = 0; sensor < NUMERO_SENSORES; sensor++) {
      if (!detectandoAnterior && valoresSensores[sensor] > valorSaturacionBajo) {
        detectandoAnterior = true;
        lineaPrincipal[0] = sensor;
        numLineas++;
      } else if (detectandoAnterior && valoresSensores[sensor] <= valorSaturacionBajo) {
        lineaPrincipal[1] = sensor - 1;
        detectandoAnterior = false;
      }
      if (sensor == NUMERO_SENSORES - 1 && detectandoAnterior && valoresSensores[sensor] > valorSaturacionBajo) {
        lineaPrincipal[1] = sensor;
      }
    }

    delay(100);
  } while (numLineas != 1 || abs(lineaPrincipal[0] - lineaPrincipal[1]) > 3);
  delay(250);
  Serial.print("Linea principal: ");
  Serial.print(lineaPrincipal[0]);
  Serial.print("\t");
  Serial.print(lineaPrincipal[1]);
  Serial.println();
  set_color_RGB(0, 255, 0);
  delay(250);
  set_color_RGB(0, 0, 0);
}

/**
 * Función de calibrado de sensores para degradado.
 * Realiza una doble calibración para obtener los valores máximos y mínimos de cada sensor y el máximo y mínimo de la posición.
 */
void calibrado_sensores_degradado() {
  if (CALIBRAR_SENSORES) {
    calibrado_sensores_linea();
    do {
      set_color_RGB(0, 128, 0);
    } while (!btn_pulsado());
    set_color_RGB(0, 0, 128);

    int calibrado = 0;
    do {
      if (calibrado == 0 || (ticksDerecho >= (ticksMm * 9.3f) || ticksIzquierdo >= (ticksMm * 9.3f))) {
        lectura_sensores_calibrados();
        for (int sensor = 0; sensor < NUMERO_SENSORES; sensor++) {
          calibradoDegradado[calibrado][sensor] = valoresSensores[sensor];
        }
        ticksDerecho = 0;
        ticksIzquierdo = 0;
        calibrado++;
      }
    } while (calibrado < NUMERO_CALIBRACIONES_DEGRADADO);
    for (int calibrado = 0; calibrado < NUMERO_CALIBRACIONES_DEGRADADO; calibrado++) {

      for (int sensor = 0; sensor < NUMERO_SENSORES; sensor++) {
        Serial.print("calibradoDegradado[");
        Serial.print(calibrado);
        Serial.print("][");
        Serial.print(sensor);
        Serial.print("] = ");
        Serial.print(calibradoDegradado[calibrado][sensor]);
        Serial.print(";\n");
      }
    }

    Serial.print("posicionDegradadoMaxima = ");
    Serial.print(posicionDegradadoMaxima);
    Serial.println(";");

    set_color_RGB(0, 0, 0);
  } else {

    valoresCalibracionMaximos[0] = 3910;
    valoresCalibracionMaximos[1] = 3589;
    valoresCalibracionMaximos[2] = 3492;
    valoresCalibracionMaximos[3] = 3400;
    valoresCalibracionMaximos[4] = 3393;
    valoresCalibracionMaximos[5] = 2832;
    valoresCalibracionMaximos[6] = 3678;
    valoresCalibracionMaximos[7] = 2850;
    valoresCalibracionMaximos[8] = 2916;
    valoresCalibracionMaximos[9] = 3284;
    valoresCalibracionMaximos[10] = 3410;
    valoresCalibracionMaximos[11] = 3728;

    valoresCalibracionMinimos[0] = 210;
    valoresCalibracionMinimos[1] = 225;
    valoresCalibracionMinimos[2] = 213;
    valoresCalibracionMinimos[3] = 213;
    valoresCalibracionMinimos[4] = 214;
    valoresCalibracionMinimos[5] = 198;
    valoresCalibracionMinimos[6] = 198;
    valoresCalibracionMinimos[7] = 194;
    valoresCalibracionMinimos[8] = 220;
    valoresCalibracionMinimos[9] = 253;
    valoresCalibracionMinimos[10] = 263;
    valoresCalibracionMinimos[11] = 1466;

    calibradoDegradado[0][0] = 2305;
    calibradoDegradado[0][1] = 2582;
    calibradoDegradado[0][2] = 2555;
    calibradoDegradado[0][3] = 2682;
    calibradoDegradado[0][4] = 2857;
    calibradoDegradado[0][5] = 2826;
    calibradoDegradado[0][6] = 2240;
    calibradoDegradado[0][7] = 2995;
    calibradoDegradado[0][8] = 3838;
    calibradoDegradado[0][9] = 3753;
    calibradoDegradado[0][10] = 3532;
    calibradoDegradado[0][11] = 3768;
    calibradoDegradado[1][0] = 2171;
    calibradoDegradado[1][1] = 2676;
    calibradoDegradado[1][2] = 2471;
    calibradoDegradado[1][3] = 2743;
    calibradoDegradado[1][4] = 2930;
    calibradoDegradado[1][5] = 3050;
    calibradoDegradado[1][6] = 2271;
    calibradoDegradado[1][7] = 2820;
    calibradoDegradado[1][8] = 3728;
    calibradoDegradado[1][9] = 3699;
    calibradoDegradado[1][10] = 3625;
    calibradoDegradado[1][11] = 3660;
    calibradoDegradado[2][0] = 2149;
    calibradoDegradado[2][1] = 2694;
    calibradoDegradado[2][2] = 2322;
    calibradoDegradado[2][3] = 2532;
    calibradoDegradado[2][4] = 2802;
    calibradoDegradado[2][5] = 3173;
    calibradoDegradado[2][6] = 2370;
    calibradoDegradado[2][7] = 2346;
    calibradoDegradado[2][8] = 3752;
    calibradoDegradado[2][9] = 3672;
    calibradoDegradado[2][10] = 3583;
    calibradoDegradado[2][11] = 3763;
    calibradoDegradado[3][0] = 2042;
    calibradoDegradado[3][1] = 2760;
    calibradoDegradado[3][2] = 2484;
    calibradoDegradado[3][3] = 2649;
    calibradoDegradado[3][4] = 2743;
    calibradoDegradado[3][5] = 2557;
    calibradoDegradado[3][6] = 1863;
    calibradoDegradado[3][7] = 2397;
    calibradoDegradado[3][8] = 3402;
    calibradoDegradado[3][9] = 3617;
    calibradoDegradado[3][10] = 3401;
    calibradoDegradado[3][11] = 3658;
    calibradoDegradado[4][0] = 1855;
    calibradoDegradado[4][1] = 2700;
    calibradoDegradado[4][2] = 2295;
    calibradoDegradado[4][3] = 2679;
    calibradoDegradado[4][4] = 2780;
    calibradoDegradado[4][5] = 2324;
    calibradoDegradado[4][6] = 1934;
    calibradoDegradado[4][7] = 1730;
    calibradoDegradado[4][8] = 3194;
    calibradoDegradado[4][9] = 3378;
    calibradoDegradado[4][10] = 3265;
    calibradoDegradado[4][11] = 3610;
    calibradoDegradado[5][0] = 2499;
    calibradoDegradado[5][1] = 3023;
    calibradoDegradado[5][2] = 2926;
    calibradoDegradado[5][3] = 2943;
    calibradoDegradado[5][4] = 3043;
    calibradoDegradado[5][5] = 2677;
    calibradoDegradado[5][6] = 1743;
    calibradoDegradado[5][7] = 1466;
    calibradoDegradado[5][8] = 2667;
    calibradoDegradado[5][9] = 3286;
    calibradoDegradado[5][10] = 3129;
    calibradoDegradado[5][11] = 3478;
    calibradoDegradado[6][0] = 2425;
    calibradoDegradado[6][1] = 2844;
    calibradoDegradado[6][2] = 2471;
    calibradoDegradado[6][3] = 2181;
    calibradoDegradado[6][4] = 2617;
    calibradoDegradado[6][5] = 1687;
    calibradoDegradado[6][6] = 1291;
    calibradoDegradado[6][7] = 731;
    calibradoDegradado[6][8] = 2667;
    calibradoDegradado[6][9] = 3113;
    calibradoDegradado[6][10] = 2897;
    calibradoDegradado[6][11] = 3276;
    calibradoDegradado[7][0] = 1998;
    calibradoDegradado[7][1] = 2208;
    calibradoDegradado[7][2] = 1516;
    calibradoDegradado[7][3] = 1429;
    calibradoDegradado[7][4] = 2155;
    calibradoDegradado[7][5] = 1804;
    calibradoDegradado[7][6] = 929;
    calibradoDegradado[7][7] = 147;
    calibradoDegradado[7][8] = 1976;
    calibradoDegradado[7][9] = 2737;
    calibradoDegradado[7][10] = 2674;
    calibradoDegradado[7][11] = 3149;
    calibradoDegradado[8][0] = 1793;
    calibradoDegradado[8][1] = 2193;
    calibradoDegradado[8][2] = 2265;
    calibradoDegradado[8][3] = 1673;
    calibradoDegradado[8][4] = 1592;
    calibradoDegradado[8][5] = 686;
    calibradoDegradado[8][6] = 268;
    calibradoDegradado[8][7] = 147;
    calibradoDegradado[8][8] = 1799;
    calibradoDegradado[8][9] = 2671;
    calibradoDegradado[8][10] = 2468;
    calibradoDegradado[8][11] = 2785;
    calibradoDegradado[9][0] = 2903;
    calibradoDegradado[9][1] = 2942;
    calibradoDegradado[9][2] = 2642;
    calibradoDegradado[9][3] = 2171;
    calibradoDegradado[9][4] = 2144;
    calibradoDegradado[9][5] = 1413;
    calibradoDegradado[9][6] = 557;
    calibradoDegradado[9][7] = 111;
    calibradoDegradado[9][8] = 2308;
    calibradoDegradado[9][9] = 3151;
    calibradoDegradado[9][10] = 3092;
    calibradoDegradado[9][11] = 2938;
    calibradoDegradado[10][0] = 3102;
    calibradoDegradado[10][1] = 3692;
    calibradoDegradado[10][2] = 2378;
    calibradoDegradado[10][3] = 262;
    calibradoDegradado[10][4] = 133;
    calibradoDegradado[10][5] = 104;
    calibradoDegradado[10][6] = 74;
    calibradoDegradado[10][7] = 75;
    calibradoDegradado[10][8] = 1948;
    calibradoDegradado[10][9] = 4000;
    calibradoDegradado[10][10] = 4000;
    calibradoDegradado[10][11] = 3633;
    calibradoDegradado[11][0] = 3428;
    calibradoDegradado[11][1] = 1518;
    calibradoDegradado[11][2] = 68;
    calibradoDegradado[11][3] = 51;
    calibradoDegradado[11][4] = 84;
    calibradoDegradado[11][5] = 83;
    calibradoDegradado[11][6] = 52;
    calibradoDegradado[11][7] = 57;
    calibradoDegradado[11][8] = 118;
    calibradoDegradado[11][9] = 2140;
    calibradoDegradado[11][10] = 3473;
    calibradoDegradado[11][11] = 3701;
    calibradoDegradado[12][0] = 282;
    calibradoDegradado[12][1] = 97;
    calibradoDegradado[12][2] = 34;
    calibradoDegradado[12][3] = 38;
    calibradoDegradado[12][4] = 57;
    calibradoDegradado[12][5] = 53;
    calibradoDegradado[12][6] = 36;
    calibradoDegradado[12][7] = 45;
    calibradoDegradado[12][8] = 71;
    calibradoDegradado[12][9] = 844;
    calibradoDegradado[12][10] = 1473;
    calibradoDegradado[12][11] = 2413;
    calibradoDegradado[13][0] = 2494;
    calibradoDegradado[13][1] = 2456;
    calibradoDegradado[13][2] = 186;
    calibradoDegradado[13][3] = 61;
    calibradoDegradado[13][4] = 105;
    calibradoDegradado[13][5] = 80;
    calibradoDegradado[13][6] = 41;
    calibradoDegradado[13][7] = 39;
    calibradoDegradado[13][8] = 56;
    calibradoDegradado[13][9] = 340;
    calibradoDegradado[13][10] = 424;
    calibradoDegradado[13][11] = 1618;
    calibradoDegradado[14][0] = 2659;
    calibradoDegradado[14][1] = 3086;
    calibradoDegradado[14][2] = 1883;
    calibradoDegradado[14][3] = 66;
    calibradoDegradado[14][4] = 60;
    calibradoDegradado[14][5] = 59;
    calibradoDegradado[14][6] = 33;
    calibradoDegradado[14][7] = 31;
    calibradoDegradado[14][8] = 51;
    calibradoDegradado[14][9] = 506;
    calibradoDegradado[14][10] = 245;
    calibradoDegradado[14][11] = 1175;
    calibradoDegradado[15][0] = 2532;
    calibradoDegradado[15][1] = 1424;
    calibradoDegradado[15][2] = 73;
    calibradoDegradado[15][3] = 42;
    calibradoDegradado[15][4] = 33;
    calibradoDegradado[15][5] = 34;
    calibradoDegradado[15][6] = 18;
    calibradoDegradado[15][7] = 25;
    calibradoDegradado[15][8] = 43;
    calibradoDegradado[15][9] = 405;
    calibradoDegradado[15][10] = 312;
    calibradoDegradado[15][11] = 1188;
    calibradoDegradado[16][0] = 50;
    calibradoDegradado[16][1] = 34;
    calibradoDegradado[16][2] = 13;
    calibradoDegradado[16][3] = 18;
    calibradoDegradado[16][4] = 26;
    calibradoDegradado[16][5] = 30;
    calibradoDegradado[16][6] = 19;
    calibradoDegradado[16][7] = 21;
    calibradoDegradado[16][8] = 26;
    calibradoDegradado[16][9] = 34;
    calibradoDegradado[16][10] = 25;
    calibradoDegradado[16][11] = 707;
    calibradoDegradado[17][0] = 16;
    calibradoDegradado[17][1] = 22;
    calibradoDegradado[17][2] = 10;
    calibradoDegradado[17][3] = 16;
    calibradoDegradado[17][4] = 26;
    calibradoDegradado[17][5] = 31;
    calibradoDegradado[17][6] = 24;
    calibradoDegradado[17][7] = 21;
    calibradoDegradado[17][8] = 22;
    calibradoDegradado[17][9] = 23;
    calibradoDegradado[17][10] = 11;
    calibradoDegradado[17][11] = 424;
    calibradoDegradado[18][0] = 0;
    calibradoDegradado[18][1] = 8;
    calibradoDegradado[18][2] = 10;
    calibradoDegradado[18][3] = 18;
    calibradoDegradado[18][4] = 31;
    calibradoDegradado[18][5] = 22;
    calibradoDegradado[18][6] = 12;
    calibradoDegradado[18][7] = 16;
    calibradoDegradado[18][8] = 13;
    calibradoDegradado[18][9] = 26;
    calibradoDegradado[18][10] = 17;
    calibradoDegradado[18][11] = 516;
    calibradoDegradado[19][0] = 16;
    calibradoDegradado[19][1] = 27;
    calibradoDegradado[19][2] = 30;
    calibradoDegradado[19][3] = 42;
    calibradoDegradado[19][4] = 49;
    calibradoDegradado[19][5] = 41;
    calibradoDegradado[19][6] = 21;
    calibradoDegradado[19][7] = 15;
    calibradoDegradado[19][8] = 28;
    calibradoDegradado[19][9] = 42;
    calibradoDegradado[19][10] = 21;
    calibradoDegradado[19][11] = 389;
    calibradoDegradado[20][0] = 52;
    calibradoDegradado[20][1] = 68;
    calibradoDegradado[20][2] = 37;
    calibradoDegradado[20][3] = 27;
    calibradoDegradado[20][4] = 37;
    calibradoDegradado[20][5] = 47;
    calibradoDegradado[20][6] = 13;
    calibradoDegradado[20][7] = 13;
    calibradoDegradado[20][8] = 20;
    calibradoDegradado[20][9] = 18;
    calibradoDegradado[20][10] = 0;
    calibradoDegradado[20][11] = 42;

    posicionDegradadoMaxima = 20000;
    // posicionDegradadoMaxima = 17700;
  }
  posicionIdeal = calcula_posicion_degradado(0);
  filtroPosicion.SetCurrent(map(posicionIdeal, -200, 200, -6500, 6500) / 100 * 100);
  posicionIdeal = posicionIdeal / 100 * 100;
  posicionIdealObjetivo = posicionIdeal;
}
