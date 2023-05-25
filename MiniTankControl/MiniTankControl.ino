#include <SoftwareSerial.h>
#include <AFMotor.h>

SoftwareSerial BTSerial(53, 52); //9, 13

//Constants
const String FORWARD_KEY = "forward";
const String BACKWARD_KEY = "backward";
const String LEFT_KEY = "toTheLeft";
const String RIGHT_KEY = "toTheRight";
const String STOP_Y_KEY = "stopTankY";
const String STOP_X_KEY = "stopTankX";
const String MOTOR_SPEED_KEY = "motorSpeed";

const int SENSOR_FRONT_T = 51; // 11 
const int SENSOR_FRONT_E = 50; //10
const int SENSOR_FRONT_MIN = 30;

long t; //timepo que demora en llegar el eco
long d; //distancia en centimetros

int temp = 0; //temporizador para el envio de señales BLE

String currentKeyY = "";
String currentKeyX = "";

int motorspeed = 250;
 
// Sensors
int sensor1 = 10;
int sensor2 = 200;
int sensor3 = 3000;

//Parameters
AF_DCMotor motorA(1);  // IZQUIERDO 1
AF_DCMotor motorB(2);  // IZQUIERDO 2
AF_DCMotor motorC(3);  // DERECHO 1 
AF_DCMotor motorD(4);  // DERECHO 2

void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600);

  // motors
  motorA.setSpeed(motorspeed);
  motorB.setSpeed(motorspeed);
  motorA.run(RELEASE);
  motorB.run(RELEASE);

  motorC.setSpeed(motorspeed);
  motorD.setSpeed(motorspeed);
  motorC.run(RELEASE);
  motorD.run(RELEASE);

  pinMode(SENSOR_FRONT_T, OUTPUT); //pin como salida
  pinMode(SENSOR_FRONT_E, INPUT);  //pin como entrada
  digitalWrite(SENSOR_FRONT_T, LOW);
}

void loop() {
  digitalWrite(SENSOR_FRONT_T, HIGH);
  delayMicroseconds(10);       //Enviamos un pulso de 10us
  digitalWrite(SENSOR_FRONT_T, LOW);

  t = pulseIn(SENSOR_FRONT_E, HIGH);     //obtenemos el ancho del pulso
  d = t/59;                    //escalamos el tiempo a una distancia en cm

  // Serial.print("Distancia: ");
  // Serial.print(d);      //Enviamos serialmente el valor de la distancia
  // Serial.print("cm");
  // Serial.println();
  // delay(100); 

  if (BTSerial.available()) {
    DecodeMsgFromBT();
  }

  if (!currentKeyX.equals("")) {
    if (currentKeyX.equals(LEFT_KEY)) { // IZQUIERDA
      motorA.run(BACKWARD);
      motorB.run(BACKWARD);
      motorC.run(FORWARD);
      motorD.run(FORWARD);
      Serial.println("Izquierda");
    } else if (currentKeyX.equals(RIGHT_KEY)) { // DERECHA
      motorA.run(FORWARD);  
      motorB.run(FORWARD);
      motorC.run(BACKWARD);
      motorD.run(BACKWARD);
      Serial.println("Derecha");
    } else if (currentKeyX.equals(STOP_X_KEY)) {
      stopTireAll();
      currentKeyX = "";
    }
  } else if (currentKeyY.equals(BACKWARD_KEY) && d > SENSOR_FRONT_MIN) {// ADELANTE
    motorA.run(BACKWARD); 
    motorB.run(BACKWARD);
    motorC.run(BACKWARD);
    motorD.run(BACKWARD);
    Serial.println("Adelante");
  } else if (currentKeyY.equals(FORWARD_KEY)) { // ATRAS
    motorA.run(FORWARD); 
    motorB.run(FORWARD);
    motorC.run(FORWARD);
    motorD.run(FORWARD);
    Serial.println("Atras");
  } else if (currentKeyY.equals(STOP_Y_KEY)) {
    stopTireAll();
    currentKeyY = "";
  }

  motorA.setSpeed(motorspeed);
  motorB.setSpeed(motorspeed);
  motorC.setSpeed(motorspeed);
  motorD.setSpeed(motorspeed);

  // sensor1 = sensor1 + 1;
  // sensor2 = sensor2 + 2;
  // sensor3 = sensor3 + 3;
  // delay(100);

  String sendData = "sensorFront/" + String(d);//  + "--" + "sensor2/" + String(sensor2) + "--"  + "sensor3/" + String(sensor3);

  if(temp >= 100){
    temp = 0;
    // Serial.println(sendData);
    BTSerial.write(sendData.c_str());

    if(d <= SENSOR_FRONT_MIN){
      stopTireAll();
    }
  }else{
    temp++;
    // Serial.println(temp);
  }
}

void DecodeMsgFromBT() {
  String message = BTSerial.readStringUntil('\n');
  int positionSlash = message.indexOf('/');

  String key = message.substring(0, positionSlash);
  int value = message.substring(positionSlash + 1, message.length()).toInt();

  if (key.equals(BACKWARD_KEY)) {
    currentKeyY = BACKWARD_KEY;
  } else if (key.equals(LEFT_KEY)) {
    currentKeyX = LEFT_KEY;
  } else if (key.equals(FORWARD_KEY)) {
    currentKeyY = FORWARD_KEY;
  } else if (key.equals(RIGHT_KEY)) {
    currentKeyX = RIGHT_KEY;
  } else if (key.equals(STOP_Y_KEY)) {
    currentKeyY = STOP_Y_KEY;
  } else if (key.equals(STOP_X_KEY)) {
    currentKeyX = STOP_X_KEY;
  } else if (key.equals(MOTOR_SPEED_KEY)) {
    motorspeed = value;
    Serial.println("Nueva velocidad: " + String(value));
  }

  // Serial.println("Comman execute =============>>> START");
  // Serial.println(message);
  // Serial.println("Key: " + key);
  // Serial.println("Value: " + String(value));
  // Serial.println("<<<END>>>");
}

void stopTireAll() {
  motorA.run(RELEASE);
  motorB.run(RELEASE);
  motorC.run(RELEASE);
  motorD.run(RELEASE);
}
