#include<SoftwareSerial.h> //Included SoftwareSerial Library
//Started SoftwareSerial at RX and TX pin of ESP8266/NodeMCU
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
SoftwareSerial s(3,1);

WiFiClient client;
unsigned long channelNumber = 1375990; //Thingspeak channel number
const char * ReadAPIKey = "GTBDYJMLOAX6G164"; //Read API key for reading the incoming payload
const int FieldNumber1 = 1; //field number to read from


void setup() {
  // put your setup code here, to run once:
  s.begin(9600); //begin the serial communication with the baud rate of 9600bps
  WiFi.begin("NU-Student", "nustudents"); //connect wi-fi SSID and PASSWORD replace them with your own
  Serial.print("Connecting");
  while(WiFi.status() != WL_CONNECTED){  //Checking the connection status
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected, IP Address: "); 
  Serial.print(WiFi.localIP());  //print the IP Address of the wifi connected to
  ThingSpeak.begin(client);
}

void loop() {
  // put your main code here, to run repeatedly:
  int A = ThingSpeak.readLongField(channelNumber ,FieldNumber1, ReadAPIKey); //Read the data on the thingspeak cloud replace the channelNumber,FieldNumber and ReadAPIKey with your own
  s.write(A); //Writes to the Arduino via Serial Communication (UART)
  Serial.println(A); //print the received data for conformation.
}
