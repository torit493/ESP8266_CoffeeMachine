//Libraries. I dont think i can add comments at the end of lines here for some reason... Should check this at some point
#pragma once
#include <Servo.h>
#include <AccelStepper.h>
// Maybe cleaner if moved to webpage header?:
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h> // This is needed to handle http requests i think. Should maybe try changing later....
#include <ESP8266WebServer.h>  // This has to be used to be able to send to server...
#include <NTPClient.h> // is a tima library that handles NTP server synchronization "beautifully"
#include <WiFiUdp.h> //handles UTP protocol tasks such as opening a UDP port, send/recieve UDP packets etc...
#include "CoffeeMachine.h"
#include "webpage.h"
#include <String.h>


//WiFI stuff:###########################################################
const char* ssid     = "Torint_2.4";                                  // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "AEYE9U43PN3VE";                               // The password of the Wi-Fi network

ESP8266WebServer server(80);                                          // setting server port to 80 (FOR SOME REASON? IMPORTANT NUMBER??)
String header;                                                        // variable to store the HTTP request
HTTPClient http;                                                      // client used to listen for incoming clients in loop
IPAddress local_IP(192, 168, 0, 22);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);
int clientTime;                                                       // variable to store when client came. For controlling timeout
//######################################################################

//Pins used:############################################################
const uint8_t powerservo_p  = D5;                                     // GPIO14, D5 pin for power button
const uint8_t IN1           = D1;                                     // ULN2003 Motor Driver Pins
const uint8_t IN2           = D2;
const uint8_t IN3           = D7;
const uint8_t IN4           = D6;
const uint8_t vib_p1        = D3;                                     // Controlling vib motor via shield 
const uint8_t vib_p2        = D4;
//######################################################################

//Physical parameters:##################################################
const int power_deg           = 95;                                   // deg for power on
const int zero_deg            = 0;                                    // neutral position of servo pressing the power button
const int delay_time          = 10;                                   // chills out the speed og yhe servo when using powerOn code.
const int filter_open         = -1360;                                // steps_per_revolution/3*2. Found from tiral and error... Maybe fix the wiring so this can be positive?
const int filter_closed       = 0;                                    // position when closed
const int stepsPerRevolution  = 2048;                                 // change this to fit the number of steps per revolution
const int stepper_speed       = 500;                                  // set to 500 from code on web. Seems OK...
const int stepper_acc         = 100;                                  // set to 100 from code on web. Seems OK...
//######################################################################

//Temporary condition for testing code:#################################
bool open_filter  = false;
bool do_once      = false;
//######################################################################

// Objects:#############################################################
Servo powerservo;                                                     // servo to press thermostat and lights
AccelStepper stepper(AccelStepper::HALF4WIRE, IN1, IN3, IN2, IN4);    // stepper for opening the filter for change
CoffeeMachine TITsBraun = CoffeeMachine();                                            // default set to brew coffee at 0700, but false on starting condition
//######################################################################

//Clock stuff:##########################################################
const long utcOffsetInSeconds = 3600*2;                               // an offset for your timezone in seconds
WiFiUDP ntpUDP;                                                       // an instance for handling UDP stuff i guess...
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);     // object with "handler" for UDP stuff connecting to "...org"
//######################################################################

//Condition: ###########################################################
int brewstage = 0;
bool start = true;
//######################################################################

//######################################################################
//########################## Functions: ################################
//######################################################################

// Servo press, back and forth
void powerOn(Servo aServo, int pin, double angle, double zero_deg){
    aServo.attach(pin);
    int curr_angle = zero_deg;
    while(curr_angle < angle){
      curr_angle += 1;
      aServo.write(zero_deg + curr_angle);
      delay(delay_time); // To chill on the pressing speed.
    }
    while(curr_angle > zero_deg){
      curr_angle -= 1;
      aServo.write(zero_deg + curr_angle);
      delay(delay_time); // To chill on the pressing speed.
    }
    aServo.detach();
}

// Set servo to a certain position. I dont think i should do it this way. Just makes the code difficult to read. But it cleans the loop....
void zeroPosition(Servo aServo, int pin, double angle){
  aServo.attach(pin);
  aServo.write(angle);
  delay(1000); // Wait one second after writing angle to allow for finishing
  aServo.detach();
}

void info(){
  int hour = timeClient.getHours();
  int minutes = timeClient.getMinutes();
  String temp; //Temporary string to hold http code for webpage
  temp = head + "<h2>Welcome to the webpage of the house!</h2>"
          "<h3>Clock: </h3>" + hour + ":" + minutes +
          "<h3>Alternatives: </h3>";
  temp +="<a href=\"/brewNow\">/brewNow</a> &emsp;&emsp;-> sets brewing time to current time and starts brewing coffee.<br>"
                "<a href=\"/openFilter\">/openFilter</a> &emsp;&ensp;&nbsp;-> opens filter for changing.<br>"
                "<a href=\"/closeFilter\">/closeFilter</a> &emsp;&emsp;&nbsp; -> closes filter.<br>"
                "<a href=\"/status\">/status</a> &nbsp;&emsp;&emsp;&emsp;-> information on scheduled brewing<br>"
                + ending;
  server.send(200,"text/html", temp);
}

void brewNow(){
  if (TITsBraun.getFilterState()){
    TITsBraun.setBrewSchedule(timeClient.getHours(), timeClient.getMinutes());
    String temp = head + "Started brewing." + ending;
    server.send(200,"text/html", temp);
  }else{
    String temp = head + "Clean filter before brewing!" + ending;
    server.send(200,"text/html", temp);
  }
}

// ########## THIS MAKES ESP CRASH AND RESTART??? !!!! ############## It starts opening but craches before finsihes
void openFilter(){
  if (TITsBraun.getFilterOpen()){
    String temp = head + "Filter already opened!" + ending;
    server.send(200,"text/html", temp);
  }else if (TITsBraun.getCurrentlyBrewing()){       // This should be made more sophisticated! figure out how long time it takes to brew and make a function that accurately calculateshow long you have brewed...
    String temp = head + "Currently brewing, please wait before opening." + ending;
    server.send(200, "text/html", temp);
  }else{
    stepper.moveTo(filter_open);
    String temp = head + "Opening filter." + ending;
    server.send(200,"text/html", temp);
    while (stepper.distanceToGo() < 0){
      stepper.run();
    }
  temp = head + "Filter fully open now." + ending;
  server.send(200, "text/html", temp);
  }
}

void closeFilter(){
  String temp = head;
  if (TITsBraun.getFilterOpen()){ // returns true if filter is open
    stepper.moveTo(filter_closed);
    temp = temp + "Closing filter." + ending;
    //server.send(200,"text/html", temp);
    while (stepper.distanceToGo()!=0){
      stepper.run();
    }
    TITsBraun.setFilterState(true);
  }else{
    temp = temp + "Filter already closed!" + ending;
    //server.send(200, "text/html", temp);
  }
  server.send(200, "text/html", temp);
}

void status(){
  String temp = head + "Clock set to: " + timeClient.getHours() + ":" + timeClient.getMinutes();
  if (TITsBraun.getScheduledCoffee() && TITsBraun.getFilterState()){
    temp = temp + "<br>Set to brew at: " + TITsBraun.getCoffeeHours() + ":" + TITsBraun.getCoffeeMinutes() + ". ";
  }else if (TITsBraun.getCurrentlyBrewing()){
    temp = temp + "<br>Unless you drank all the coffee insanely fast there is currently coffee ready! :D";
  }else{
    temp = temp + "<br>No brewing sceduled.";
  }
  if (!TITsBraun.getFilterState()){
    temp = temp + "<br>The filter needs to be changed!";
  }
  temp = temp + ending;
server.send(200, "text/html", temp);
}

void updateTime(){ // I dont know if this will ever actually be useful, but just in case.
  timeClient.update();
  delay(500);
  String temp = head + "Clock has been updated to: " + timeClient.getHours() + ":" + timeClient.getMinutes() + ending;
  server.send(200, "text/html", temp);
}

void runVibMotor(){
  digitalWrite(vib_p1, HIGH);
  digitalWrite(vib_p2, HIGH);
  delay(60);
  digitalWrite(vib_p1, LOW);
  digitalWrite(vib_p2, LOW);
  delay(20);
}