/*
Map:
  #############################################################
  #                                   #                       #
  #                                   #                       #
  #     A           B           C     #     D           E     #
  #                                   #                       #
  #                                   #                       #
  #############           #########################           #
  #           #                                               #
  #           #                                               #
  #     F     #     G           H           I           J     #
  #           #                                               #
  #           #                                               #
  #                                   #            ############
                                      #                       #
                                      #                       #
	      K           L           M     #     N           O     #
                                    	#                       #
                                    	#                       #
	#############                       #########################
	#                       #                                   #
  #                       #                                   #
	#     P           Q     #     R           S           T     #
  #                       #                                   #
	#                       #                                   #
  #############################################################
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <stdbool.h>
#include<LiquidCrystal.h>
#include <Servo.h>

LiquidCrystal lcd(8,9,10,11,12,13);

Servo myServo; //create a servo object
int servoAngle = 0;
const int buzzer = 2; //buuzer to pin 10

const int irPin = 7;

const int button = 6;
int position = 0;

const float gammaV = 0.75;
const float alpha = 0.9;

//Define the rewards
uint8_t  R_new[20*20] = {
                0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,
                0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,

                0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,
                0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,
                0,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,
                0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,
                0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,

                0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,
                0,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,
                0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,0,
                0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,
                0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,

                0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,
                0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,
                0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,
                0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,
                0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,
              };

int numRoute = 1;
char *b_route;
char data;

//State to location
char state_to_location(int stl)
{
  return (char)(stl + 65);
}

//Location the states
int location_to_state(char los)
{
  return (int)(los - 65);
}

//Get the index which has the maximum value
int argmax(int* arr, int low, int high, int arrLen)
{
    int max = arr[low];
    for(int i=low; i<high; i++)
    {
        if(arr[i] > max)
        {
            max = arr[i];
        }
    }
    int index = -1;
    for(int j=0; j<arrLen; j++)
    {
        if(arr[low + j] == max)
        {
            index = j;
            break;
        }
    }
    return index;
}

//Return the best route
void route(char starting_location, char ending_location, int* numRoute, char* b_route)
{
  int ending_state = location_to_state(ending_location);
  R_new[ending_state*20 + ending_state] = (uint8_t)255;
  //Defining Q value
  int Q[400];
  for (int q = 0; q < 400; q++)
  {
    Q[q] = 0;

  }
  //Training
  srand(time(0));
  for(int i=0; i<580; i++)
  {
    int current_state = rand() % 20;
    int *playable_actions;
    int numAction = 0;
    bool initArray = false;
    for (int p=0; p<20; p++)
    {
      if (R_new[current_state*20 + p] > 0)
      {
        numAction++;
        if(!initArray)
        {
          playable_actions = (int *)malloc(numAction * sizeof(int));
          initArray = true;
        }
        else
        {
          playable_actions = (int *)realloc(playable_actions, numAction * sizeof(int));
        }
        playable_actions[numAction-1] = p;
      }
    }
    int next_state = playable_actions[rand() % numAction];
    float TD = ((float)R_new[current_state*20 + next_state] + (gammaV * ((float)Q[next_state*20 + argmax(Q, next_state*20, next_state*20+20, 20)]) / 100.0)) - (float)Q[current_state*20 + next_state]/ 100.0;
    Q[current_state*20 + next_state] += (int16_t)(alpha * TD * (float)100.0);
    free(playable_actions);
  }
  int nRoute = 1;
  b_route[nRoute-1] = starting_location;
  char next_location = starting_location;
  while (next_location != ending_location)
  {
    int starting_state = location_to_state(next_location);
    int next_state = argmax(Q, starting_state*20, starting_state*20+20, 20);
    next_location = state_to_location(next_state);
    b_route = (char *)realloc(b_route, nRoute * sizeof(char));
    b_route[nRoute] = next_location;
    starting_location = next_location;
    nRoute++;
  }
  *numRoute = nRoute;
}

void setup()
{
  Serial.begin(9600);
  lcd.begin(16,2);
  pinMode(buzzer, OUTPUT);
  myServo.attach(3);
  pinMode(irPin, INPUT);
  pinMode(button, INPUT);
  myServo.write(0); 
  
  numRoute = 1;
  b_route = (char *)malloc(numRoute * sizeof(char));
  route('A', 'F',  &numRoute, b_route);
}
int once = 0;
void loop()
{
  char cs = b_route[position];
  char ns = b_route[position + 1];
  
  
  if (digitalRead(button) == 1 && b_route[position + 1] != b_route[numRoute])
  {
    cs = ns;
    ns = b_route[position + 2];
    position++;
  }
    lcd.clear();

    lcd.setCursor(0,0);
    lcd.print("Current State: ");
    lcd.print(b_route[position]);

    lcd.setCursor(0,1);
    lcd.print("Next State   : ");
    lcd.print(b_route[position + 1]);
    
  	if (cs == b_route[numRoute - 1])
    {
      Serial.print('Y');
      if (digitalRead(irPin) == 1)
      {
        for (int angle = 0; angle < 90; angle++)
        {  
          myServo.write(angle); 
          delay(10); 
        }
      }
      else 
      {
        myServo.write(0);
      }
    }

  if(Serial.available())
  {
    data = Serial.read();
    
    if (data == 'Z')
    {
        digitalWrite(buzzer, 1);
      	delay(100);
      	digitalWrite(buzzer, 0);
    }
  }
  delay(200);
}