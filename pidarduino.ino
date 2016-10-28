// programa PID desde cero modificada por Isaias Cuenya 
//Robot equilibrista

#include <Wire.h>

//Direccion I2C de la IMU
#define MPU 0x68

//MPU-6050 da los valores en enteros de 16 bits
//Valores sin refinar
int16_t AcX;//variable del ascelerometro

int TiempoMuestreo=1;        // tiempo de muestreo Se encuentra en milisegundos
unsigned long pasado=0;      // tiempo pasado (Se hace para asegurar tiempo de muestreo)
unsigned long ahora;
float Ref=-0.1;              // referencia o setpoint
double Y;                    // Salida
double error;                // error
double errorPass=0;          // Suma del error para la parte integral
double errorAnt=0;           // error anterior para la derivada
double U;                    // Señal control

// constantes del controlador 
int Kp=1300;
int Ki=8;
int Kd=300;

int ruedaI=10;              // llanta Izquierda
int ruedaD=11;              // llanta derecha

 
void setup()
{
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);// configuracion acceso al mpu6050
  Wire.write(0);
  Wire.endTransmission(true);
  Serial.begin(9600);
}

void loop() {
  
    //Leer los valores del Acelerometro de la IMU
   Wire.beginTransmission(MPU);
   Wire.write(0x3B);                          //Pedir el registro 0x3B - corresponde al AcX
   Wire.endTransmission(false);
   Wire.requestFrom(MPU,6,true);             //A partir del 0x3B, se piden 6 registros
   AcX=Wire.read()<<8|Wire.read();           //Cada valor ocupa 2 registros
  
   Y=float(AcX)/17000;                       // medición de la planta
 
  ahora=millis();                            // tiempo actual 
   
  int CambioTiempo= ahora-pasado;                       // diferencia de tiempo actual- pasado
  if(CambioTiempo >= TiempoMuestreo)                    // si se supera el tiempo de muestreo
  {
      error = Ref - Y;                                 // error (Lazo cerrado)-- Feedback o retroalimentacion Señal proporcional
      errorPass=error*TiempoMuestreo+errorPass;        // cálculo de aproximación del area Señal integrada
      double errorD =(error-errorAnt)/TiempoMuestreo;  // cálculo derivativo  Señal derivativa
      float P=Kp*error;                                // control proporcional
      float I=Ki*errorPass;                            // control Integral
      float D=Kd*errorD;                               //controlderibativo      
      
      U=P+I+D;                       // Señal de control  la mas importante del pid 
      
      pasado=ahora;                // actualizar tiempo
      errorAnt=error;              // actualizar el error
     
  }  
  
   Serial.println(U);

  if (U < -255)     // límites de saturación de la señal de control esto es cuando supera ciertos parametros se impone un maximo
  {
      U=-255;
  }
  if (U > 255)    // límites de saturación de la señal de control esto es cuando supera ciertos parametros se impone un maximo
  {
  {
      U=255;
  }
  
   int atras=0;
   int adelante=0;
  
   if (U>0){
     adelante=U;       // PWM de los motores atras y adelante (las dos ruedas funcionan al tiempo)
   }
   if (U<0){
     atras =abs(U);
   }
  if (adelante != 0){   // no se permite que se envie el comando hacia adelante y hacia atras al tiempo 
     if (atras == 0){  
       analogWrite(ruedaI,adelante);
       digitalWrite(6,LOW);
       digitalWrite(7,HIGH);
     
       analogWrite(ruedaD,adelante);
       digitalWrite(4,LOW);
       digitalWrite(5,HIGH);
     }
  }
 
  if (atras != 0){         // no se permite que se envie el comando hacia adelante y hacia atras al tiempo 
     if (adelante == 0){
       analogWrite(llantaI,atras);
       digitalWrite(6,HIGH);
       digitalWrite(7,LOW);
     
       analogWrite(llantaD,atras);
       digitalWrite(4,HIGH);
       digitalWrite(5,LOW);
    }
  }
}
