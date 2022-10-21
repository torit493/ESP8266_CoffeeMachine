// WiFi stuff: You should check if you need all of these!
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>  // This is needed to handle http requests i think. Should maybe try changing later....
#include <ESP8266WebServer.h>   // This has to be used to be able to send to server...
#include <NTPClient.h>          // is a time library that handles NTP server synchronization "beautifully"
#include <WiFiUdp.h>            // handles UTP protocol tasks such as opening a UDP port, send/recieve UDP packets etc...
#include "webpage.h"

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
//######################################################################

//Clock stuff:##########################################################
const long utcOffsetInSeconds = 3600*2;                               // an offset for your timezone in seconds
WiFiUDP ntpUDP;                                                       // an instance for handling UDP stuff i guess...
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);     // object with "handler" for UDP stuff connecting to "...org"
//######################################################################

//CoffeeMachine:########################################################
CoffeeMachine TITsBraun = CoffeeMachine(D1, D2, D7, D6, D5, D3, D4);
//######################################################################

//######################################################################
//######################### Server Functions ###########################
//######################################################################

void info(){
  int hour = timeClient.getHours();
  int minutes = timeClient.getMinutes();
  String temp; //Temporary string to hold http code for webpage
  temp = head + "<h2>Welcome to the webpage of the house!</h2>"
          "<h3>Clock: </h3>" + hour + ":" + minutes +
          "<h3>Alternatives: </h3>";
  temp += "<a href=\"/brewNow\">/brewNow</a> &emsp;&emsp;-> sets brewing time to current time and starts brewing coffee.<br>"
          "<a href=\"/openFilter\">/openFilter</a> &emsp;&ensp;&nbsp;-> opens filter for changing.<br>"
          "<a href=\"/closeFilter\">/closeFilter</a> &emsp;&emsp;&nbsp; -> closes filter.<br>"
          "<a href=\"/status\">/status</a> &nbsp;&emsp;&emsp;&emsp;-> information on scheduled brewing<br>"
          "<a href=\"/updateTimeClient\">/updateTimeClient</a> &nbsp;&emsp;&emsp;&emsp;-> Updating time for time client.<br>"
          + ending;
  server.send(200,"text/html", temp);
}

void updateTimeClient(){ // I dont know if this will ever actually be useful, but just in case.
  timeClient.update();
  delay(500);
  String temp = head + "Clock has been updated to: " + timeClient.getHours() + ":" + timeClient.getMinutes() + ending;
  server.send(200, "text/html", temp);
}

void status(){
  String temp = head + "Clock set to: " + timeClient.getHours() + ":" + timeClient.getMinutes();
  if (TITsBraun.getBrewingScheduled() && !TITsBraun.getFilterOpen()){
    temp = temp + "<br>Set to brew at: " + TITsBraun.getBrewHour() + ":" + TITsBraun.getBrewMin() + ". ";
  }else if (TITsBraun.getCurrentlyBrewing()){
    temp = temp + "<br>Unless you drank all the coffee insanely fast there is currently coffee ready! :D";
  }else{
    temp = temp + "<br>No brewing sceduled.";
  }
  if (TITsBraun.getFilterOpen()){
    temp = temp + "<br>The filter is open so you can change it!";
  }
  temp = temp + ending;
  server.send(200, "text/html", temp);
}

void closeFilter(){
  String temp = head;
  if (TITsBraun.getFilterOpen()){ // returns true if filter is open
    while (TITsBraun.getFilterOpen()){
      TITsBraun.closeFilter();
    }
    temp = temp + "Filter closed." + ending;
  }else{
    temp = temp + "Filter already closed!" + ending;
  }
  server.send(200, "text/html", temp);
}

void openFilter(){
  String temp = head;
  if (TITsBraun.getFilterOpen()){
    temp = temp + "Filter already opened!" + ending;
  }else if (TITsBraun.getCurrentlyBrewing()){
    temp = temp + "Currently brewing, please wait before opening." + ending;
  }else{
    while (!TITsBraun.getFilterOpen()){
      TITsBraun.openFilter();
    }
    temp = temp + "Filter fully open now." + ending;
  }
  server.send(200, "text/html", temp);
}

void brewNow(){
  String temp = head;
  if (!TITsBraun.getFilterOpen() && !TITsBraun.getCurrentlyBrewing()){
    TITsBraun.setBrewSchedule(timeClient.getHours(), timeClient.getMinutes());
    temp = temp + "Set brewing scheduled to now." + ending;
  }else{
    temp = temp + "Clean filter and close it before brewing!" + ending;
  }
  server.send(200,"text/html", temp);
}