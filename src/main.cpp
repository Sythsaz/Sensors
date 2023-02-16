#include <Arduino.h>
#include <CCS811.h>
#include <SPI.h>
#include <WiFiNINA_Generic.h>
#include <secrets.h>

static bool secDebug = SECRET_DEBUG;
int CO2PPM = 0;
int TVOCPPB = 0;
bool CCS811Fail = false;
bool sensorCheck = false;
bool wifi_setup = false;

/*
 * IIC address default 0x5A, the address becomes 0x5B if the ADDR_SEL is soldered.
 */
// CCS811 sensor(&Wire, /*IIC_ADDRESS=*/0x5A);
CCS811 sensor;

char ssid[] = SECRET_SSID;   // your network SSID (name)
char pass[] = SECRET_PASS;   // your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS; // the WiFi radio's status

void setup()
{
  if (secDebug == true)
  {
    Serial.begin(9600);
  }

  while (sensor.begin() != 0)
  {
    delay(1000);
  }
  /**
   * @brief Set measurement cycle
   * @param cycle:in typedef enum{
   *                  eClosed,      //Idle (Measurements are disabled in this mode)
   *                  eCycle_1s,    //Constant power mode, IAQ measurement every second
   *                  eCycle_10s,   //Pulse heating mode IAQ measurement every 10 seconds
   *                  eCycle_60s,   //Low power pulse heating mode IAQ measurement every 60 seconds
   *                  eCycle_250ms  //Constant power mode, sensor measurement every 250ms
   *                  }eCycle_t;
   */
  sensor.setMeasCycle(sensor.eCycle_250ms);

  if (WiFi.status() == WL_NO_MODULE)
  {
    if (secDebug == true)
    {
      Serial.println("Communication with WiFi module failed!");
    }
    // don't continue
    while (true)
      ;
  }

  while (status != WL_CONNECTED)
  {
    if (secDebug == true)
    {
      Serial.print("Attempting to connect to WPA SSID: ");
      Serial.println(ssid);
    }
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }
  if (secDebug == true)
  {
    // you're connected now, so print out the data:
    Serial.print("You're connected to the network");
  }
}

void printMacAddress(byte mac[])
{
  for (int i = 5; i >= 0; i--)
  {
    if (mac[i] < 16)
    {
      if (secDebug == true)
      {
        Serial.print("0");
      }
    }
    if (secDebug == true)
    {
      Serial.print(mac[i], HEX);
    }
    if (i > 0)
    {
      if (secDebug == true)
      {
        Serial.print(":");
      }
    }
  }
  if (secDebug == true)
  {
    Serial.println();
  }
}

void printWifiData()
{
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  if (secDebug == true)
  {
    Serial.print("IP Address: ");
    Serial.println(ip);
    Serial.println(ip);
  }

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  if (secDebug == true)
  {
    Serial.print("MAC address: ");
  }
  printMacAddress(mac);
}

void printCurrentNet()
{
  // print the SSID of the network you're attached to:
  if (secDebug == true)
  {
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());
  }

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  if (secDebug == true)
  {
    Serial.print("BSSID: ");
  }
  printMacAddress(bssid);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  if (secDebug == true)
  {
    Serial.print("signal strength (RSSI):");
    Serial.println(rssi);
  }

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  if (secDebug = true)
  {
    Serial.print("Encryption Type:");
    Serial.println(encryption, HEX);
    Serial.println();
  }
}

bool GetCCS811()
{
  if (sensor.checkDataReady() == true)
  {
    CO2PPM = sensor.getCO2PPM();
    TVOCPPB = sensor.getTVOCPPB();
    return true;
  }
  else
  {
    return false;
    CCS811Fail = true;
  }
}

void loop()
{
  delay(250);
  GetCCS811();

  if (CCS811Fail = true)
  {
    if (secDebug = true)
    {
      Serial.println("CCS811 Failure?");
    }
  }
  else
  {
    if (secDebug = true)
    {
      Serial.println("CCS811 succeeded?");
    }
  }

  /*!
   * @brief Set baseline
   * @param get from getBaseline.ino
   */
  sensor.writeBaseLine(0x847B);
  // delay cannot be less than measurement cycle
  // delay(1000);
  if (wifi_setup == false)
  {
    printCurrentNet();
    printWifiData();
    wifi_setup = true;
  }
}