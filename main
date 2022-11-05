#include "CoffeeMachine.h"
#include "func.h"

void setup() {
  Serial.begin(115200);

  // Pin setup: #######################################################################
  pinMode(TITsBraun.getVibPin(), OUTPUT);

  // WiFi: ############################################################################
  if (!WiFi.config(local_IP, gateway, subnet)) { // THIS IS USED TO ALWAYS CONNECT TO THE SAME IP
    Serial.println("STA Failed to configure");
  }
  WiFi.begin(ssid, password);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid); 
  Serial.println(" ...");
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }
  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // 

  // Time: ############################################################################
  timeClient.begin();   // This initializes the NTP client
  delay(1000);
  timeClient.update(); // This updates the time information
  Serial.print("Current time: ");
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.println(timeClient.getMinutes());

  // Server functions: ################################################################
  server.begin();
  server.on("/", info);
  server.on("/brewNow", brewNow);
  server.on("/openFilter", openFilter);
  server.on("/closeFilter", closeFilter);
  server.on("/status", status);
  server.on("/updateTimeClient", updateTimeClient);

  // CoffeeMachine: ###################################################################
  TITsBraun.setBrewSchedule(timeClient.getHours(), timeClient.getMinutes()+1);

}

void loop() {
  // ##### Server code: #####
  server.handleClient();
  
  // ##### Brewing code: #####
  TITsBraun.update(timeClient.getHours(), timeClient.getMinutes());
  if (TITsBraun.timeToBrew()){
    while (!TITsBraun.getFilterOpen()){
      TITsBraun.openFilter();
    }
    TITsBraun.fillCoffee(); // uses delay()!!!
    while (TITsBraun.getFilterOpen()){
      TITsBraun.closeFilter();
    }
    TITsBraun.powerOn();    // uses delay()!!!
  }
  if (!TITsBraun.getCurrentlyBrewing()){ // opens filter when no longer brewing so its easy to change, currently_brewing is set false one hour after brew start by update() function.
    if (!TITsBraun.getFilterOpen()){
      TITsBraun.openFilter();
    }
  }
}
