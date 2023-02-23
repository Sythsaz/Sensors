#include <Arduino.h>
#include <CCS811.h>
#include <SPI.h>
#include <WiFiNINA_Generic.h>
#include <MQTT.h>
#include <ArduinoJson.h>
#include <secrets.h>
// #include <havars.h>

static bool secDebug = SECRET_DEBUG;

intvars ints;   // declare common ints
boolvars bools; // declare common bools
/*
 * IIC address default 0x5A, the address becomes 0x5B if the ADDR_SEL is soldered.
 */
// CCS811 sensor(&Wire, /*IIC_ADDRESS=*/0x5A);
CCS811 sensor;
MQTTClient client;
unsigned long lastMillis = 0;

char ssid[] = SECRET_SSID;   // your network SSID (name)
char pass[] = SECRET_PASS;   // your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS; // the WiFi radio's status

// general class to simplify the syntax to write to a pin
class PinOut {
  public:
    PinOut(uint8_t pin): m_pin(pin) {
      pinMode(pin, OUTPUT);
    }

    PinOut& operator= (uint8_t state) {
      digitalWrite(m_pin, state ? HIGH : LOW);
      return *this;
    }

  private:
    uint8_t m_pin;
};

// example usage: create a "led" object, which uses the pin number LED_BUILTIN
PinOut led(LED_BUILTIN);

void setup()
{
  Serial.begin(9600);
  if (secDebug == true)
  {
    
    led = 1;
    delay(10);
    led = 0;
  }

  while (sensor.begin() != 0)
  {
    led = 1;
    delay(10);
    led = 0;
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
}

void connect()
{
  if (secDebug == true)
  {
    Serial.println("Checking WiFi...");
  }
  if (WiFi.status() == WL_NO_MODULE)
  {
    if (secDebug == true)
    {
      Serial.println("Communication with WiFi module failed!");
      led = 1;
      delay(10);
      led = 0;
    }
    // don't continue
    while (true);
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
    led = 1;
    delay(10);
    led = 0;
    // wait 10 seconds for connection:
    delay(10000);
  }
  if (secDebug == true)
  {
    // you're connected now, so print out the data:
    Serial.print("You're connected to the network");
  }
  if (secDebug == true)
  {
    Serial.print("\nconnecting...");
  }
  while (!client.connect("arduino", "public", "public"))
  {
    if (secDebug == true)
    {
      Serial.print(".");
    }
    delay(1000);
  }
  if (secDebug == true)
  {
    Serial.println("\nconnected!");
  }
  client.subscribe("/hello");

}

void messageReceived(String &topic, String &payload)
{
  Serial.println("incoming: " + topic + " - " + payload);

  // Note: Do not use the client in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `client.loop()`.
}

void printMacAddress(byte mac[])
{
  led = 1;
  delay(10);
  led = 0;
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
  led = 1;
  delay(10);
  led = 0;
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
  led = 1;
  delay(10);
  led = 0;
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

bool GetCCS811() // return true or false if data has been acquired or not
{
  if (sensor.checkDataReady() == true)
  {
    ints.CO2PPM0 = sensor.getCO2PPM();
    ints.TVOCPPB0 = sensor.getTVOCPPB();
    led = 1;
    delay(10);
    led = 0;
    bools.CCS811Fail = false;
    return true;
  }
  else
  {
    led = 1;
    delay(10);
    led = 0;
    bools.CCS811Fail = true;
    return false;
  }
}

void sendData()
{
  // client.publish("/hello", "world");
  // client.publish("/hello", "world");
}

void loop()
{
  delay(250);
  client.loop();

  if (!client.connected())
  {
    connect();
  }

  
  GetCCS811();

  // Get air quality data from sensor, return bool if acquired or not

  if (bools.CCS811Fail = true) // air quality data not acquired
  {
    if (secDebug = true)
    {
      Serial.println("CCS811 Failure?");
      led = 1;
      delay(150);
      led = 0;
    }
  }
  else if(bools.CCS811Fail = false) // air quality data acquired
  {
    if (secDebug = true)
    {
      Serial.println("CCS811 succeeded?");
      Serial.println(ints.CO2PPM0);  // Print out info
      Serial.println(ints.TVOCPPB0);
    }
  }

  /*!
   * @brief Set baseline
   * @param get from getBaseline.ino
   */
  sensor.writeBaseLine(0x847B);
  // delay cannot be less than measurement cycle
  // delay(1000);
  if (bools.wifi_setup == false) // Print wifi data once
  {
    printCurrentNet();
    printWifiData();
    bools.wifi_setup = true;
  }

  if (millis() - lastMillis > 1000)
  {
    lastMillis = millis();
    sendData();
  }
}
