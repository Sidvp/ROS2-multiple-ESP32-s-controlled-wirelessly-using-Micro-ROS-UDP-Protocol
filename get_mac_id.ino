// To obtain MAC ID of ESP32:
// The MAC address is a unique identifier for the device's network interface.

// When this code is uploaded and run on ESP32 microcontroller, it will print the MAC address of the device to the serial monitor once, 
// and then it will continue to run the empty loop() function indefinitely.



#include <WiFi.h>  // includes WiFi library required for ESP32 to work on WiFi.

void setup() {    // setup function, which runs once when microcontroller is powered on/ reset.
  // put your setup code here, to run once:

  Serial.begin(115200); // Serial communication is initialized at a baud rate of 115200. 
                        // Serial monitor in the Arduino IDE can be used to view the output of the program.
  WiFi.mode(WIFI_MODE_STA);  // sets the WiFi mode to Station (STA) mode, which means ESP32 will connect to an existing WiFi network as a client.
  Serial.println(WiFi.macAddress());  // prints the MAC address of the device to the serial monitor. 
}

void loop() {     // Loop function, which runs repeatedly after the setup function has completed.
  // put your main code here, to run repeatedly:

}
