// motor control pin
const int motorDirPin = 12; // L298 Input 1
const int motorPWMPin = 13; // L298 Input 2
const int motorEnable = 9; // L298 enable 

// encoder pin
const int encoderPinA = 2;
const int encoderPinB = 3;

 int encoderPos = 0;
//const float ratio = 360./103./52.;

//const float ratio = 360./103./300.;

//const float ratio = 0.0116f;

//const float ratio = 360./16./120.0;

//const float ratio = 0.1875;
const float ratio = 0.093;

// P control
//float Kp = 30;
float Kp = 0.6;
float targetDeg = 360;

bool ispressed = false;

long input=2;
long des_Angle =0;

void doEncoderA(){  encoderPos += (digitalRead(encoderPinA)==digitalRead(encoderPinB))?1:-1;}
void doEncoderB(){  encoderPos += (digitalRead(encoderPinA)==digitalRead(encoderPinB))?-1:1;}


void doMotor(bool dir, int vel){

  int pwmOutput = map(vel, 0, 255, 70 , 255);
  analogWrite(motorEnable, pwmOutput);
//  analogWrite(motorEnable, dir?(255 - vel):vel);
  if(dir)
  {
    digitalWrite(motorDirPin, HIGH);
    digitalWrite(motorPWMPin, LOW);
  }else
  {
    digitalWrite(motorDirPin, LOW);
    digitalWrite(motorPWMPin, HIGH);

  }  
}

void demoOne()
{
   // this function will run the motors in both directions at a fixed speed
   // turn on motor A
doMotor(true,100);

   delay(2000);
doMotor(false,100);

   delay(2000);
   // now turn off motors
   digitalWrite(motorDirPin, LOW);
   digitalWrite(motorPWMPin, LOW);
   delay(2000);

}


/*
void doMotor(bool dir, int vel){
//  analogWrite(motorEnable, dir?(255 - vel):vel);
  digitalWrite(motorDirPin, dir);
  analogWrite(motorPWMPin, dir?(255 - vel):vel);
}
*/
void setup() {
  pinMode(encoderPinA, INPUT_PULLUP);
  attachInterrupt(0, doEncoderA, CHANGE);
  
  pinMode(encoderPinB, INPUT_PULLUP);
  attachInterrupt(1, doEncoderB, CHANGE);
  
  pinMode(motorDirPin, OUTPUT);
  pinMode(motorPWMPin,OUTPUT);
  pinMode(motorEnable, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  float motorDeg = float(encoderPos)*ratio;
  
  float error = targetDeg - motorDeg;
  float control = Kp*error;
//demoOne();
  if (Serial.available()){
       des_Angle= Serial.parseInt();
       if(des_Angle !=0)
          input = des_Angle;
  }

  if(input == 2)
  {
     digitalWrite(motorDirPin,LOW);
      digitalWrite(motorPWMPin,LOW);
      encoderPos = 0;
      
  }else
    doMotor( (control>=0)?HIGH:LOW, min(abs(control), 255));

 if(-2< error && error <2)
{
       digitalWrite(motorDirPin,LOW);
      digitalWrite(motorPWMPin,LOW);
}

 if(3870< encoderPos && encoderPos <3880)
{
       digitalWrite(motorDirPin,LOW);
      digitalWrite(motorPWMPin,LOW);
}
      
  /*
  else if( input > 2)
   doMotor( true, 240);
   else {
     doMotor( false, 240);
   }

   if(encoderPos > 3540)
   {
     digitalWrite(4,LOW);
      digitalWrite(5,LOW);
    
   }
   */

/*

  if(encoderPos > input)
  {
  //  doMotor(false,255);
   //doMotor( (control>=0)?HIGH:LOW, min(abs(control), 255));
   doMotor( true, 1);
  
  }else if(encoderPos == input)
  {
      digitalWrite(4,LOW);
      digitalWrite(5,LOW);
  }else
  {
     doMotor(false,   255);
  }
*/
  

  Serial.print("encoderPos : ");
  Serial.print(encoderPos);
  Serial.print("   motorDeg : ");
  Serial.print(float(encoderPos)*ratio);
  Serial.print("   error : ");
  Serial.print(error);
  Serial.print("    control : ");
  Serial.print(control);
  Serial.print("    motorVel : ");
  Serial.println(min(abs(control), 255)); 
}
