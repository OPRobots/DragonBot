
int deadband_z = 0;
/**
 * Función para iniciar y configurar el giroscopio y acelerómetro del MPU9250
 */
void inicia_MPU9250() {
  I2CwriteByte(MPU9250_ADDRESS, 26, ACC_FULL_SCALE_16_G);
  I2CwriteByte(MPU9250_ADDRESS, 27, GYRO_FULL_SCALE_2000_DPS);
  gyro_calibrate_deadband();
}

void lectura_raw() {
  uint8_t bufferDatos[14];
  I2Cread(MPU9250_ADDRESS, 0x3B, 14, bufferDatos);

  float ax = (-(bufferDatos[0] << 8 | bufferDatos[1])) / 2048.0f;
  float ay = (-(bufferDatos[2] << 8 | bufferDatos[3])) / 2048.0f;
  float az = (bufferDatos[4] << 8 | bufferDatos[5]) / 2048.0f;
}

/**
* Calcula el ruido normal del giroscopio
* para excluirlo de las lecturas reales.
*/
void gyro_calibrate_deadband(){
  float noise = 0;
  for(int i = 0; i < 50;i++){
    noise += read_gyro_z();
    delay(10);
  }
  deadband_z = noise/50.0f;
}


/**
* Lectura directa del eje Z del giroscopio
* @return rad/s actuales
*/
float read_gyro_z(){
  uint8_t Buf[14];
  I2Cread(MPU9250_ADDRESS, 0x3B, 14, Buf);
  int z = ((Buf[10] << 8 | Buf[11]) - deadband_z)/16.4;
  return (abs(z) > 1)? z: 0;
}

/**
 * Función auxiliar de lectura para el MPU9250
 * @param direccion
 * @param registro
 * @param numeroBytes
 * @param bufferDatos
 */
void I2Cread(uint8_t direccion, uint8_t registro, uint8_t numeroBytes, uint8_t *bufferDatos) {
  Wire.beginTransmission(direccion);
  Wire.write(registro);
  Wire.endTransmission();

  Wire.requestFrom(direccion, numeroBytes);
  uint8_t index = 0;
  while (Wire.available())
    bufferDatos[index++] = Wire.read();
}

/**
 * Función auxiliar de escritura para el MPU9250
 * @param direccion
 * @param registro
 * @param datos
 */
void I2CwriteByte(uint8_t direccion, uint8_t registro, uint8_t datos) {
  Wire.beginTransmission(direccion);
  Wire.write(registro);
  Wire.write(datos);
  Wire.endTransmission();
}
