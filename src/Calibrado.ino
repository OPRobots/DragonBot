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
    calibrado_sensores_linea();

    while (!btn_pulsado()) {
      set_color_RGB(0, 0, 255);
    }
    // Esperar hasta encontrar una sola línea centrada
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
        Serial.print(valoresSensores[sensor]);
    // Serial.print(" (");
    // Serial.print(valoresSensores[sensor]);
    // Serial.print(")");
    Serial.print("\t");
      }
      Serial.print("\n");

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

    do {
      set_color_RGB(0, 128, 0);
    } while (!btn_pulsado());
    set_color_RGB(0, 0, 128);

    long tiempoInicial = millis();
    int ultimaPosicion;
    do {
      ultimaPosicion = calcula_posicion_degradado(ultimaPosicion);
      if (abs(ultimaPosicion) > posicionDegradadoMaxima) {
        posicionDegradadoMaxima = abs(ultimaPosicion);
      }
    } while (millis() - tiempoInicial < TIEMPO_CALIBRADO);

    for (int sensor = 0; sensor < NUMERO_SENSORES; sensor++) {
    }

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
    valoresCalibracionMaximos[0] = 2410;
    valoresCalibracionMaximos[1] = 2432;
    valoresCalibracionMaximos[2] = 2364;
    valoresCalibracionMaximos[3] = 2347;
    valoresCalibracionMaximos[4] = 2535;
    valoresCalibracionMaximos[5] = 2432;
    valoresCalibracionMaximos[6] = 2248;
    valoresCalibracionMaximos[7] = 2290;
    valoresCalibracionMaximos[8] = 2799;
    valoresCalibracionMaximos[9] = 3284;
    valoresCalibracionMaximos[10] = 3360;
    valoresCalibracionMaximos[11] = 3992;

    valoresCalibracionMinimos[0] = 199;
    valoresCalibracionMinimos[1] = 200;
    valoresCalibracionMinimos[2] = 199;
    valoresCalibracionMinimos[3] = 196;
    valoresCalibracionMinimos[4] = 198;
    valoresCalibracionMinimos[5] = 204;
    valoresCalibracionMinimos[6] = 197;
    valoresCalibracionMinimos[7] = 200;
    valoresCalibracionMinimos[8] = 201;
    valoresCalibracionMinimos[9] = 209;
    valoresCalibracionMinimos[10] = 196;
    valoresCalibracionMinimos[11] = 207;

    calibradoDegradado[0][0] = 1800;
    calibradoDegradado[0][1] = 1515;
    calibradoDegradado[0][2] = 1660;
    calibradoDegradado[0][3] = 1749;
    calibradoDegradado[0][4] = 1664;
    calibradoDegradado[0][5] = 1822;
    calibradoDegradado[0][6] = 1737;
    calibradoDegradado[0][7] = 1763;
    calibradoDegradado[0][8] = 1314;
    calibradoDegradado[0][9] = 1133;
    calibradoDegradado[0][10] = 972;
    calibradoDegradado[0][11] = 1127;
    calibradoDegradado[1][0] = 1481;
    calibradoDegradado[1][1] = 1471;
    calibradoDegradado[1][2] = 1863;
    calibradoDegradado[1][3] = 1786;
    calibradoDegradado[1][4] = 1824;
    calibradoDegradado[1][5] = 1729;
    calibradoDegradado[1][6] = 1647;
    calibradoDegradado[1][7] = 1855;
    calibradoDegradado[1][8] = 1426;
    calibradoDegradado[1][9] = 1674;
    calibradoDegradado[1][10] = 1600;
    calibradoDegradado[1][11] = 1741;
    calibradoDegradado[2][0] = 1340;
    calibradoDegradado[2][1] = 1870;
    calibradoDegradado[2][2] = 1391;
    calibradoDegradado[2][3] = 1511;
    calibradoDegradado[2][4] = 1542;
    calibradoDegradado[2][5] = 1537;
    calibradoDegradado[2][6] = 1524;
    calibradoDegradado[2][7] = 1814;
    calibradoDegradado[2][8] = 1742;
    calibradoDegradado[2][9] = 1861;
    calibradoDegradado[2][10] = 1847;
    calibradoDegradado[2][11] = 1784;
    calibradoDegradado[3][0] = 1361;
    calibradoDegradado[3][1] = 1551;
    calibradoDegradado[3][2] = 1454;
    calibradoDegradado[3][3] = 1573;
    calibradoDegradado[3][4] = 1270;
    calibradoDegradado[3][5] = 1593;
    calibradoDegradado[3][6] = 1414;
    calibradoDegradado[3][7] = 1844;
    calibradoDegradado[3][8] = 1941;
    calibradoDegradado[3][9] = 1908;
    calibradoDegradado[3][10] = 1435;
    calibradoDegradado[3][11] = 1337;
    calibradoDegradado[4][0] = 1137;
    calibradoDegradado[4][1] = 1389;
    calibradoDegradado[4][2] = 1424;
    calibradoDegradado[4][3] = 1249;
    calibradoDegradado[4][4] = 1341;
    calibradoDegradado[4][5] = 1583;
    calibradoDegradado[4][6] = 1633;
    calibradoDegradado[4][7] = 1646;
    calibradoDegradado[4][8] = 1335;
    calibradoDegradado[4][9] = 1164;
    calibradoDegradado[4][10] = 886;
    calibradoDegradado[4][11] = 968;
    calibradoDegradado[5][0] = 977;
    calibradoDegradado[5][1] = 1196;
    calibradoDegradado[5][2] = 1234;
    calibradoDegradado[5][3] = 1223;
    calibradoDegradado[5][4] = 997;
    calibradoDegradado[5][5] = 1272;
    calibradoDegradado[5][6] = 1180;
    calibradoDegradado[5][7] = 1181;
    calibradoDegradado[5][8] = 1154;
    calibradoDegradado[5][9] = 821;
    calibradoDegradado[5][10] = 598;
    calibradoDegradado[5][11] = 704;
    calibradoDegradado[6][0] = 915;
    calibradoDegradado[6][1] = 914;
    calibradoDegradado[6][2] = 924;
    calibradoDegradado[6][3] = 1047;
    calibradoDegradado[6][4] = 932;
    calibradoDegradado[6][5] = 1095;
    calibradoDegradado[6][6] = 548;
    calibradoDegradado[6][7] = 729;
    calibradoDegradado[6][8] = 815;
    calibradoDegradado[6][9] = 727;
    calibradoDegradado[6][10] = 333;
    calibradoDegradado[6][11] = 634;
    calibradoDegradado[7][0] = 594;
    calibradoDegradado[7][1] = 821;
    calibradoDegradado[7][2] = 472;
    calibradoDegradado[7][3] = 414;
    calibradoDegradado[7][4] = 305;
    calibradoDegradado[7][5] = 508;
    calibradoDegradado[7][6] = 176;
    calibradoDegradado[7][7] = 439;
    calibradoDegradado[7][8] = 381;
    calibradoDegradado[7][9] = 350;
    calibradoDegradado[7][10] = 138;
    calibradoDegradado[7][11] = 511;
    calibradoDegradado[8][0] = 340;
    calibradoDegradado[8][1] = 534;
    calibradoDegradado[8][2] = 83;
    calibradoDegradado[8][3] = 99;
    calibradoDegradado[8][4] = 83;
    calibradoDegradado[8][5] = 125;
    calibradoDegradado[8][6] = 92;
    calibradoDegradado[8][7] = 105;
    calibradoDegradado[8][8] = 99;
    calibradoDegradado[8][9] = 83;
    calibradoDegradado[8][10] = 56;
    calibradoDegradado[8][11] = 275;
    calibradoDegradado[9][0] = 227;
    calibradoDegradado[9][1] = 129;
    calibradoDegradado[9][2] = 61;
    calibradoDegradado[9][3] = 56;
    calibradoDegradado[9][4] = 61;
    calibradoDegradado[9][5] = 69;
    calibradoDegradado[9][6] = 68;
    calibradoDegradado[9][7] = 83;
    calibradoDegradado[9][8] = 64;
    calibradoDegradado[9][9] = 67;
    calibradoDegradado[9][10] = 36;
    calibradoDegradado[9][11] = 90;
    calibradoDegradado[10][0] = 60;
    calibradoDegradado[10][1] = 51;
    calibradoDegradado[10][2] = 45;
    calibradoDegradado[10][3] = 50;
    calibradoDegradado[10][4] = 48;
    calibradoDegradado[10][5] = 61;
    calibradoDegradado[10][6] = 49;
    calibradoDegradado[10][7] = 57;
    calibradoDegradado[10][8] = 43;
    calibradoDegradado[10][9] = 41;
    calibradoDegradado[10][10] = 32;
    calibradoDegradado[10][11] = 39;
    calibradoDegradado[11][0] = 38;
    calibradoDegradado[11][1] = 37;
    calibradoDegradado[11][2] = 39;
    calibradoDegradado[11][3] = 47;
    calibradoDegradado[11][4] = 39;
    calibradoDegradado[11][5] = 40;
    calibradoDegradado[11][6] = 44;
    calibradoDegradado[11][7] = 46;
    calibradoDegradado[11][8] = 35;
    calibradoDegradado[11][9] = 33;
    calibradoDegradado[11][10] = 22;
    calibradoDegradado[11][11] = 24;
    calibradoDegradado[12][0] = 28;
    calibradoDegradado[12][1] = 36;
    calibradoDegradado[12][2] = 28;
    calibradoDegradado[12][3] = 33;
    calibradoDegradado[12][4] = 35;
    calibradoDegradado[12][5] = 31;
    calibradoDegradado[12][6] = 30;
    calibradoDegradado[12][7] = 43;
    calibradoDegradado[12][8] = 33;
    calibradoDegradado[12][9] = 30;
    calibradoDegradado[12][10] = 24;
    calibradoDegradado[12][11] = 24;
    calibradoDegradado[13][0] = 25;
    calibradoDegradado[13][1] = 25;
    calibradoDegradado[13][2] = 30;
    calibradoDegradado[13][3] = 29;
    calibradoDegradado[13][4] = 27;
    calibradoDegradado[13][5] = 35;
    calibradoDegradado[13][6] = 25;
    calibradoDegradado[13][7] = 29;
    calibradoDegradado[13][8] = 28;
    calibradoDegradado[13][9] = 24;
    calibradoDegradado[13][10] = 15;
    calibradoDegradado[13][11] = 21;
    calibradoDegradado[14][0] = 29;
    calibradoDegradado[14][1] = 19;
    calibradoDegradado[14][2] = 24;
    calibradoDegradado[14][3] = 28;
    calibradoDegradado[14][4] = 23;
    calibradoDegradado[14][5] = 21;
    calibradoDegradado[14][6] = 30;
    calibradoDegradado[14][7] = 27;
    calibradoDegradado[14][8] = 18;
    calibradoDegradado[14][9] = 24;
    calibradoDegradado[14][10] = 15;
    calibradoDegradado[14][11] = 15;
    calibradoDegradado[15][0] = 20;
    calibradoDegradado[15][1] = 18;
    calibradoDegradado[15][2] = 15;
    calibradoDegradado[15][3] = 22;
    calibradoDegradado[15][4] = 20;
    calibradoDegradado[15][5] = 17;
    calibradoDegradado[15][6] = 21;
    calibradoDegradado[15][7] = 23;
    calibradoDegradado[15][8] = 15;
    calibradoDegradado[15][9] = 16;
    calibradoDegradado[15][10] = 16;
    calibradoDegradado[15][11] = 13;
    calibradoDegradado[16][0] = 18;
    calibradoDegradado[16][1] = 16;
    calibradoDegradado[16][2] = 18;
    calibradoDegradado[16][3] = 14;
    calibradoDegradado[16][4] = 16;
    calibradoDegradado[16][5] = 17;
    calibradoDegradado[16][6] = 13;
    calibradoDegradado[16][7] = 14;
    calibradoDegradado[16][8] = 16;
    calibradoDegradado[16][9] = 10;
    calibradoDegradado[16][10] = 10;
    calibradoDegradado[16][11] = 14;
    calibradoDegradado[17][0] = 11;
    calibradoDegradado[17][1] = 11;
    calibradoDegradado[17][2] = 18;
    calibradoDegradado[17][3] = 12;
    calibradoDegradado[17][4] = 9;
    calibradoDegradado[17][5] = 17;
    calibradoDegradado[17][6] = 10;
    calibradoDegradado[17][7] = 12;
    calibradoDegradado[17][8] = 16;
    calibradoDegradado[17][9] = 9;
    calibradoDegradado[17][10] = 0;
    calibradoDegradado[17][11] = 10;
    calibradoDegradado[18][0] = 13;
    calibradoDegradado[18][1] = 9;
    calibradoDegradado[18][2] = 12;
    calibradoDegradado[18][3] = 16;
    calibradoDegradado[18][4] = 7;
    calibradoDegradado[18][5] = 7;
    calibradoDegradado[18][6] = 14;
    calibradoDegradado[18][7] = 8;
    calibradoDegradado[18][8] = 7;
    calibradoDegradado[18][9] = 11;
    calibradoDegradado[18][10] = 0;
    calibradoDegradado[18][11] = 0;
    calibradoDegradado[19][0] = 8;
    calibradoDegradado[19][1] = 8;
    calibradoDegradado[19][2] = 9;
    calibradoDegradado[19][3] = 7;
    calibradoDegradado[19][4] = 12;
    calibradoDegradado[19][5] = 8;
    calibradoDegradado[19][6] = 0;
    calibradoDegradado[19][7] = 12;
    calibradoDegradado[19][8] = 8;
    calibradoDegradado[19][9] = 0;
    calibradoDegradado[19][10] = 6;
    calibradoDegradado[19][11] = 0;
    calibradoDegradado[20][0] = 0;
    calibradoDegradado[20][1] = 0;
    calibradoDegradado[20][2] = 9;
    calibradoDegradado[20][3] = 0;
    calibradoDegradado[20][4] = 0;
    calibradoDegradado[20][5] = 8;
    calibradoDegradado[20][6] = 0;
    calibradoDegradado[20][7] = 0;
    calibradoDegradado[20][8] = 8;
    calibradoDegradado[20][9] = 0;
    calibradoDegradado[20][10] = 0;
    calibradoDegradado[20][11] = 0;

    posicionDegradadoMaxima = 17700;
  }
  posicionIdeal = calcula_posicion_degradado(0);
}
