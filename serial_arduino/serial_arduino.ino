#include <Adafruit_Sensor.h>
#include <Adafruit_AM2320.h>


Adafruit_AM2320 the_sensor = Adafruit_AM2320();


void setup() {
  Serial.begin(9600);
  delay(1000);
  the_sensor.begin();
}

void loop() {
  
  char prepared_message[30];
  
  int temp = the_sensor.readTemperature() * 100; // devide by 100 on other end, ez no float
  int humid = the_sensor.readHumidity() * 100; // also do the same

  
  sprintf(prepared_message, "%i %i", temp, humid);

  Serial.println(prepared_message);

  delay(10000);
  
}
