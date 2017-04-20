#include <SPI.h>
#include <WiFi.h>
#include <WifiIPStack.h>
#include <Countdown.h>
#include <MQTTClient.h>

// I manipulated this example from the WiFi QuickStart Example Project

// For better security these should be placed in another file
//     called "credentials" or something similar


char ssid[] = "NETGEAR43"; // your network name also called SSID
char password[] = "classycomet157"; // your network password

// IBM IoT Foundation Cloud Settings
#define MQTT_MAX_PACKET_SIZE 100  // max allowed packet size
#define IBMSERVERURLLEN  64       // length of server name
#define IBMIOTFSERVERSUFFIX "messaging.internetofthings.ibmcloud.com" // server name suffix
char organization[] = "quickstart";  // we will use the "quickstart" function of bluemix
char typeId[] = "ubicog-class"; // we need an instance of the topic (this can be anything you want)
char pubtopic[] = "iot-2/evt/status/fmt/json"; // the format will be JSON
char deviceId[] = "000000000000"; // we will fill this in later, just set the right length for now
char clientId[64]; // the client name, we fill it in later

char mqttAddr[IBMSERVERURLLEN]; // the server for the MQTT upload
int mqttPort = 1883; // and the port 

MACAddress mac; // we will store the MAC address of the device with this object.
  
WifiIPStack ipstack;  
MQTT::Client<WifiIPStack, Countdown, MQTT_MAX_PACKET_SIZE> client(ipstack);

void setup() {
  uint8_t macOctets[6]; // this will store the pieces of the MAC address
  
  // print some stuff out to the console for debugging 
  Serial.begin(115200);
  
  Serial.print("Attempting to connect to Network named: "); // attempt to connect to Wifi network
  Serial.println(ssid); // print the network name (SSID)
  
  // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED) {
    // print dots while we wait to connect
    Serial.print(".");
    delay(300); // try about three times per second
  }
  
  Serial.println("\nYou're connected to the network!");
  Serial.println("Waiting for an ip address!");
  
  while (WiFi.localIP() == INADDR_NONE) {
    // print dots while we wait for an ip addresss
    Serial.print(".");
    delay(300);
  }

  // We are connected and have an IP address.
  Serial.print("\nIP Address obtained: ");
  Serial.println(WiFi.localIP());

  // get the MAC address
  mac = WiFi.macAddress(macOctets);
  Serial.print("MAC Address: ");
  Serial.println(mac);
  
  // Use MAC Address as deviceId, to be sure its unique!
  sprintf(deviceId, "%02x%02x%02x%02x%02x%02x", macOctets[0], macOctets[1], macOctets[2], macOctets[3], macOctets[4], macOctets[5]);
  Serial.print("deviceId: ");
  Serial.println(deviceId);

  // now get the strings for the MQTT client setup
  sprintf(clientId, "d:%s:%s:%s", organization, typeId, deviceId);
  sprintf(mqttAddr, "%s.%s", organization, IBMIOTFSERVERSUFFIX);

  // show the user where to view the online data from
  Serial.println("IBM IoT Foundation QuickStart example, view data in cloud here");
  Serial.print("--> http://quickstart.internetofthings.ibmcloud.com/#/device/");
  Serial.println(deviceId);
}

void loop() {

  int rc = -1;
  if (!client.isConnected()) {
    Serial.print("Connecting to ");
    Serial.print(mqttAddr);
    Serial.print(":");
    Serial.println(mqttPort);
    Serial.print("With client id: ");
    Serial.println(clientId);
    
    while (rc != 0) {
      rc = ipstack.connect(mqttAddr, mqttPort);
    }

    MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;
    connectData.MQTTVersion = 3;
    connectData.clientID.cstring = clientId;
    
    rc = -1;
    while ((rc = client.connect(connectData)) != 0)
      ;
    Serial.println("Connected\n");
  }

  
  char json[62];

  int sensorDataA3 = analogRead(A3);

  sprintf(json,
    "{\"d\":{\"myName\":\"UbiCogEricSensor\","
    "\"A3\":%d"
    "}}\0",sensorDataA3);
    
  Serial.print("Publishing: ");
  Serial.println(json);
  
  MQTT::Message message;
  message.qos = MQTT::QOS0; 
  message.retained = false;
  message.payload = json; 
  message.payloadlen = strlen(json);
  rc = client.publish(pubtopic, message);
  if (rc != 0) {
    Serial.print("Message publish failed with return code : ");
    Serial.println(rc);
  }
  
  // Wait for three seconds before publishing again
  client.yield(3000);
}


