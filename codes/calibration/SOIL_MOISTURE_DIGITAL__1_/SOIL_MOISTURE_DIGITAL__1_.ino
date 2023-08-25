int sensor_pin1= A0;    // defining input pin
//int sensor_pin2= A1;    // defining input pin


void setup() {
  // put your setup code here, to run once:
pinMode(sensor_pin1,INPUT);  //assign pins
//pinMode(sensor_pin2, INPUT);  // assign pins
Serial.begin(9600);   //communincating with computer

}

void loop() {
  // put your main code here, to run repeatedly:
int a1 = analogRead(sensor_pin1);
//int a2 = analogRead(sensor_pin2);
//delay(100);
//int a2 = analogRead(sensor_pin2);//reading the input
delay(1);
Serial.print("a1 = ");       //printing the input
Serial.println(a1);
//Serial.print(", a2 =");
//Serial.println(a2);
delay(800);
}
