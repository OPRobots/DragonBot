
/**
 * Función para iniciar y configurar el giroscopio y acelerómetro del MPU9250
 */
void inicia_MPU9250() {
  I2CwriteByte(MPU9250_ADDRESS, 26, ACC_FULL_SCALE_16_G);
  I2CwriteByte(MPU9250_ADDRESS, 27, GYRO_FULL_SCALE_2000_DPS);
}

void lectura_raw() {
  uint8_t bufferDatos[14];
  I2Cread(MPU9250_ADDRESS, 0x3B, 14, bufferDatos);

  float ax = (-(bufferDatos[0] << 8 | bufferDatos[1])) / 5030.0f;
  float ay = (-(bufferDatos[2] << 8 | bufferDatos[3])) / 5030.0f;
  float az = (bufferDatos[4] << 8 | bufferDatos[5]) / 5030.0f;

  // int16_t gx = -(bufferDatos[8] << 8 | bufferDatos[9]);
  // int16_t gy = -(bufferDatos[10] << 8 | bufferDatos[11]);
  // int16_t gz = bufferDatos[12] << 8 | bufferDatos[13];

  Serial.print(ax);
  Serial.print("\t");
  Serial.print(ay);
  Serial.print("\t");
  Serial.print(az);
  // Serial.print("\t");
  //
  // Serial.print(gx, DEC);
  // Serial.print("\t");
  // Serial.print(gy, DEC);
  // Serial.print("\t");
  // Serial.print(gz, DEC);
  Serial.print("\n");

  delay(2500);
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
