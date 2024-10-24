/*
* Name: clock and temp project
* Author: Matiin Axner
* Date: 2024-10-10
* Description: This project uses a ds3231 to measure time and displays the time to an 1306 oled display,
* Further, it measures temprature with a analog temprature module and displays a mapped value to a 9g-servo-motor. 
*It is also a extra fan that is cooling the temprature module when it gets to hot
*/

// Include Libraries
#include <RTClib.h>
#include <Wire.h>
#include "U8glib.h"
#include <Servo.h>

// Init constants
const int tempPin = A1;

// Init global variables
int pos = 0;
// construct objects
RTC_DS3231 rtc;
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);
Servo myservo;


void setup() {


  // oled screen fonts
  u8g.setFont(u8g_font_helvB14);


  // init communication
  Serial.begin(9600);
  Wire.begin();

  // Init Hardware
  rtc.begin();
  pinMode(tempPin, INPUT);
  myservo.attach(9);
  pinMode(7, OUTPUT);

  //Settings
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void loop() {

  servoWrite(getTemp());
  oledWrite("Time: " + String(getTime()), "Temp: " + String(getTemp()) + String(char(176)) + "C");
  //Serial.println("Tiden är " + getTime());
  delay(1000);

  fan();
}


/*
*This function reads time from an ds3231 module and package the time as a String
*Parameters: Void
*Returns: time in hh:mm:ss as String
*/
String getTime() {

  DateTime now = rtc.now();
  return String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
}

/*
* This function reads an analog pin connected to an analog temprature sensor and calculates the corresponding temp
* It calculates from Kelvin and than to Celcius.
*Parameters: Void
*Returns: temprature as float
*/
float getTemp() {
  float temp = 0;
  float R1 = 10000;  // value of R1 on board
  float logR2, R2, T;
  float c1 = 0.001129148, c2 = 0.000234125, c3 = 0.0000000876741;  //steinhart-hart coeficients for thermistor

  int Vo = analogRead(tempPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);  //calculate resistance on thermistor
  logR2 = log(R2);
  temp = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));  // temperature in Kelvin
  temp = temp - 273.15;                                           //convert Kelvin to Celcius

  return temp;
}

/*
* This function takes a string and draws it to an oled display
*Parameters: - text: String to write to display
*Returns: void
*/
void oledWrite(String text1, String text2) {
  u8g.firstPage();
  do {
    u8g.drawStr(0, 20, text1.c_str());
    u8g.drawStr(0, 50, text2.c_str());
  } while (u8g.nextPage());
}


/*
* takes a temprature value and maps it to corresponding degree on a servo
*Parameters: - value: temprature
*Returns: void
*/

void servoWrite(float value) {
  float pos = map(value, 22, 28, 0, 180);
  myservo.write(pos);
}

/*
 * This function checks the temperature using the `getTemp()` function. 
 * If the temperature exceeds 28 degrees Celsius, it turns the fan on by 
 * setting the digital output on pin 7 to HIGH. If the temperature is 
 * 28 degrees or lower, it turns the fan off by setting the output to LOW.
 *Parameters: - none
 *Returns: void
*/
void fan() {
  if (getTemp() > 28) {
    digitalWrite(7, HIGH);
  } else {

    digitalWrite(7, LOW);
  }
}