//Define pins
const int motor1Input1 = 12;
const int motor1Input2 = 11;
const int motor2Input1 = 10;
const int motor2Input2 = 9;

const int enable2 = 5;
const int enable1 = 6;
const int light = 13;

const int speedRegulator = 1;
const int lightSensor = 0;

const int ultrasonic = 8;

long duration = 0;
long cm = 0;
  
int motorInputs[4*4] = {
  						  1,0,0,1,
  						  0,1,1,0,
  						  1,0,1,0,
  						  0,1,0,1
						};

//Forward
void forward(int speed)
{
  int moPin = 0;
  analogWrite(enable1,speed);
  analogWrite(enable2,speed);

  for (int i=9; i<13; ++i)
  {
    digitalWrite(i,motorInputs[0*4 + moPin]);
    moPin++;
  }
}

//Backward
void backward(int speed)
{
  int moPin = 0;
  analogWrite(enable1,speed);
  analogWrite(enable2,speed);

  for (int i=9; i<13; ++i)
  {
    digitalWrite(i,motorInputs[1*4 + moPin]);
    moPin++;
  }
}

//Turn left
void left(int speed)
{
  int moPin = 0;
  analogWrite(enable1,speed);
  analogWrite(enable2,speed);

  for (int i=9; i<13; ++i)
  {
    digitalWrite(i,motorInputs[2*4 + moPin]);
    moPin++;
  }
}

//Turn right
void right(int speed)
{
  int moPin = 0;
  analogWrite(enable1,speed);
  analogWrite(enable2,speed);

  for (int i=9; i<13; ++i)
  {
    digitalWrite(i,motorInputs[3*4 + moPin]);
    moPin++;
  }
}

//Stop
void stop()
{
  int moPin = 0;
  for (int i=9; i<13; ++i)
  {
    digitalWrite(i,0);
    moPin++;
  }
}

void setup()
{
  Serial.begin(9600);
  pinMode(light, OUTPUT);
  pinMode(enable1, OUTPUT);
  pinMode(enable2, OUTPUT);
  for (int i=9; i<14; ++i)
  {
    pinMode(i, OUTPUT);
  }
  for (int i=9; i<13; ++i)
  {
    digitalWrite(i, 0);
  }
}
bool cStop = false;
void loop()
{  
  //Light on when dark, otherwise off
  if (analogRead(lightSensor) < 10)
  {
    digitalWrite(light, 1);
  }
  else
  {
    digitalWrite(light, 0);
  }
  
  //Measure distance
  pinMode(ultrasonic, OUTPUT);
  digitalWrite(ultrasonic, 0); //ensure a clean HIGH pulse
  delayMicroseconds(2);
  digitalWrite(ultrasonic, 1); //send out a pulse (ping signal)
  delayMicroseconds(5);
  digitalWrite(ultrasonic, 0); //ensure a clean HIGH pulse
  
  pinMode(ultrasonic, INPUT);
  duration = pulseIn(ultrasonic, 1); //save the return time in duration
  
  cm = (duration/2)/29; //convert time to distance 340m/s = 29ms/cm
  
  int motorSpeed = map(analogRead(speedRegulator), 0, 1023, 0, 255); //speed of motor
  
  
  if(Serial.available())
  {
    char data = Serial.read();
    
    if (data == 'Y')
    {
      cStop = true;
    }
  }
  
  if (cStop)
  {
    stop();
  }
  else
  {
    if(cm > 30)
    {
      forward(motorSpeed);
    }
    else
    {
      stop();
      Serial.print('Z');

      delay(100);
    }    
  }
}






