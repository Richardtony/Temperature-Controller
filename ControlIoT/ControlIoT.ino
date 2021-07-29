#include <LiquidCrystal_I2C.h>
#include <max6675.h>
#include <Wire.h>

//Define the Pins of the Thermocouple and the PWM pin on the arduino UNO
#define PWM_Pin 11 
int thermoDO = 4;
int thermoCS = 5;
int thermoCLK = 6;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO); //Create an instance of the MAX6675 class

//Declaration of the PID values .. Make sure to change the kp,ki and kd values as per your trials
float Temp_ErrorOld=0;
float Temp_Error=0;
float kp = 15;
float kd = 0.12;
float ki = 0.01;
float sensed_val;
float P;
float I;
float D;
float dE;
float corrected_val;
float Area = 0;

int milliOld;
int milliNew;
int dt;
int set_Temp; 

//Import the liquid crystal I2C library
LiquidCrystal_I2C lcd(0x27, 16, 2);

// make a cute degree symbol
uint8_t degree[8]  = {140,146,146,140,128,128,128,128};

void setup() {
  Serial.begin(9600);  // Set the serial monitor at an baud rate of 9600bps
  pinMode(PWM_Pin, OUTPUT);  //Define the mode of the pwm pin 
  milliNew = millis(); 
  TCCR2B = TCCR2B & B11111000 | B00000011 ; //Sets timer 2 divisor to 32 for PWM frequency of 980.39 Hz 
  

  lcd.init();
  lcd.createChar(0, degree);
  lcd.backlight();

  // wait for MAX chip to stabilize
  delay(500);  
}

void loop() {
  set_Temp = Serial.read(); //Read the temp coming from ESP8266
  sensed_val = thermocouple.readCelsius(); //get the temp reading from the thermocouple
  //Calculate the error and PID values
  milliOld = milliNew;
  milliNew = millis();
  dt = (milliNew - milliOld)/1000;
  Temp_Error = set_Temp - sensed_val;
  dE = Temp_Error - Temp_ErrorOld;
  Area+= (Temp_Error*dt);
  P = kp*Temp_Error;
  D = kd*(dE/dt);
  I = ki*Area;
  corrected_val = P+D+I;
  if (corrected_val < 0){
    corrected_val = 0;
  }
  if (corrected_val> 255){
    corrected_val = 255;
  }
  //Write the value to the PWM pin
  analogWrite(PWM_Pin, 255-corrected_val);
  Temp_ErrorOld = Temp_Error; //Setting the old error with the current error and repeating the process
  Serial.println("PWM:" + String(255-corrected_val) + " P:" + String(P)+ " D:" + String(D)+ " I:" + String(I) + " Controller:" + String(corrected_val)+ set_Temp);  //Display the data readings on the Serial Monitor
  
  // Display the Set point and the Feedback value onto the LCD display
  lcd.clear();
  // go to line #1
  lcd.setCursor(0,1);
  lcd.print("SP:"+String(float(set_Temp))); //Displays Set point on the LCD display
#if ARDUINO >= 100
  lcd.write((byte)0);
#else
  lcd.print(0, BYTE);
#endif
  lcd.print("C ");
  
  lcd.setCursor(0,0);
  lcd.print("RV:"+String(sensed_val)); //Displays feddback value on the LCD display
#if ARDUINO >= 100
  lcd.write((byte)0);
#else
  lcd.print(0, BYTE);
#endif
  lcd.print('C');
  
  delay(1000);
}
