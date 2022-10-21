#include "func.h"

void setup() {
  // Initialize the serial port #######################################################
  Serial.begin(115200);

  // Start pinModes: ##################################################################
  pinMode(vib_p1, OUTPUT);
  pinMode(vib_p2, OUTPUT);
  
  // Stepper: #########################################################################
  stepper.setMaxSpeed(stepper_speed);
  stepper.setAcceleration(stepper_acc);
  Serial.println("stepper initialized with: \n Max speed = 500 \n acceleration = 100");
  
  // Servo: ###########################################################################
  powerservo.attach(powerservo_p);
  powerservo.write(zero_deg);
  delay(500);
  powerservo.detach();                            // The reason I do this is beacuse I want it to run 24/7. If not done the servo pulls power. Waste...
  Serial.println("put servo in zero position");

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

  // Server functions: ################################################################
  server.begin();
  server.on("/", info);
  server.on("/brewNow", brewNow);
  server.on("/openFilter", openFilter);
  server.on("/closeFilter", closeFilter);
  server.on("/status", status);
  server.on("/updateTime", updateTime);

  // Time: ############################################################################
  timeClient.begin();   // This initializes the NTP client
  delay(1000);
  timeClient.update(); // This updates the time information
  Serial.print("Current time: ");
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.println(timeClient.getMinutes());

  // CoffeeMachine: ###################################################################
  TITsBraun.setBrewSchedule(timeClient.getHours(), timeClient.getMinutes()+1);
  TITsBraun.setFilterState(true);
  timeClient.update(); // This updates the time information
  Serial.println("fill time:");
  Serial.print(TITsBraun.getCoffeeHours());
  Serial.print(":");
  Serial.println(TITsBraun.getCoffeeMinutes());
  brewstage = 0;
  start = true;
}

void loop() {
  // ##### Server code: #####
  server.handleClient();

  // ##### Brewing code: #####
  if (TITsBraun.shouldBrewNow(timeClient.getHours(), timeClient.getMinutes())){
    if (brewstage == 0){                                      // open filter
      if (start){
        stepper.moveTo(filter_open);
        start = false;
        Serial.println("opening filter.");
      }
      else if (stepper.distanceToGo() == 0){
        brewstage++;
        start = true;
        TITsBraun.setFilterOpen(true);
        Serial.println("stepper reached open target.");
      }
      stepper.run();
    }
    else if (brewstage == 1){                                 // fill filter
      // fill in code for filling filter
      Serial.println("Filling coffee...");
      signed long timer = millis();
      signed long stopp = millis() + 5000;
      while (stopp - timer > 0){
        runVibMotor();
        timer = millis();
        Serial.println(stopp-timer);
      }
      brewstage++;
    }
    else if (brewstage == 2){                                 // close filter
      if (start) {
        stepper.moveTo(filter_closed);
        start = false;
        Serial.println("closing filter.");
      }
      else if (stepper.distanceToGo() == 0){
        brewstage++;
        start = true;
        TITsBraun.setFilterOpen(false);
        Serial.println("stepper reached closed target.");
      }
      stepper.run();
    }
    else if (brewstage == 3){                                 // power on machine
      powerOn(powerservo, powerservo_p, power_deg, zero_deg);
      brewstage++;
      TITsBraun.setPowerPressed(timeClient.getHours(), timeClient.getMinutes(), true);
      Serial.println("Power on.");
    }
  }
  else if (timeClient.getMinutes() - TITsBraun.getCoffeeMinutes() == 0 && brewstage == 4){
    brewstage = 0;
    Serial.println("Set brewstage to 0 again.");
  }
  if ((timeClient.getHours()-TITsBraun.getCoffeeHours())*60 + timeClient.getMinutes() - TITsBraun.getCoffeeMinutes() >= 60){
    TITsBraun.setCurrentlyBrewing(false);
    openFilter();
  }
}









