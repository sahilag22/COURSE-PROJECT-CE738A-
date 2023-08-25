/*The circuit:
   analog sensors  rtc on A4 and A5 adn with 3.3v
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)
    dht22 sensor connnect to digital pin 2 */

// we have used 3 sensors, sd card module, dht22 temp and humidity and rtc to keep a record of time
#include <SPI.h>    
#include <math.h>
#include <SD.h>   //SD CARD LIBRARY
#include <Wire.h>  
#include "RTClib.h"
RTC_DS1307 rtc; 
#include "DHT.h"  
#define DHTPIN 2
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
const int chipSelect = 4;
DHT dht(DHTPIN, DHTTYPE);
int soil_1 = A0;   // defining input pin
int soil_2 = A1;   // defining input pin
float m1 = -0.0381;
float m2 = -0.152;
float i1 = 36.9346;   //DEFINING THE UNCERTAINTY VALUES
float i2 = 75.022;
float Um1 = 0.0026;
float Um2 = 0.022;
float Ui1 = 1.554;
float Ui2 = 5.154;
float v1, v2, Vmean, Uvmc, Uvmc1, Uvmc2, Us1, Us2;  //DEFINING VARIABLE USED
File myFile;
void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  if (! rtc.begin())
  {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  // running a check for rtc functioning.
  if (! rtc.isrunning())
  {
    Serial.println("RTC is NOT running!");
  }
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));//auto update from computer time
  //rtc.adjust(DateTime(2022, 11, 18, 16, 07, 20));// to set the time manualy
  // Open serial communications and wait for port to open:
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  dht.begin();
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
}
//above process will happpen once while starting the process
void loop() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  int a1 = analogRead(soil_1); //raw values of soil moisture sensor
  int a2 = analogRead(soil_2);
  //Serial.print(a1);
  //Serial.print(",");
  //Serial.println(a2);
  delay(1);
  // average of 50 values and sumation of square of 10 values
  int s1 = 0, s2 = 0, SQ1 = 0, SQ2 = 0;
  for (int i = 0; i < 50; i++) {
    s1 = s1 + a1;   //sum of sensor values
    s2 = s2 + a2;   // sum of sensor vaues
    SQ1 = SQ1 + a1 * a1; //sum of squares  values
    SQ2 = SQ2 + a2 * a2; // sum of squares values
  }
  s1 = s1 / 50;  //average
  s2 = s2 / 50;
  Us1 = sqrt((SQ1 - 50 * s1 * s1) / 49) / (sqrt(50));     // UNCERTAITY IN SENSOR READING
  Us2 = sqrt((SQ2 - 50 * s2 * s2) / 49) / (sqrt(50));
  //Serial.print(s1);
  //Serial.print(", ,");
  //Serial.println(s2);
  //standard error in  sesnor values 50 at a time.
  //volmetric moisture content from calibrating equation in % FOR DIFFERENT CASES
  if (s1 <= 299 and s2 <= 299) {
    float v1 = m1 * s1 + i1;
    float v2 = m1 * s2 + i1;
    Uvmc1 = sqrt((m1 * Us1) * (m1 * Us1) + (s1 * Um1) * (s1 * Um1) + (Ui1) * (Ui1));
    Uvmc2 = sqrt((m1 * Us2) * (m1 * Us2) + (s2 * Um1) * (s2 * Um1) + (Ui1) * (Ui1));
    Vmean = (v1 + v2) / 2;
    Uvmc = (2 * sqrt((Uvmc1) * (Uvmc1) + (Uvmc2) * (Uvmc2)) / 2); //expanded Uncertainty for normal distributiion coverage factor k =2
  }
  else if (s1 > 299 and s2 > 299) {
    float v1 = m2 * s1 + i2;
    float v2 = m2 * s2 + i2;
    Uvmc1 = sqrt((m2 * Us1) * (m2 * Us1) + (s1 * Um2) * (s1 * Um2) + (Ui2) * (Ui2));
    Uvmc2 = sqrt((m2 * Us2) * (m2 * Us2) + (s2 * Um2) * (s2 * Um2) + (Ui2) * (Ui2));
    Vmean = (v1 + v2) / 2;
    Uvmc = (2 * sqrt((Uvmc1) * (Uvmc1) + (Uvmc2) * (Uvmc2)) / 2); //expanded Uncertainty for normal distributiion coverage factor k =2
  }
  else if (s1 > 299 and s2 < 299) {
    float v1 = m2 * s1 + i2;
    float v2 = m1 * s2 + i1;
    Uvmc1 = sqrt((m2 * Us1) * (m2 * Us1) + (s1 * Um2) * (s1 * Um2) + (Ui2) * (Ui2));
    Uvmc2 = sqrt((m1 * Us2) * (m1 * Us2) + (s2 * Um1) * (s2 * Um1) + (Ui1) * (Ui1));
    Vmean = (v1 + v2) / 2;
    Uvmc = (2 * sqrt((Uvmc1) * (Uvmc1) + (Uvmc2) * (Uvmc2)) / 2); //expanded Uncertainty for normal distributiion coverage factor k =2
  }
  else if (s1<299 and s2>299) {
    float v1 = m1 * s1 + i1;
    float v2 = m2 * s2 + i2;
    Uvmc1 = sqrt((m1 * Us1) * (m1 * Us1) + (s1 * Um1) * (s1 * Um1) + (Ui1) * (Ui1));
    Uvmc2 = sqrt((m2 * Us2) * (m2 * Us2) + (s2 * Um2) * (s2 * Um2) + (Ui2) * (Ui2));
    Vmean = (v1 + v2) / 2;
    Uvmc = (2 * sqrt((Uvmc1) * (Uvmc1) + (Uvmc2) * (Uvmc2)) / 2); //expanded Uncertainty for normal distributiion coverage factor k =2
  }
  delay(800);
  DateTime now = rtc.now();  //taking data of time and date from the computer at the moment
  Serial.print(now.day());
  Serial.print('/');
  Serial.print(now.month());
  Serial.print('/');
  Serial.print(now.year());
  Serial.print(",");
  Serial.print(now.hour());
  Serial.print(':');
  Serial.print(now.minute());
  Serial.print(':');
  Serial.print(now.second());
  Serial.print(",");
  Serial.print(Vmean);    /PRINT ON SERIAL MONITOR WHIHC WILL BE THEN SEND TO SERVER
  Serial.print(",");
  Serial.print(Uvmc);
  Serial.print(",");
  Serial.print(h);
  Serial.print(",");
  Serial.println(t);
  
  String File_name = String(now.day()) + String(now.month()) + String(now.year()) + ".txt";
  File dataFile = SD.open( File_name, FILE_WRITE);
  if (dataFile) {  //write data in sd card 
    //Serial.print("DATE: ");
    dataFile.print("DATE: ");
    dataFile.print(now.day());

    dataFile.print('/');

    dataFile.print(now.month());

    dataFile.print('/');

    dataFile.print(now.year());

    //Serial.print(",  TIME: ");
    dataFile.print(",  TIME: ");
    dataFile.print(now.hour());

    dataFile.print(':');

    dataFile.print(now.minute());

    dataFile.print(':');

    dataFile.print(now.second());

    dataFile.print((", VMC:"));
    //Serial.print((", VMC:"));
    dataFile.print(Vmean);

    //Serial.print(("%, EXP_UNCERTAINTY`_VMC:"));

    dataFile.print(("%, HUMIDITY: "));
    //Serial.print(("%, HUMIDITY:"));
    dataFile.print(h);

    dataFile.print(("%,  TEMP: "));
    //Serial.print(("%,  TEMP: "));
    dataFile.print(t);

    dataFile.println(("°C"));
    //Serial.println(("°C "));
    dataFile.close();   // closing file after saving all data
  }
  else {
    Serial.println("error opening vmcdhtrtc.txt");
  }
  delay(2000);
}
