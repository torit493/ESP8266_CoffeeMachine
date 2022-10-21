#pragma once
#include "Arduino.h"

// This is the first draft for my coffee machine. It should keep track of time to brew, if brew has started, when it started. I think thats all.
// The second version will have a class containing other objects to see if that is cleaner/better...
// and maybe an update function which checks info on a webpage/phone if changes have been made.

class CoffeeMachine {
private:
  int coffee_hr, coffee_min; // Iwill use this determining when coffee should be made
  bool scheduled_coffee; // This says if i want coffee to be made at given time
  bool opened_filter; // position of filter ### I THINK THESE SHOULD NOT BE HERE IN A NICE CODE ###
  bool power_pressed; // has power been pressed? ### I THINK THESE SHOULD NOT BE HERE IN A NICE CODE ###
  int power_pressed_h, power_pressed_m; // hour and min of power pressed
  bool filling_coffee; // should be true while filling coffee. filter will close again when set false after filling started. (is the plan)
  bool new_filter; // Variable to check if a new filter is in the machine. Dont want to start brewing ontop of old coffee!
  bool currently_brewing; // Variable used to check if the machine has been turned on. NEED TO FIGURE OUT WHEN AND HOW THIS SHOULD BE RESET...

public:
  // Setup class:
  CoffeeMachine() {coffee_hr=7; coffee_min=0, scheduled_coffee=false; opened_filter=false;power_pressed=false;power_pressed_h=0;power_pressed_m=0;};
  CoffeeMachine(int coffee_hr, int coffee_min, bool scheduled_coffee);
  // Set functions:

  // Bool functions:
  bool shouldBrewNow(int hr, int min); //compared hr, min with brew time and returns if time to brew. Make sure not runs multiple times in loop...
  bool isOn(int hr, int min); // compares hr, min with power_pressed_h/m and returns false is <1hr since pressed
  void update(int hr, int min); // I want this to change condition of power_pressed when sufficient time passes.
  bool allowFilterChange(); // returns true if it is possible to change filter at curents time

  bool getFillingCoffee();
  int getCoffeeMinutes();
  int getCoffeeHours();
  bool getFilterState(); // true if ready, false if used
  bool getFilterOpen();
  bool getScheduledCoffee();
  int getBrewStartHour();
  bool getCurrentlyBrewing();

  void setBrewSchedule(int coffee_hr, int coffee_min); // set time to brew and sceduled_coffee=true 
  void setPowerPressed(int coffee_pressed_h, int coffee_pressed_m, bool power_pressed);
  void setFilterOpen(bool opened_filter);
  void setFillingCoffee(bool filling_coffee);
  void setFilterState(bool new_filter);
  void setCurrentlyBrewing(bool currently_brewing);
};