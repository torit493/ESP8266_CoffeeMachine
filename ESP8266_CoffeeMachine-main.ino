#include "func.h"

void setup() {
  Serial.begin(115200);

  // CoffeeMachine: ###################################################################
  //TITsBraun.setBrewSchedule(timeClient.getHours(), timeClient.getMinutes()+1);
  TITsBraun.attach(power_servo_p);
  TITsBraun.setupPowerServo();
  TITsBraun.detach();

  // WiFi: ############################################################################
  // if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) { // THIS IS USED TO ALWAYS CONNECT TO THE SAME IP, fils to update time if used... why???
  //   Serial.println("STA Failed to configure");
  // }
  WiFi.begin(ssid, password);                         // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid); 
  Serial.println(" ...");

  IPAddress ip1 = ipaddr_addr("192.168.0.22"); // Brilliant fix: https://github.com/arduino-libraries/NTPClient/issues/58; need to give dns values
  IPAddress ip2 = ipaddr_addr("192.168.0.1");
  IPAddress ip3 = ipaddr_addr("255.255.255.000");
  IPAddress ip4 = ipaddr_addr( "8.8.8.8" );//dns 1: DNS translates domain names to IP addresses so browsers can load internet resources.
  IPAddress ip5 = ipaddr_addr( "8.8.4.4" );//dns 2
  if ( true ){ // Used to say ip1!=0, but this gives error message
    WiFi.config(ip1, ip2, ip3, ip4, ip5);
  } 

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {             // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }
  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());                     // I would love to make this static but cant update time when i do!

  // Time: ############################################################################
  timeClient.begin();                                 // This initializes the NTP client
  delay(1000);
  // i=0;
  // while(!timeClient.update()){
  //   timeClient.forceUpdate();                         // This updates the time information
  //   delay(1000);
  //   Serial.print(++i); Serial.print(' ');
  // }
  Serial.print("Current time: ");
  Serial.println(timeClient.getFormattedTime());      // prints hr:min:sec

  // Server functions: ################################################################
  server.begin();
  server.on("/", info);
  server.on("/brewNow", brewNow);
  server.on("/openFilter", openFilter);
  server.on("/closeFilter", closeFilter);
  server.on("/status", status);
  server.on("/updateTimeClient", updateTimeClient);   // Will enter a while loop until time is updated and printing to webpage number of attempts.
  server.on("/resetMachine", resetMachine);
  server.on("/enableMorningBrew", enableMorningBrew);
  server.on("/stepperClosed", setStepperClosed);
  server.on("/stepperOpened", setStepperOpened);
}

void loop() {
  // ##### Server code: #####
  server.handleClient();
  
  // ##### Brewing code: #####
  TITsBraun.update(timeClient.getHours(), timeClient.getMinutes());
  if (TITsBraun.timeToBrew()){
    while (!TITsBraun.getFilterOpen()){
      TITsBraun.openFilter(1); // 1 will open filter for filling, 2 will open fully
    }
    TITsBraun.attach(coffee_servo_p);
    TITsBraun.fillCoffee();
    TITsBraun.detach();
    while (TITsBraun.getFilterOpen()){
      TITsBraun.closeFilter();
    }
    TITsBraun.attach(power_servo_p);
    TITsBraun.powerOn();
    TITsBraun.detach();
  }
  if (!TITsBraun.getCurrentlyBrewing() && !TITsBraun.getNewFilter()){ // opens filter when no longer brewing so its easy to change, currently_brewing is set false one hour after brew start by update() function.
   if (!TITsBraun.getFilterOpen()){
     TITsBraun.openFilter(2);
   }
  }
}
