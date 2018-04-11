/** 
 * Comprueba si algún botón de la cruceta está pulsado y actua en consecuencia.
 * Nota: Si se pulsa IZQUIERDA-DERECHA se entra en modo menú, suspendiendo todas las demás funciones del robot.
 * @return 	 [void] 
 */
void btn_cruceta() {
  bool btn_cruceta_pulsado = false;
  int menuAnterior = menuActual;
  if (!menuSeleccionModificar) {
    switch (calcular_btn_cruceta(analogRead(BTN_CRUCETA))) {
    case CRUCETA_ARRIBA:
      menuSeleccion[menuActual]--;
      btn_cruceta_pulsado = true;
      break;
    case CRUCETA_ABAJO:
      menuSeleccion[menuActual]++;
      btn_cruceta_pulsado = true;
      break;
    case CRUCETA_DERECHA:
      if (!menuSeleccionVolver) {
        menuActual = menuSeleccion[menuActual] + 1;
      } else {
        menuSeleccionVolver = false;
        menuActual = MENU_PRINCIPAL;
      }
      btn_cruceta_pulsado = true;
      break;
    case CRUCETA_IZQUIERDA:
      menuActual = MENU_PRINCIPAL;
      btn_cruceta_pulsado = true;
      break;
    case CRUCETA_ARRIBA_ABAJO:

      break;
    case CRUCETA_DERECHA_IZQUIERDA:
      calibra_sensores();
      break;
    case CRUCETA_ARRIBA_DERECHA:

      break;
    case CRUCETA_DERECHA_ABAJO:
      if (!modoMenu) {
        btn_cruceta_pulsado = true;
        robot_modo_menu();
      }
      break;
    case CRUCETA_ABAJO_IZQUIERDA:

      break;
    case CRUCETA_IZQUIERDA_ARRIBA:

      break;
    }
  } else {
    switch (calcular_btn_cruceta(analogRead(BTN_CRUCETA))) {
    case CRUCETA_DERECHA:
      switch (menuActual) {
      case MENU_PID:
        *menuConfigPID[menuSeleccion[menuActual]] += menuConfigCambioPID[menuSeleccion[menuActual]];
        btn_cruceta_pulsado = true;
        break;
      case MENU_VELOCIDAD:
        *menuConfigVelocidad[menuSeleccion[menuActual]] += menuConfigCambioVelocidad[menuSeleccion[menuActual]];
        btn_cruceta_pulsado = true;
        break;
      case MENU_SUCCION:
        *menuConfigSuccion[menuSeleccion[menuActual]] += menuConfigCambioSuccion[menuSeleccion[menuActual]];
        btn_cruceta_pulsado = true;
        break;
      case MENU_PISTA:
        sectoresPista[menuSeleccion[menuActual] - 1][SECTOR_TICKS] += menuConfigCambioPista[menuSeleccion[menuActual]];
        btn_cruceta_pulsado = true;
        break;
      }

      break;
    case CRUCETA_IZQUIERDA:
      switch (menuActual) {
      case MENU_PID:
        *menuConfigPID[menuSeleccion[menuActual]] -= menuConfigCambioPID[menuSeleccion[menuActual]];
        btn_cruceta_pulsado = true;
        break;
      case MENU_VELOCIDAD:
        *menuConfigVelocidad[menuSeleccion[menuActual]] -= menuConfigCambioVelocidad[menuSeleccion[menuActual]];
        btn_cruceta_pulsado = true;
        break;
      case MENU_SUCCION:
        *menuConfigSuccion[menuSeleccion[menuActual]] -= menuConfigCambioSuccion[menuSeleccion[menuActual]];
        btn_cruceta_pulsado = true;
        break;
      case MENU_PISTA:
        sectoresPista[menuSeleccion[menuActual] - 1][SECTOR_TICKS] -= menuConfigCambioPista[menuSeleccion[menuActual]];
        btn_cruceta_pulsado = true;
        break;
      }
      break;
    }
  }
  if (btn_pulsado() && menuActual != MENU_PRINCIPAL) {
    if (menuActual != MENU_PISTA || (menuSeleccion[menuActual] > 0 && menuSeleccion[menuActual] < NUMERO_MENU_PISTA + NUMERO_SECTORES)) {
      menuSeleccionModificar = !menuSeleccionModificar;
      btn_cruceta_pulsado = true;
    }
  }

  if (btn_cruceta_pulsado && modoMenu) {
    if (menuAnterior != MENU_PRINCIPAL && menuActual != MENU_PRINCIPAL && menuAnterior != menuActual) {
      menuActual = menuAnterior;
      btn_cruceta_pulsado = false;
    }
    int numeroItemsMenu = 0;
    switch (menuActual) {
    case MENU_PRINCIPAL:
      numeroItemsMenu = NUMERO_MENU_PRINCIPAL;
      break;
    case MENU_PID:
      numeroItemsMenu = NUMERO_MENU_PID;
      break;
    case MENU_VELOCIDAD:
      numeroItemsMenu = NUMERO_MENU_VELOCIDAD;
      break;
    case MENU_SUCCION:
      numeroItemsMenu = NUMERO_MENU_SUCCION;
      break;
    case MENU_PISTA:
      numeroItemsMenu = NUMERO_MENU_PISTA;
      break;
    }
    if (menuSeleccion[menuActual] > numeroItemsMenu) {
      if (menuActual != MENU_PISTA) {
        menuSeleccion[menuActual] = numeroItemsMenu;
        btn_cruceta_pulsado = false;
      } else {
        if (menuSeleccion[menuActual] > (numeroItemsMenu + NUMERO_SECTORES)) {
          menuSeleccion[menuActual] = numeroItemsMenu + NUMERO_SECTORES;
          btn_cruceta_pulsado = false;
        }
      }
    } else if (menuSeleccion[menuActual] < 0) {
      menuSeleccion[menuActual] = 0;
      btn_cruceta_pulsado = false;
    }
  }
  if (btn_cruceta_pulsado && modoMenu) {
    mostrar_menu();
    delay(250);
  }
}

/** 
 * Muestra el menú por Serial, indicando la opción seleccionada y sus opciones 
 * @return 	 [void] 
 */
void mostrar_menu() {
  String tituloMenu = "";
  int numeroItemsMenu = 0;
  switch (menuActual) {
  case MENU_PRINCIPAL:
    tituloMenu = "PRINCIPAL";
    numeroItemsMenu = NUMERO_MENU_PRINCIPAL;
    break;
  case MENU_PID:
    tituloMenu = "PID";
    numeroItemsMenu = NUMERO_MENU_PID;
    break;
  case MENU_VELOCIDAD:
    tituloMenu = "VELOCIDAD";
    numeroItemsMenu = NUMERO_MENU_VELOCIDAD;
    break;
  case MENU_SUCCION:
    tituloMenu = "SUCCIÓN";
    numeroItemsMenu = NUMERO_MENU_SUCCION;
    break;
  case MENU_PISTA:
    tituloMenu = "PISTA";
    numeroItemsMenu = NUMERO_MENU_PISTA;
    break;
  }
  Serial.print("MENÚ ");
  Serial.println(tituloMenu);
  Serial.println("*********************");
  int menuItem;
  for (menuItem = 0; menuItem < numeroItemsMenu; menuItem++) {
    if (menuSeleccion[menuActual] == menuItem) {
      if (!menuSeleccionModificar) {
        Serial.print("· ");
      } else {
        Serial.print("× ");
      }
    } else {
      Serial.print("  ");
    }
    switch (menuActual) {
    case MENU_PRINCIPAL:
      Serial.print(menuPrincipal[menuItem]);
      Serial.print(": ");
      Serial.println();
      break;
    case MENU_PID:
      Serial.print(menuPID[menuItem]);
      Serial.print(": ");
      Serial.print(*menuConfigPID[menuItem], 4);
      Serial.println();
      break;
    case MENU_VELOCIDAD:
      Serial.print(menuVelocidad[menuItem]);
      Serial.print(": ");
      Serial.print(*menuConfigVelocidad[menuItem]);
      Serial.println();
      break;
    case MENU_SUCCION:
      Serial.print(menuSuccion[menuItem]);
      Serial.print(": ");
      Serial.print(*menuConfigSuccion[menuItem]);
      Serial.println();
      break;
    case MENU_PISTA:
      Serial.print(menuPista[menuItem]);
      Serial.print(": ");
      Serial.print(menuConfigPista[menuItem]);
      Serial.println();
      break;
    }
  }
  if (menuActual == MENU_PISTA) {
    for (int sector = menuItem; sector < NUMERO_SECTORES + menuItem; sector++) {
      if (menuSeleccion[menuActual] == sector) {
        if (!menuSeleccionModificar) {
          Serial.print("· ");
        } else {
          Serial.print("× ");
        }
      } else {
        Serial.print("  ");
      }
      Serial.print(sector - menuItem + 1);
      if (sectoresPista[sector - menuItem][SECTOR_TIPO] == TIPO_SECTOR_RECTA) {
        Serial.print(" R ");
      } else {
        Serial.print(" C ");
      }
      Serial.println(sectoresPista[sector - menuItem][SECTOR_TICKS]);
    }
    menuItem += NUMERO_SECTORES;
  }

  if (menuSeleccion[menuActual] == menuItem) {
    Serial.print("· ");
    menuSeleccionVolver = true;
  } else {
    Serial.print("  ");
  }
  if (menuActual == MENU_PRINCIPAL) {
    Serial.println("Salir");
  } else {
    Serial.println("Volver");
  }
  for (int extra = menuItem; extra < NUMERO_MENU_MAXIMO; extra++) {
    Serial.println();
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

/** 
 * Pausa los timers y restablece los ajustes del robot a sus valores default 
 * @return 	 [void] 
 */
void robot_modo_menu() {
  pausa_timers();
  competicionIniciada = false;
  velocidadBase = 0;
  velocidadSuccion = 0;
  kp = 0;
  ki = 0;
  kd = 0;
  velocidadActual = 0;
  modoMenu = true;
}