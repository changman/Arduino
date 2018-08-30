// motor control pin
#include <Encoder.h>
#include <MotorController.h>
#include <PID_v1.h>

const unsigned int motorDirPin =4; // L298 Input 1
const unsigned  int motorPWMPin = 5; // L298 Input 2
const unsigned int motorEnable = 9;

// encoder pin
const int encoderPinA = 2;
const int encoderPinB = 3;

const float ratio = 0.09375;

// P control
//float Kp = 0.005;
//float targetDeg = 360;
float targetDeg = 3840;

//float Kp = 1;

double duration,abs_duration;//the number of the pulses
boolean Direction;//the rotation direction 
boolean result;

double val_output;//Power supplied to the motor PWM value.
double Setpoint;
double Kp=0.075, Ki=0.01, Kd=0.05f;  

Encoder motor_encoder(encoderPinA,encoderPinB);
MotorController motor_controller(motorDirPin,motorPWMPin,motorEnable);
PID myPID(&abs_duration, &val_output, &Setpoint, Kp, Ki, Kd, DIRECT); 

void setup() {

  Serial.begin(9600);

   Setpoint =3840;  //Set the output value of the PID
   myPID.SetMode(AUTOMATIC);//PID is set to automatic mode
   myPID.SetSampleTime(100);//Set PID sampling frequency is 100ms
   
}

void loop() {

  long new_position;
  
  new_position = motor_encoder.read();
  abs_duration=new_position;
  float pwm = val_output;
  int dir = 0;
  if(Setpoint-new_position>=0){
    dir = 1;
    pwm= val_output/255;
  }else {
       dir = -1;
       pwm= val_output*dir;
       pwm= pwm/255;

   //    pwm = -1;
  }
  //pwm = -1;
  //Serial.print("pwm : ");
  //Serial.print(pwm);
  //Serial.print(" new_position : ");
  Serial.println(new_position);
  //Serial.print(" val_output : ");
  //Serial.println(val_output);

 
  motor_controller.set(pwm);


  boolean result=myPID.Compute();//PID conversion is complete and returns 1
  
 // Serial.print("pwm : ");
//  Serial.print(pwm);
// Serial.println("new_position : ");
//   Serial.println(new_position);
 // Serial.print("   motorDeg : ");
 // Serial.print(float(new_position)*ratio);
 // Serial.print("   error : ");
 // Serial.print(error);
 // Serial.print("    control : ");
 // Serial.print(control);
 // Serial.print("    motorVel : ");
 // Serial.println(min(abs(control), 255)); 
}

