//Code written by Avinash and Tommy at Hackathon
//Code was modified from Github sources that were based on sending emails from the NodeMCU ESP8266 module as well as ESP8266_GPS_OLED_Youtube on Github
#include <ESP8266WiFi.h>
#include <TinyGPS++.h>                                 // Tiny GPS Plus Library
#include <SoftwareSerial.h>                             // Software Serial Library so we can use other Pins for communication with the GPS module



static const int RXPin = 12, TXPin = 13;                // Ublox 6m GPS module to pins 12 and 13
static const uint32_t GPSBaud = 9600;                   // Ublox GPS default Baud Rate is 9600

const double Home_LAT = 34.029384;                      // Your Home Latitude
const double Home_LNG = -118.289342;                     // Your Home Longitude

TinyGPSPlus gps;                                        // Create an Instance of the TinyGPS++ object called gps
SoftwareSerial ss(RXPin, TXPin);

byte sendEmail();
byte eRcv(WiFiClientSecure client);
void efail(WiFiClientSecure client);
void displayInfo();

const char* SSID = "iPhone"; // wifi ssid
const char* PASS = "Bike Lock"; // ssid password
const char* user_base64 = "YmlrZWxvY2tpb3RAZ21haWwuY29t"; //your gmail account in base64 - https://www.base64encode.org/
const char* user_password_base64 = "YmlrZWxvY2sxMjM0"; //your gmail password in base64
const char* from_email = "MAIL From:<bikelockiot@gmail.com>"; //dummy from email
const char* to_email = "RCPT TO:<bikelockiot@gmail.com>"; //your gmail account to sent to

void setup()
{
  pinMode(16, INPUT);
  pinMode(13, OUTPUT);
  Serial.begin(115200);
  delay(10);
  Serial.println("");
  Serial.print("Connecting To ");
  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  Serial.println("WiFi Connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
    ss.begin(GPSBaud);
    displayInfo();
    sendEmail();

}

void loop()
{
  ESP.wdtDisable();

  }
void displayInfo()
{
  //We have new GPS data to deal with!
  Serial.println();
 Serial.print("Latitude  : ");
  Serial.println(gps.location.lat(), 5);
  Serial.print("Longitude : ");
  Serial.println(gps.location.lng(), 4);
  Serial.print("Satellites: ");
  Serial.println(gps.satellites.value());
  Serial.print("Elevation : ");
  Serial.print(gps.altitude.feet());
  Serial.println("ft"); 
  Serial.print("Time UTC  : ");
  Serial.print(gps.time.hour());                       // GPS time UTC 
  Serial.print(":");
  Serial.print(gps.time.minute());                     // Minutes
  Serial.print(":");
  Serial.println(gps.time.second());                   // Seconds
  Serial.print("Heading   : ");
  Serial.println(gps.course.deg());
  Serial.print("Speed     : ");
  Serial.println(gps.speed.mph());
  
  unsigned long Distance_To_Home = (unsigned long)TinyGPSPlus::distanceBetween(gps.location.lat(),gps.location.lng(),Home_LAT, Home_LNG);
  Serial.print("KM to Home: ");                        // Have TinyGPS Calculate distance to home and Serial it
  Serial.print(Distance_To_Home);
                            // Update Serial
  delay(200); 
  
  smartDelay(500);                                      // Run Procedure smartDelay

  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received: check wiring"));
}

byte sendEmail() //sending email function
{
  WiFiClientSecure client;

  if (client.connect("smtp.gmail.com", 465) == 1) {
    Serial.println(F("connected"));
  } else {
    Serial.println(F("connection failed"));
    return 0;
  }
  if (!eRcv(client)) return 0;
  Serial.println(F("--- Sending EHLO")); client.println("EHLO 1.2.3.4"); if (!eRcv(client)) return 0;
  Serial.println(F("--- Sending login")); client.println("AUTH LOGIN"); if (!eRcv(client)) return 0;
  Serial.println(F("--- Sending User base64")); client.println(user_base64); if (!eRcv(client)) return 0;
  Serial.println(F("--- Sending Password base64")); client.println(user_password_base64); if (!eRcv(client)) return 0;
  Serial.println(F("--- Sending From")); client.println(from_email); if (!eRcv(client)) return 0;
  Serial.println(F("--- Sending To")); client.println(to_email); if (!eRcv(client)) return 0;
  Serial.println(F("--- Sending DATA")); client.println(F("DATA")); if (!eRcv(client)) return 0;
  client.println(F("Subject: Esp8266 email test\r\n"));
  client.println(F("******This is from my ESP8266\n"));
  client.println();
  client.print("Latitude  : ");
  client.println(gps.location.lat(), 5);
  client.print("Longitude : ");
  client.println(gps.location.lng(), 4);
  client.print("Satellites: ");
  client.println(gps.satellites.value());
  client.print("Elevation : ");
  client.print(gps.altitude.feet());
  client.println("ft"); 
  client.print("Time UTC  : ");
  client.print(gps.time.hour());                       // GPS time UTC 
  client.print(":");
  client.print(gps.time.minute());                     // Minutes
  client.print(":");
  client.println(gps.time.second());                   // Seconds
  client.print("Heading   : ");
  client.println(gps.course.deg());
  client.print("Speed     : ");
  client.println(gps.speed.mph());
  
  unsigned long Distance_To_Home = (unsigned long)TinyGPSPlus::distanceBetween(gps.location.lat(),gps.location.lng(),Home_LAT, Home_LNG);
  client.print("KM to Home: ");                        // Have TinyGPS Calculate distance to home and client it
  client.print(Distance_To_Home);
  client.println(F("Hello "));
  client.println(F("."));
  if (!eRcv(client)) return 0;
  Serial.println(F("--- Sending QUIT"));
  client.println(F("QUIT"));
  if (!eRcv(client)) return 0;
  client.stop();
  Serial.println(F("disconnected"));
  return 1;
}

byte eRcv(WiFiClientSecure client)
{
  byte respCode;
  byte thisByte;
  int loopCount = 0;

  while (!client.available()) {
    delay(1);
    loopCount++;
    // if nothing received for 10 seconds, timeout
    if (loopCount > 10000) {
      client.stop();
      Serial.println(F("\r\nTimeout"));
      return 0;
    }
  }

  respCode = client.peek();
  while (client.available())
  {
    thisByte = client.read();
    Serial.write(thisByte);
  }

  if (respCode >= '4')
  {
    //  efail();
    return 0;
  }
  return 1;
}

void efail(WiFiClientSecure client)
{
  byte thisByte = 0;
  int loopCount = 0;

  client.println(F("QUIT"));

  while (!client.available()) {
    delay(1);
    loopCount++;
    // if nothing received for 10 seconds, timeout
    if (loopCount > 10000) {
      client.stop();
      Serial.println(F("\r\nTimeout"));
      return;
    }
  }

  while (client.available())
  {
    thisByte = client.read();
    Serial.write(thisByte);
  }
  client.stop();
  Serial.println(F("disconnected"));
}

static void smartDelay(unsigned long ms)                // This custom version of delay() ensures that the gps object is being "fed".
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

