#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>
#define SensorPin A2            //pH meter Analog output to Arduino Analog Input 0
#define Offset 0.00            //deviation compensate
unsigned long int avgValue;     //Store the average value of the sensor feedback

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 6

SoftwareSerial mySerial(7, 8);

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

int sensorPin = A0;
float phValue;
float temperatureC;
long duration, cm;

void setup(void)
{
  // start serial port
  Serial.begin(9600);
  // Start up the library
  sensors.begin(); // IC Default 9 bit. If you have troubles consider upping it 12. Ups the delay giving the IC more time to process the temperature measurement
}

void loop() {
  sensors.requestTemperatures(); // Send the command to get temperature
  Serial.println(sensors.getTempCByIndex(0));
  int reading = analogRead(sensorPin);

  // converting that reading to voltage,
  float voltage = reading * 5.0;
  voltage /= 1024.0;

  // now print out the temperature
  float temperatureC = (voltage - 0.5) * 100 ;
  PH();
  temperature ();
  turbidity();
  send_sms();
  delay(4000);
  send_sms1();
  delay(8000);
}

//////////////////////////////////////////////////////////// FUNCTIONS ////////////////////////////////////////////////////////////////////
void PH(){
  Serial.println(" ");
  Serial.println("Taking Readings from PH Sensor");
  int buf[10];                //buffer for read analog
  for(int i=0;i<10;i++)       //Get 10 sample value from the sensor for smooth the value
  {
    buf[i]=analogRead(SensorPin);
    delay(10);
  }
  for(int i=0;i<9;i++)        //sort the analog from small to large
  {
    for(int j=i+1;j<10;j++)
    {
      if(buf[i]>buf[j])
      {
        int temp=buf[i];
        buf[i]=buf[j];
        buf[j]=temp;
      }
    }
  }
  avgValue=0;
  for(int i=2;i<8;i++)                      //take the average value of 6 center sample
    avgValue+=buf[i];
  float phValue=(float)avgValue*3.8/1030/6; //convert the analog into millivolt
  phValue=3.3*phValue+Offset;                      //convert the millivolt into pH value
  Serial.print("pH:");
  Serial.print(phValue,2);
  Serial.println(" ");

  if(phValue >= 7.30){
    Serial.print("PH VALUE: ");
    Serial.println(phValue);
    Serial.println("Water Alkalinity high");
    delay(3000);
   }

  if(phValue >= 6.90 && phValue <= 7.19){
    Serial.print("PH VALUE: ");
    Serial.println(phValue);
    Serial.println("Water Is  neutral (safe)");
  }

  if(phValue < 6.89){
    Serial.print("PH VALUE: ");
    Serial.println(phValue);
    Serial.println("Water Acidity High");
    delay(3000);
   }

  delay(8000);
}


void temperature (){
  Serial.println(" ");
  Serial.println("Taking Readings from Temperature Sensor");
  temp_check_surr();
  delay(4000);
  temp_check_water();
}

void temp_check_surr(){
  int reading = analogRead(sensorPin);

  // converting that reading to voltage,
  float voltage = reading * 5.0;
  voltage /= 1024.0;

  // now print out the temperature
  float temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
  //to degrees ((voltage - 500mV) times 100)
  Serial.print("Surrounding Temperature: ");
  Serial.println(temperatureC);

  if(temperatureC > 50){
    Serial.print("Surrounding Temperature: ");
    Serial.print(temperatureC);
    Serial.println(" degree C");
    Serial.println("Surrounding Temperature high");
    delay(3000);
   }

  if(temperatureC >= 10 && temperatureC <= 50){
    Serial.print("Surrounding Temperature: ");
    Serial.print(temperatureC);
    Serial.println(" degree C");
    Serial.println("Surrounding Temperature normal");
  }

  if(temperatureC < 10){
    Serial.print("Surrounding Temperature: ");
    Serial.print(temperatureC);
    Serial.println(" degree C");
    Serial.println("Surrounding Temperature low");
    delay(3000);
   }
  delay(8000);
}


void temp_check_water(){
  sensors.requestTemperatures(); // Send the command to get temperature
  Serial.print("Water Temperature: ");
  Serial.println(sensors.getTempCByIndex(0));

  if(sensors.getTempCByIndex(0) > 40){
    Serial.print("Water Temperature: ");
    Serial.print(sensors.getTempCByIndex(0));
    Serial.println(" degree C");
    Serial.println("Water Temperature high");
    delay(3000);
  }

  if(sensors.getTempCByIndex(0) >= 15 && sensors.getTempCByIndex(0) <= 40){
    Serial.print("Water Temperature: ");
    Serial.print(sensors.getTempCByIndex(0));
    Serial.println(" degree C");
    Serial.println("Water Temperature normal");
  }

  if(sensors.getTempCByIndex(0) < 15){
    Serial.print("Water Temperature: ");
    Serial.print(sensors.getTempCByIndex(0));
    Serial.println(" degree C");
    Serial.println("Water Temperature low");
    delay(3000);
  }
  delay(8000);
}

void turbidity() {
  Serial.println(" ");
  Serial.println("Taking Readings from turbidity Sensor");
  int turbidityValue = analogRead(A1);
  float turbidityV = turbidityValue/100;
  Serial.print("Turbidity level: ");
  Serial.println(turbidityV);

  if( turbidityV > 9){
    Serial.print("Turbidity Level: ");
    Serial.print(turbidityV);
    Serial.println("NTU");
    Serial.println("Water Very Clean ");
    delay(3000);
  }

  if( turbidityV >= 6 && turbidityValue/100 <= 9 ){
    Serial.print("Turbidity Level: ");
    Serial.print(turbidityV);
    Serial.println("NTU");
    Serial.println("Water Clean ");
  }

  if( turbidityV < 6){
    Serial.print("Turbidity Level: ");
    Serial.print(turbidityV);
    Serial.println("NTU");
    Serial.println("Water Very Dirty ");
    delay(3000);
   }
  delay (8000);
}

//////////////////////////////////////////////////////// SMS FUNCTION /////////////////////////////////////////////////////////////////////
void send_sms(){
  String temp;
  String phm;
  String turb;
  int turbidityValue = analogRead(A1);
  float turbidityV = turbidityValue/100;

  int buf[10];                //buffer for read analog
  for(int i=0;i<10;i++)       //Get 10 sample value from the sensor for smooth the value
  {
    buf[i]=analogRead(SensorPin);
    delay(10);
  }
  for(int i=0;i<9;i++)        //sort the analog from small to large
  {
    for(int j=i+1;j<10;j++)
    {
      if(buf[i]>buf[j])
      {
        int temp=buf[i];
        buf[i]=buf[j];
        buf[j]=temp;
      }
    }
  }
  avgValue=0;
  for(int i=2;i<8;i++)                      //take the average value of 6 center sample
    avgValue+=buf[i];
  float phValue=(float)avgValue*3.8/1030/6; //convert the analog into millivolt
  phValue=3.3*phValue+Offset;

    if(sensors.getTempCByIndex(0) > 40){
    temp = String("HIGH");
    }
    if(sensors.getTempCByIndex(0) >= 10 && sensors.getTempCByIndex(0) <= 40){
    temp = String("NORMAL");
    }
    if(sensors.getTempCByIndex(0) < 10){
     temp = String("LOW");
    }

    if(phValue >= 7.30){
    phm = String("ALKALINE");
    }
    if(phValue >= 6.90 && phValue <= 7.19){
    phm = String("NORMAL");
    }
    if(phValue < 6.89){
     phm = String("ACIDIC");
    }

    if(turbidityV >= 6 && turbidityValue/100 <= 9){
    turb = String("CLEAN");
    }
    if(turbidityV < 6){
    turb = String("DIRTY");
    }

  mySerial.begin(19200);  //Default serial port setting for the GPRS modem is 19200bps 8-N-1
  mySerial.print("\r");
  delay(1000);                    //wait for a second while the modem sends an "OK"
  mySerial.print("AT+CMGF=1\r");    //Because we want to send the SMS in text mode
  delay(1000);
  mySerial.print("AT+CMGS=\"+447450867345\"\r");    //Start accepting the text for the message
  delay(1000);
  mySerial.print(temp);
  mySerial.print(" \r");
  mySerial.print("WATER TEMPERATURE= \r");   //The text for the message
  mySerial.print(sensors.getTempCByIndex(0));
  mySerial.print("*C\r");
  mySerial.println("\r");
  mySerial.print(" \r");
  mySerial.print(phm);
  mySerial.print(" \r");
  mySerial.print("WATER PH VALUE= \r");   //The text for the message
  mySerial.print(phValue);
  mySerial.println("\r");
  mySerial.print(turb);
  mySerial.print(" \r");
  mySerial.print("WATER TURBIDITY= \r");   //The text for the message
  mySerial.print(turbidityV);
  mySerial.print("NBT\r");
  delay(3000);
  mySerial.write(0x1A);  //Equivalent to sending Ctrl+Z
}

void send_sms1(){
  String temp;
  String phm;
  String turb;
  int turbidityValue = analogRead(A1);
  float turbidityV = turbidityValue/100;

  int buf[10];                //buffer for read analog
  for(int i=0;i<10;i++)       //Get 10 sample value from the sensor for smooth the value
  {
    buf[i]=analogRead(SensorPin);
    delay(10);
  }
  for(int i=0;i<9;i++)        //sort the analog from small to large
  {
    for(int j=i+1;j<10;j++)
    {
      if(buf[i]>buf[j])
      {
        int temp=buf[i];
        buf[i]=buf[j];
        buf[j]=temp;
      }
    }
  }
  avgValue=0;
  for(int i=2;i<8;i++)                      //take the average value of 6 center sample
    avgValue+=buf[i];
  float phValue=(float)avgValue*3.8/1030/6; //convert the analog into millivolt
  phValue=3.3*phValue+Offset;

    if(sensors.getTempCByIndex(0) > 40){
    temp = String("HIGH");
    }
    if(sensors.getTempCByIndex(0) >= 10 && sensors.getTempCByIndex(0) <= 40){
    temp = String("NORMAL");
    }
    if(sensors.getTempCByIndex(0) < 10){
     temp = String("LOW");
    }

    if(phValue >= 7.30){
    phm = String("ALKALINE");
    }
    if(phValue >= 6.90 && phValue <= 7.19){
    phm = String("NORMAL");
    }
    if(phValue < 6.89){
     phm = String("ACIDIC");
    }

    if(turbidityV >= 6 && turbidityValue/100 <= 9){
    turb = String("CLEAN");
    }
    if(turbidityV < 6){
    turb = String("DIRTY");
    }

  mySerial.begin(19200);  //Default serial port setting for the GPRS modem is 19200bps 8-N-1
  mySerial.print("\r");
  delay(1000);                    //wait for a second while the modem sends an "OK"
  mySerial.print("AT+CMGF=1\r");    //Because we want to send the SMS in text mode
  delay(1000);
  mySerial.print("AT+CMGS=\"+233265188849\"\r");    //Start accepting the text for the message
  delay(1000);
  mySerial.print(temp);
  mySerial.print(" \r");
  mySerial.print("WATER TEMPERATURE= \r");   //The text for the message
  mySerial.print(sensors.getTempCByIndex(0));
  mySerial.print("*C\r");
  mySerial.println("\r");
  mySerial.print(" \r");
  mySerial.print(phm);
  mySerial.print(" \r");
  mySerial.print("WATER PH VALUE= \r");   //The text for the message
  mySerial.print(phValue);
  mySerial.println("\r");
  mySerial.print(turb);
  mySerial.print(" \r");
  mySerial.print("WATER TURBIDITY= \r");   //The text for the message
  mySerial.print(turbidityV);
  mySerial.print("NBT\r");

  delay(3000);
  mySerial.write(0x1A);  //Equivalent to sending Ctrl+Z
}