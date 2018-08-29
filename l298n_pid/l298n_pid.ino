double count = 0; //set the counts of the encoder
double angle = 0;//set the angles
boolean A,B;
byte state, statep;

double pwm = 9;// this is the PWM pin for the motor for how much we move it to correct for its error
const int dir1 = 5;//these pins are to control the direction of the motor (clockwise/counter-clockwise)
const int dir2 = 4;

double setpoint = 360;//I am setting it to move through 100 degrees
//double Kp = 0.32;// you can set these constants however you like depending on trial & error
//double Ki = 0.1;
//double Kd = 0.3;
//double Kp =0.715f;
double Kp  = 0.85f;
double Ki  = 0.004f;
double Kd  = 5.0f;

float last_error = 0;
float error = 0;
float changeError = 0;
float totalError = 0;
float pidTerm = 0;
float pidTerm_scaled = 0;// if the total gain we get is not in the PWM range we scale it down so that it's not bigger than |255|
float pidTerm_scaled1 = 0;


unsigned long lasttick=0;

void setup() {
  Serial.begin(9600);
  pinMode(2, INPUT);//encoder pins
  pinMode(3, INPUT);
  attachInterrupt(0,Achange,CHANGE);//interrupt pins for encoder
  attachInterrupt(1,Bchange,CHANGE); 
  
  pinMode(pwm, OUTPUT);
  pinMode(dir1, OUTPUT);
  pinMode(dir2, OUTPUT);

  lasttick = millis();
}

void loop(){
  
  PIDcalculation();// find PID value
  bool dir = false;
  if (angle < setpoint) {
    digitalWrite(dir1, LOW);// Forward motion
    digitalWrite(dir2, HIGH);
    dir = true;
  } else {
    digitalWrite(dir1, HIGH);//Reverse motion
    digitalWrite(dir2, LOW);
    dir = false;
  }

   if(setpoint==angle)
  {
    Serial.println("Stop");
    digitalWrite(dir1,LOW);
    digitalWrite(dir2,LOW);
  }

  analogWrite(pwm, pidTerm_scaled1);
//analogWrite(pwm, 67);
//  Serial.print(" WHEEL ANGLE,");
  Serial.print(dir);
  Serial.print(" ,dir,") ;

  Serial.print(angle);
  Serial.print(" ,pidTerm_scaled1,") ;
  Serial.println(pidTerm_scaled1);
//  Serial.print(" ,intval,") ;
//  Serial.print(millis()-lasttick);
//  Serial.print(" ,lasttic,") ;
//  Serial.println(lasttick);
 
  lasttick = millis();
  delay(1);
}

void PIDcalculation(){
 // angle = (0.9 * count);//count to angle conversion
  angle = (0.0928 * count);//count to angle conversion

  error = setpoint - angle;
  
  changeError = error - last_error; // derivative term
  totalError += error; //accumalate errors to find integral term
  pidTerm = (Kp * error) + (Ki * totalError) + (Kd * changeError);//total gain
  pidTerm = constrain(pidTerm, -255, 255);//constraining to appropriate value
  pidTerm_scaled = abs(pidTerm);//make sure it's a positive value
  //pidTerm_scaled1 = map(pidTerm_scaled, 0, 255, 67 , 255);
  pidTerm_scaled1 = pidTerm_scaled;
  last_error = error;
}
  
void Achange() //these functions are for finding the encoder counts
{
  A = digitalRead(2);
  B = digitalRead(3);

  if ((A==HIGH)&&(B==HIGH)) state = 1;
  if ((A==HIGH)&&(B==LOW)) state = 2;
  if ((A==LOW)&&(B==LOW)) state = 3;
  if((A==LOW)&&(B==HIGH)) state = 4;
  switch (state)
  {
    case 1:
    {
      if (statep == 2) count++;
      if (statep == 4) count--;
      break;
    }
    case 2:
    {
      if (statep == 1) count--;
      if (statep == 3) count++;
      break;
    }
    case 3:
    {
      if (statep == 2) count --;
      if (statep == 4) count ++;
      break;
    }
    default:
    {
      if (statep == 1) count++;
      if (statep == 3) count--;
    }
  }
  statep = state;

}

void Bchange()
{
  A = digitalRead(2);
  B = digitalRead(3);

  if ((A==HIGH)&&(B==HIGH)) state = 1;
  if ((A==HIGH)&&(B==LOW)) state = 2;
  if ((A==LOW)&&(B==LOW)) state = 3;
  if((A==LOW)&&(B==HIGH)) state = 4;
  switch (state)
  {
    case 1:
    {
      if (statep == 2) count++;
      if (statep == 4) count--;
      break;
    }
    case 2:
    {
      if (statep == 1) count--;
      if (statep == 3) count++;
      break;
    }
    case 3:
    {
      if (statep == 2) count --;
      if (statep == 4) count ++;
      break;
    }
    default:
    {
      if (statep == 1) count++;
      if (statep == 3) count--;
    }
  }
  statep = state;
  
}



