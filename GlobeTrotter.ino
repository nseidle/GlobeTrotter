/*
 7-15-2012
 Spark Fun Electronics
 Nathan Seidle
 
 This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).
 
 GlobeTrotter lights a large white plastic globe with softly changing random RGB colors. 
 
 Color comes from RGB strips:
 http://www.sparkfun.com/products/10260
 that light up a very large 22" plastic globe
 http://www.lamppostglobes.com/servlet/the-Round-Replacement-Globes-cln-22-Inch-Globes/Categories
 all sorts of different colors. The LEDs are driven by 12V so I use the MOSFET shield:
 http://www.sparkfun.com/products/10618
 to control each RGB channel. Power comes direct from a 12V car battery. Globe is elevated on the end of a 10'
 PVC pipe.
 
 The RGB colors slowly fade from one color to the next. Works well. I plan to add external control from
 danger shield sliders:
 http://www.sparkfun.com/products/10570

 RGB MOSFETs are on PWM channels 3, 5, 6
 Analog sliders are on A0, 1, 2
 Photocell is on A3
 Buttons are D10, D11, D12

 */

//MOSFETs connected to PWM pins
int RED_LED_PIN = 3;
int GREEN_LED_PIN = 5;
int BLUE_LED_PIN = 6;

//How quickly do we want to get to the next color?
int STEPVALUE = 3;
int STEPS = 200;

int lightSensor = A3;

int redSlider = A0;
int greenSlider = A1;
int blueSlider = A2;
int redWant, blueWant, greenWant; //Read the slider values into these variables
int oldRed, oldBlue, oldGreen; //Be able to rememeber where we left the slider

boolean slidersMoved; //Have we moved the sliders since the last read?
int MIN_SLIDER_CHANGE = 7;

char tempString[100];

long newColor;
long oldColor;

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(5));

  pinMode(lightSensor, INPUT);

  pinMode(redSlider, INPUT);
  pinMode(greenSlider, INPUT);
  pinMode(blueSlider, INPUT);
}

void loop() {
  //Check sliders to see if they've changed
  redWant = analogRead(redSlider);
  greenWant = analogRead(greenSlider);
  blueWant = analogRead(blueSlider);
  
  slidersMoved = false;

  if(oldRed - redWant > MIN_SLIDER_CHANGE) 
    slidersMoved = true;
  else if (oldGreen - greenWant > MIN_SLIDER_CHANGE) 
    slidersMoved = true;
  else if (oldBlue - blueWant > MIN_SLIDER_CHANGE) 
    slidersMoved = true;
  
  oldRed = redWant;
  oldGreen = greenWant;
  oldBlue = blueWant;
  
  if(slidersMoved == true) {
    sprintf(tempString, "Sliders: %02d / %02d / %02d", redWant, greenWant, blueWant);
    Serial.println(tempString);
    
  }
  

  oldColor = newColor;
  newColor = random(0xFFFFFF);

  //Push more values to off, 25% chance
  if(random(100) < 25) newColor = newColor & 0xFFFF00; //Turn blue off
  if(random(100) < 25) newColor = newColor & 0xFF00FF; //Turn green off
  if(random(100) < 25) newColor = newColor & 0x00FFFF; //Turn red off

  setColor(oldColor, newColor); //Display this color

  Serial.print("Old:");
  Serial.print(oldColor, HEX);
  Serial.print(" New:");
  Serial.println(newColor, HEX);
}

void setColor(unsigned long oldColor, unsigned long newColor) {
  /* 
   0x specifies the value is a hexadecimal number
   RR is a byte specifying the red intensity
   GG is a byte specifying the green intensity
   BB is a byte specifying the blue intensity
   */

  // Extract the intensity values from the hex triplet
  float oldRed = (oldColor >> 16) & 0xFF;
  float oldGreen = (oldColor >> 8) & 0xFF;
  float oldBlue = oldColor & 0xFF;

  byte newRed = (newColor >> 16) & 0xFF;
  byte newGreen = (newColor >> 8) & 0xFF;
  byte newBlue = newColor & 0xFF;

  int redDiff = abs(oldRed - newRed);
  int greenDiff = abs(oldGreen - newGreen);
  int blueDiff = abs(oldBlue - newBlue);

  float redStep = (float)redDiff / (STEPS/STEPVALUE); //50 / (STEPS/5) = 0;
  float greenStep = (float)greenDiff / (STEPS/STEPVALUE);
  float blueStep = (float)blueDiff / (STEPS/STEPVALUE);

  if (redStep == 0) redStep = 1;
  if (greenStep == 0) greenStep = 1;
  if (blueStep == 0) blueStep = 1;

  //Move Intensity1 towards Intensity2
  for(int fadeSteps = 0 ; fadeSteps <= STEPS ; fadeSteps += STEPVALUE){

    //if less, then add
    //if more, then subtract
    //if equal, do nothing

    if(oldRed == newRed) ; //Do nothing
    else if(oldRed < newRed) oldRed += redStep;
    else if(oldRed > newRed)
    {
      if(oldRed > redStep) oldRed -= redStep;
      else if(oldRed > 0) oldRed--;
    }

    if(oldGreen == newGreen) ; //Do nothing
    else if(oldGreen < newGreen) oldGreen += greenStep;
    else if(oldGreen > newGreen)
    {
      if(oldGreen > greenStep) oldGreen -= greenStep;
      else if(oldGreen > 0) oldGreen--;
    }

    if(oldBlue == newBlue) ; //Do nothing
    else if(oldBlue < newBlue) oldBlue += blueStep;
    else if(oldBlue > newBlue)
    {
      if(oldBlue > blueStep) oldBlue -= blueStep;
      else if(oldBlue > 0) oldBlue--;
    }

    //Some interesting case corrections
    if(oldRed > 255) oldRed = 255;
    if(oldBlue > 255) oldBlue = 255;
    if(oldGreen > 255) oldGreen = 255;
    if(oldRed < 0) oldRed = 0;
    if(oldBlue < 0) oldBlue = 0;
    if(oldGreen < 0) oldGreen = 0;

    analogWrite(RED_LED_PIN, oldRed);
    analogWrite(GREEN_LED_PIN, oldGreen);
    analogWrite(BLUE_LED_PIN, oldBlue); 

    /*Serial.print("redDiff:");
     Serial.print(blueDiff, DEC);
     Serial.print(" redStep:");
     Serial.print(blueStep, 2);
     Serial.print(" oldRed:");
     Serial.print(oldBlue, 2);
     Serial.print(" newRed:");
     Serial.println(newBlue, DEC);*/

    delay(45); //30 miliseconds between steps
  }

  delay(500);
}



