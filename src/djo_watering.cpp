/* 
 * Project myProject
 * Author: Your Name
 * Date: 
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"

int waterThreshold = 2800;

int measR = 0;
unsigned long TimerMeas = 0;
unsigned long TimerNotify = 0;
int GroundPIN = A0;
int SensePIN = A1;
int PowerPIN = A2;
bool LightState = FALSE;

STARTUP(WiFi.selectAntenna(ANT_AUTO)); // continually switches at high speed between antennas

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(DISABLED);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);

void setup() {
    WiFi.setCredentials("orser_g", "illusionsofthemind", WPA2, WLAN_CIPHER_AES);
    WiFi.setCredentials("orser_g_garage", "illusionsofthemind", WPA2, WLAN_CIPHER_AES);
    WiFi.setCredentials("csetech", "ZestarYum21", WPA2, WLAN_CIPHER_AES);
  
    pinMode(D0,OUTPUT);
    digitalWrite(D0,LOW); // Red  LED
    pinMode(D1,OUTPUT);
    digitalWrite(D1,LOW); // Green LED

    pinMode(SensePIN,INPUT);
    pinMode(GroundPIN,OUTPUT);
    pinMode(PowerPIN,OUTPUT);
    digitalWrite(GroundPIN,LOW);
    digitalWrite(PowerPIN,LOW);

    Serial.begin(9600);
    Particle.variable("measR", measR);
    Particle.variable("waterThreshold", waterThreshold);
}

void loop() {
    
    if(TimerMeas < millis()) {
        digitalWrite(PowerPIN,HIGH);
        delay(500);
        measR = analogRead(SensePIN);
        
        Serial.print("test:");
        Serial.println(measR);
        
        Particle.publish("djoWater_measR",String(measR),PRIVATE);
        
        digitalWrite(PowerPIN,LOW);
        TimerMeas = millis() + 1000*60*60;  // wait 60 minutes
        TimerMeas = millis() + 2*60*1000;  // wait 2 minutes (testing mode)
    }

    if(measR > waterThreshold && millis() > TimerNotify) { //fails once every 40 days
        Particle.publish("djoWater_WaterMe",String(measR),PRIVATE);
        TimerNotify = millis() + 1000*60*60*24; // Wait 24 hours
    }
    
    if(measR > waterThreshold) {LightState = TRUE;} else {LightState = FALSE;}
    digitalWrite(D0,LightState);
}