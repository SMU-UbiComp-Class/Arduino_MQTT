
/*
  Manipulated from Web client Example

 
 */

 
// using MQTT server files from https://github.com/hirotakaster/MQTT
// To use, download from above, place in "Library" for the arduino
// Then you might need to add a "maintainer=SomeUser" in the java.properties 
//  so that the code passes the check (also adding other properties will suppress warnings)
#include "MQTT.h"

/* 
 * Defaultly disabled. More details: https://docs.particle.io/reference/firmware/photon/#system-thread 
 */
//SYSTEM_THREAD(ENABLED);

/*
 * Defaultly disabled. If BLE setup is enabled, when the Duo is in the Listening Mode, it will de-initialize and re-initialize the BT stack.
 * Then it broadcasts as a BLE peripheral, which enables you to set up the Duo via BLE using the RedBear Duo App or customized
 * App by following the BLE setup protocol: https://github.com/redbear/Duo/blob/master/docs/listening_mode_setup_protocol.md#ble-peripheral 
 * 
 * NOTE: If enabled and upon/after the Duo enters/leaves the Listening Mode, the BLE functionality in your application will not work properly.
 */
//BLE_SETUP(ENABLED);

/*
 * SYSTEM_MODE:
 *     - AUTOMATIC: Automatically try to connect to Wi-Fi and the Particle Cloud and handle the cloud messages.
 *     - SEMI_AUTOMATIC: Manually connect to Wi-Fi and the Particle Cloud, but automatically handle the cloud messages.
 *     - MANUAL: Manually connect to Wi-Fi and the Particle Cloud and handle the cloud messages.
 *     
 * SYSTEM_MODE(AUTOMATIC) does not need to be called, because it is the default state. 
 * However the user can invoke this method to make the mode explicit.
 * Learn more about system modes: https://docs.particle.io/reference/firmware/photon/#system-modes .
 */
#if defined(ARDUINO) 
SYSTEM_MODE(SEMI_AUTOMATIC); 
#endif



// your network name also called SSID
char ssid[] = "UbicompGuest";
// your network password
char password[] = "aksjdhaksjhdkajshd"; // this is not currently used

// MQTTServer to use
char server[] = "192.168.2.2";// point this to the mosquitto server

// callback for what happens on subscribed event
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Received message for topic ");
  Serial.print(topic);
  Serial.print(" with length ");
  Serial.println(length);
  Serial.println("Message:");
  Serial.write(payload, length);
  Serial.println();
}

// Initialize the MQTT Server
MQTT client(server, 1883, callback);


// give protoype functions
void printWifiStatus();
void connectMQTT();


void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(57600);

  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to Network named: ");
  // print the network name (SSID);
  Serial.println(ssid); 
  
  // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
  WiFi.on();
  //WiFi.setCredentials(ssid,password); // have password?
  WiFi.setCredentials(ssid); // or just open connection?
  WiFi.connect();
  
  while ( WiFi.connecting()) {
    // print dots while we wait to connect
    Serial.print(".");
    delay(300);
  }
  
  Serial.println("\nYou're connected to the network");
  Serial.println("Waiting for an ip address");
  
  IPAddress localIP = WiFi.localIP();
  while (localIP[0] == 0) {
    localIP = WiFi.localIP();
    Serial.println("waiting for an IP address");
    delay(1000);
  }

  Serial.println("\nIP Address obtained");
  printWifiStatus();

  Serial.println("\nStarting connection to server...");
  // if you get a connection, start subscription services
  connectMQTT();
}

void loop() {
  // Reconnect if the connection was lost
  connectMQTT();

  int adcVal = analogRead(A3);
  char tmp[30];
  sprintf(tmp,"%d",adcVal);
  
  if(client.publish("$ubicomp/instructor2",tmp)) {
    Serial.println("Publish success");
  } else {
    Serial.println("Publish failed");
  }

  // Check if any message were received
  // on the topic we subscribed to
  if (client.isConnected()){
        client.loop();
        delay(5000); // Hmm... is this the best way to do this???
        // during the in class assignment, you will likely need to fix this
  }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void connectMQTT(){
  // make sure we are not already connected
  if (!client.isConnected()) {
    Serial.println("Reconnecting....");

    if(!client.connect("energiaClient")) {
      Serial.println("Connection failed");
    } else {
      Serial.println("Connection success");
      
      // now lets subscribe to the topic we are interested in knowing about
      if(client.subscribe("$SYS/Eric")) {
        Serial.println("Subscription successfull to SYS/Eric");
      }
    }
  }
}

