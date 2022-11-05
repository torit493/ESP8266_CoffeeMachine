// WiFi stuff: You should check if you need all of these!
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>  // This is needed to handle http requests i think. Should maybe try changing later....
#include <ESP8266WebServer.h>   // This has to be used to be able to send to server...
#include <NTPClient.h>          // is a time library that handles NTP server synchronization "beautifully"
#include <WiFiUdp.h>            // handles UTP protocol tasks such as opening a UDP port, send/recieve UDP packets etc...
#include "webpage.h"
#include "CoffeeMachine.h"

//WiFI stuff:###########################################################
const char* ssid     = "Torint_2.4";                                  // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "AEYE9U43PN3VE";                               // The password of the Wi-Fi network
ESP8266WebServer server(80);                                          // setting server port to 80 (FOR SOME REASON? IMPORTANT NUMBER??)
IPAddress local_IP(192, 168, 0, 22);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);
// String header;   //DONT THINK I USE THIS?                          // variable to store the HTTP request
// HTTPClient http; // OR THIS?                                       // client used to listen for incoming clients in loop
//######################################################################

//Clock stuff:##########################################################
const long utcOffsetInSeconds = 3600*1;                               // an offset for your timezone in seconds
WiFiUDP ntpUDP;                                                       // an instance for handling UDP stuff i guess...
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);     // object with "handler" for UDP stuff connecting to "...org"
//######################################################################

//CoffeeMachine:########################################################
uint8_t IN1           = D1;
uint8_t IN2           = D2;                                           // IN2 & IN3 are switched for IRL for some reason, so hook IN2 to IN3 and IN3 to IN2 with this code!!!
uint8_t IN3           = D7;
uint8_t IN4           = D6;
uint8_t power_servo_p = D5;
uint8_t coffee_servo_p= D4;
CoffeeMachine TITsBraun = CoffeeMachine(IN1, IN2, IN3, IN4, power_servo_p, coffee_servo_p);
//######################################################################

//######################################################################
//######################### Server Functions ###########################
//######################################################################

void info(){
  int hour = timeClient.getHours();
  int minutes = timeClient.getMinutes();
  String temp; //Temporary string to hold http code for webpage
  temp = head + "<h2> Welcome to my CoffeeMachine! &#9749 </h2>"
          "&thinsp;/&thinsp;################&thinsp;\<br>"
          "#&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&thinsp;#<br>"
          "#&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&thinsp;#<br>"
          "#&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&thinsp;#<br>"
          "#&emsp;&thinsp;/&thinsp;#############&thinsp;/<br>"
          "#&emsp;#&nbsp;______:____ &emsp;&emsp;&emsp;&emsp;&emsp;&emsp;(<br>"
          "#&emsp;#&nbsp;\\&ensp;&emsp;&emsp;&thinsp;:&emsp;&emsp;&nbsp;\\&ensp;&thinsp;_ &emsp;&emsp;&emsp;(&emsp;&thinsp;)&ensp;(<br>"
          "#&emsp;#&nbsp; |&emsp;&emsp;&ensp;:&emsp;&emsp;&ensp;&thinsp;|/&ensp;&thinsp;\\ &emsp;&emsp;&emsp;)&ensp;(&emsp;)<br>"
          "#&emsp;#&nbsp; |~~~~~~~~~~(&emsp;) &emsp;&ensp;&nbsp;&thinsp;_(____(_<br>"
          "#&emsp;#&nbsp; |&emsp;&emsp;&emsp;&emsp;&emsp;&ensp;|\\_/ &nbsp;&emsp;&thinsp;.-'-----------|<br>"
          "#&emsp;#&ensp;&thinsp;\\__________/ &emsp;&emsp;&thinsp;( c|\\/\\/\\/\\/\\/\\/\\|<br>"
          "#&emsp;\\&thinsp;##############&thinsp;\\ &emsp;&ensp;'-|/\\/\\/\\/\\/\\/\\/|<br>"
          "\\&thinsp;#################&thinsp;/ &emsp;&emsp;'-----------'<br>"
          "<h3> Clock: </h3>" + hour + ":" + minutes +
          "<h3> Alternatives: </h3>";
  temp += "<a href=\"/brewNow\">Start brewing</a> &emsp;&emsp;&emsp;&emsp; &#8594; sets brewing time to current time and starts brewing coffee.<br>"
		      "<a href=\"/openFilter\">Open filter</a> &emsp;&emsp;&emsp;&emsp;&emsp; &#8594; opens filter for changing.<br>"
          "<a href=\"/closeFilter\">Close filter</a> &emsp;&emsp;&emsp;&emsp;&emsp; &#8594; closes filter.<br>"
          "<a href=\"/status\">Display current status</a> &emsp;&#8594; information on scheduled brewing<br>"
          "<a href=\"/updateTimeClient\">Update the timeClient</a> &ensp;&nbsp;&thinsp;&#8594; Updating time for time client.<br>"
          "<a href=\"/resetMachine\">Reset CoffeeMachine</a> &emsp;&#8594; Careful with this one. For testing code.<br>"
          + ending;
  server.send(200,"text/html", temp);
}

void updateTimeClient(){
String temp = head + "Trying to update time...";
server.send(200, "text/html", temp + ending);
int i = 0;
while(!timeClient.update()){
  timeClient.forceUpdate();
  temp = temp + i;
  server.send(200, "text/html", temp + ending);
}
  temp = head + "Clock has been updated to: " + timeClient.getHours() + ":" + timeClient.getMinutes() + ending;
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
  if (TITsBraun.getFilterOpen()){
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
      TITsBraun.openFilter(2); // 1 will open for filling coffee, 2 will open for changing filter
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

void resetMachine(){
  String temp = head + "reset coffee machine parameters." + ending;
  TITsBraun.resetMachine();
  server.send(200, "text/html", temp);
}