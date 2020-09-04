///
/// \file  TestTEA5767.ino
/// \brief An Arduino sketch to operate a TEA5767 chip based radio using the Radio library.
///
/// \author Matthias Hertel, http://www.mathertel.de
/// \copyright Copyright (c) 2014 by Matthias Hertel.\n
/// This work is licensed under a BSD style license.\n
/// See http://www.mathertel.de/License.aspx
///
/// \details
/// This sketch implements a "as simple as possible" radio without any possibility to modify the settings after initializing the chip.\n
/// The radio chip is initialized and setup to a fixed band and frequency. These settings can be changed by modifying the 
/// FIX_BAND and FIX_STATION definitions. 
///
/// Open the Serial console with 57600 baud to see the current radio information.
///
/// Wiring
/// ------ 
/// Arduino port | Si4703 signal
/// ------------ | ---------------
///     3.3V | VCC
///      GND | GND
///       A5 | SCLK
///       A4 | SDIO
///       D2 | RST
///
/// More documentation and source code is available at http://www.mathertel.de/Arduino
///
/// History:
/// --------
/// * 15.09.2014 created.
/// * 3.09.2020 some changes by doctea to use this for a eurorack module

#include <Arduino.h>
#include <Wire.h>
#include <radio.h>
#include <TEA5767.h>

#include <ResponsiveAnalogRead.h>


ResponsiveAnalogRead analogOne(A0, true);
ResponsiveAnalogRead analogTwo(A1, true);

/// The band that will be tuned by this sketch is FM.
#define FIX_BAND RADIO_BAND_FM

/// The station that will be tuned by this sketch is 89.30 MHz.
#define FIX_STATION 9925

#define MAX_FREQ 26820.0

TEA5767 radio;    // Create an instance of Class 

uint8_t test1;
byte test2;

/// Setup a FM only radio configuration
/// with some debugging on the Serial port
void setup() {
  // open the Serial port
  Serial.begin(57600);
  Serial.println("Radio...");
  delay(200);

  // Initialize the Radio 
  radio.init();

  // Enable information to the Serial port
  //radio.debugEnable();

  // HERE: adjust the frequency to a local sender
  radio.setBandFrequency(FIX_BAND, FIX_STATION); // hr3 nearby Frankfurt in Germany
  radio.setVolume(15);
  radio.setMono(true);
  radio.setMute(false);

  analogOne.setSnapMultiplier(0.9f);  // 0.1 easing default
  analogTwo.setSnapMultiplier(0.9f);  // 0.1 easing default
} // setup


/// show the current chip data every 3 seconds.
void loop() {
  static int i = 1000;; //9200;
  static int last_freq = i;
  /*char s[12];
  radio.formatFrequency(s, sizeof(s));
  Serial.print("Station:"); 
  Serial.println(s);
  
  Serial.print("Radio:"); 
  radio.debugRadioInfo();
  
  Serial.print("Audio:"); 
  radio.debugAudioInfo();*/
  //radio.setMute(false);

  //radio.setBandFrequency(FIX_BAND, i+=random(10)); //random(10000,10500));
  //radio.setBandFrequency(FIX_BAND, 9200 + (-0.5+sin(micros()/100)*100)); //(0.5+sin(micros())/2) * 9200);

  //int step = 500.0f*(sin(micros())*10.0);
  //int step = abs(100.0f*(sin(radians(micros()/100))*10.0f));
  //Serial.print(step);

  i = readFrequency();

  //i = 24600;

  //changing to 26820*32766 top !
  // 00:03:25.551 -> changing to -10*14479 //wraps.. except it doesnt wtf
  // 00:03:25.126 -> changing to 0*27 bottom !
  
  //if (abs(i - last_freq)>=6) {
  if (i!=last_freq) {
    last_freq = i;
    //Serial.print("changing to ");//
    //Serial.print (i);
    //radio.setMute(true);
    radio.setFrequency(i);
    //i-=10;
    //delay(300);
  }

  //if (i>35000) i = 9200;
  //if (i>10000) i = 9200;
  //i++;
  //radio.setVolume(random(16));
  
  //delay(1);
  //delay(step/100);

  //if ((micros()>100)) {
    //radio.setMute(true);
    //radio.setMute(false);
  //}
} // loop

// End.


int readFrequency() {

  analogOne.update();
  analogTwo.update();
  
    // read the input on analog pin 0:
  int sensorValue1 = analogOne.getValue(); //Read(A0);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage1 = 1.0 - (sensorValue1 * (1.0 / 1023.0));
  // print out the value you read:
  //Serial.print(voltage);
  //Serial.print("    ");

  int sensorValue2 = analogTwo.getValue(); //Read(A1);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage2 = 1.0 - (sensorValue2 * (1.0 / 1023.0));
  // print out the value you read:
  //Serial.println(voltage);

  /* whoa this gives nuts modulation bleeps !! */
  /*int freq = 4000 + 
        (((sensorValue1/1023.0) * 8000.0)/2) +
        (((sensorValue2/1023.0) * 2000.0)/2);*/

  /*int freq = 8000 + 
        (((sensorValue1/1023.0) * 1000.0)/2)-500 +
        (((sensorValue2/1023.0) * 100.0)/2)-100;*/

#define sv1 voltage1 //(sensorValue1/1023.0)
#define sv2 voltage2 //(sensorValue2/1023.0)
       
  //int freq = constrain(((MAX_FREQ) * sv1) + ((MAX_FREQ) * sv2),0,MAX_FREQ);
  float v = ((sv1*10.0) + sv2)/11.0;

  int freq = v * MAX_FREQ;

  //Serial.println(freq);
  
  return freq;
}
