# PIDfromBTlib

Librería para simplificar y mejorar la gestión de las constantes de un PID para Arduino desde una APP Android, especialmente pensada para robots siguelíneas o similares.

Con esta librería, combinada con una APP disponible proximamente en https://github.com/robotaleh/PIDfromBT, se puede calibrar rapidamente un sistema PID e incluso establecer el punto de consigna deseado sin tener que cargar una y otra vez un nuevo programa, usando comunicación Bluetooth entre tu Smartphone y el Arduino, o incluso cualquier placa de desarrollo similar.

## Funcionamiento
Actualmente existen tres formas diferentes de usar la librería, en función de las variables que se deseen modificar:

#### PIDV; Proporcional, Integral, Derivada, Velocidad
```php
PIDfromBT pid_calibrate(&kp, &kd, &ki, &vel, DEBUG);
```

#### PIDVI; Proporcional, Integral, Derivada, Velocidad, Ideal
```php
PIDfromBT pid_calibrate(&kp, &kd, &ki, &vel, &ideal, DEBUG);
```

#### PIDVIS; Proporcional, Integral, Derivada, Velocidad, Ideal, Succión
```php
PIDfromBT pid_calibrate(&kp, &kd, &ki, &vel, &ideal, &suc, DEBUG);
```
