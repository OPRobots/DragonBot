#ifndef PIDfromBT_h
#define PIDfromBT_h
#define LIBRARY_VERSION 1.0
class PIDfromBT
{

#define DEBUG true
#define NO_DEBUG false

#define MIN_IDEAL -500  // Valor por defecto del Mínimo Ideal
#define MAX_IDEAL 500   // Valor por defecto del Máimo Ideal

#define TYPE_PIDV   1 // Tipo Proporcional, Integral, Derivada, Velocidad
#define TYPE_PIDVI  2 // Tipo Proporcional, Integral, Derivada, Velocidad, Ideal
#define TYPE_PIDVIS 3 // Tipo Proporcional, Integral, Derivada, Velocidad, Ideal, Succion

public:
  /**
   * Constructor del objeto PIDfromBT de tipo PIDV; Proporcional, Integral, Derivada, Velocidad
   * @param kp    Referencia a la constante de Proporcionalidad
   * @param ki    Referencia a la constante de Integración
   * @param kd    Referencia a la constante de Derivación
   * @param vel   Reverencia a la variable de Velocidad
   * @param debug Parámetro que indica si se imprimirán los valores establecidos
   */
  PIDfromBT(float* kp, float* kd, float* ki, int* vel, bool debug);

  /**
   * Constructor del objeto PIDfromBT de tipo PIDVI; Proporcional, Integral, Derivada, Velocidad, Ideal
   * @param kp    Referencia a la constante de Proporcionalidad
   * @param ki    Referencia a la constante de Integración
   * @param kd    Referencia a la constante de Derivación
   * @param vel   Reverencia a la variable de Velocidad
   * @param ideal Referencia a la variable de Ideal o Consigna de Posición
   * @param debug Parámetro que indica si se imprimirán los valores establecidos
   */
  PIDfromBT(float* kp, float* kd, float* ki, int* vel, int* ideal, bool debug);


  /**
   * Constructor del objeto PIDfromBT de tipo PIDVIS; Proporcional, Integral, Derivada, Velocidad, Ideal, Succión
   * @param kp      Referencia a la constante de Proporcionalidad
   * @param ki      Referencia a la constante de Integración
   * @param kd      Referencia a la constante de Derivación
   * @param vel     Reverencia a la variable de Velocidad
   * @param ideal   Referencia a la variable de Ideal o Consigna de Posición
   * @param suction Referencia a la variable de Succion o Velocidad del Ventilador/Turbina
   * @param debug   Parámetro que indica si se imprimirán los valores establecidos
   */
  PIDfromBT(float* kp, float* kd, float* ki, int* vel, int* ideal, int* suction, bool debug);

  /**
   * Función para comprobar si existen comandos pendientes en el Buffer de entrada; debe ejecutarse
   * esta función al menos una vez por cada ciclo de ejecución
   */
  void update();

  /**
   * Función para establecer el ideal mínimo en cualquier momento
   * @param minIdeal Valor entero para el mínimo ideal
   */
  void setMinIdeal(int minIdeal);

  /**
   * Función para establecer el idea máximo en cualquier momento
   * @param maxIdeal Valor entero para el máximo ideal
   */
  void setMaxIdeal(int maxIdeal);

private:
  /**
   * Función para modificar el valor de la variable indicada por la letra
   * @param letter Letra que identifica la variable que hay que modificar
   * @param value  Valor que tomará la variable indicada por la Letra anterior
   */
  void execute_task(char letter, float value);

  String _value;
  char _letter;
  long _last_update;
  bool _debug;
  int _type;
  float *_kp;
  float *_ki;
  float *_kd;
  int *_vel;
  int *_ideal;
  int *_suction;
  int _maxIdeal;
  int _minIdeal;
};
#endif
