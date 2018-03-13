///////////////////////////////////
// VARIABLES DE CONTROL DE LEDS  //
///////////////////////////////////
int colorRGB[] = {255,0,0};
int colorDesc = 0;
int colorAsc = 1;
long ultimoCambioRGB = 0;

/**
 * Función para aplicar un Rainbow al led RGB de la placa principal
 * Esta función aplica un intervalo por defecto de 1 ms
 */
void set_rainbow_color(){
  set_rainbow_color(1);
}

/**
 * Función para aplicar un Rainbow al led RGB de la placa principal
 * @param [int] intervalo Tiempo en ms que transcurre entre cambios de color
 */
void set_rainbow_color(int intervalo){
  if(millis()-ultimoCambioRGB > intervalo){
      colorRGB[colorDesc] -= 3;
      colorRGB[colorAsc] += 3;
      set_color_RGB(colorRGB[0], colorRGB[1], colorRGB[2]);
      if(colorRGB[colorDesc]<=0 || colorRGB[colorAsc]>=255){
        colorRGB[colorDesc] = 0;
        colorRGB[colorAsc] = 255;
        set_color_RGB(colorRGB[0], colorRGB[1], colorRGB[2]);
        colorDesc++;
        if(colorDesc > 2){
          colorDesc = 0;
        }
        colorAsc = colorDesc == 2 ? 0 : colorDesc+1;
      }
      ultimoCambioRGB = millis();
  }
}

/**
 * Función para establecer el valor de cada color del led RGB de la placa principal
 * @param r [int] Color Rojo 0-255
 * @param g [int] Color Verde 0-255
 * @param b [int] Color Azul 0-255
 */
void set_color_RGB(int r, int g, int b) {
  analogWrite(RED_RGB_R, r);
  analogWrite(RED_RGB_G, g);
  analogWrite(RED_RGB_B, b);
 }
