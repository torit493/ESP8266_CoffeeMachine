#include "CoffeeMachine.h"

bool CoffeeMachine::timeToBrew(){
  if (curr_hour == brew_hour && curr_min == brew_min && !currently_brewing && !filter_open && brewing_scheduled){
    currently_brewing = true;
    return true;
  }
  return false;
}

void CoffeeMachine::powerOn(){
  int curr_angle = zero_deg;
  while(curr_angle < power_deg){
    curr_angle += 1;
    write(zero_deg + curr_angle);
    delay(delay_time); // To chill on the pressing speed.
  }
  while(curr_angle > zero_deg){
    curr_angle -= 1;
    write(zero_deg + curr_angle);
    delay(delay_time); // To chill on the pressing speed.
  }
  power_on = true;
  new_filter = false;
}


void CoffeeMachine::fillCoffee(){
  write(coffee_screw_speed);
  delay(fill_time);
  write(coffee_zero_speed);
}

void CoffeeMachine::openFilter(int position){
  /** 
   * Opens filter to position 1 or 2 
   * 1 = filling position
   * 2 = fully open
   * sets filter open variable true in object when done
    */  
  if (!filter_open && !filter_moving){
    if (position == 1){
      moveTo(open_filter_steps);
    }else if (position == 2){
      moveTo(open_filter_fully_steps);      
    }
    filter_moving = true;
  }else if (distanceToGo() == 0 && filter_moving){
    filter_open = true;
    filter_moving = false;
    Serial.println("set filter_open=true and filter_moving=false");
  }else if (filter_moving){
    run();
    Serial.println(distanceToGo());
  }
}

void CoffeeMachine::closeFilter(){
  if (filter_open && !filter_moving){
    moveTo(close_filter_steps);
    filter_moving = true;
  }else if (distanceToGo() == 0 && filter_moving){
    filter_open = false;
    filter_moving = false;
    clean_filter = true;                                  // Need to pay attention to this. Convenient as when machine finishes it opens filter and sets it to dirty. When closing the filter, which needs to be done manually it will be put to clean again automatically.
    Serial.println("set filter_open=false and filter_moving=false");
  }else  if (filter_moving){
    run();
    Serial.println(distanceToGo());
  }
}

void CoffeeMachine::update(int curr_hour, int curr_min){ // Make sure this updates everything thats need to be updated with respect to time
  this->curr_hour = curr_hour;
  this->curr_min = curr_min;
  signed long seconds_passed = (curr_hour*3600 + curr_min*60) - (brew_hour*3600 + brew_min*60);
  if (seconds_passed > 0 && seconds_passed > brew_time){
    currently_brewing = false;
    power_on = false;
  }
}

void CoffeeMachine::resetMachine(){
  filter_open = false;
  currently_brewing = false;
  brew_hour = 7;
  brew_min = 0;
  brewing_scheduled = false;
  power_on = false;
  filter_moving = false;
  new_filter = true;
}

void CoffeeMachine::setupPowerServo(){
  write(zero_deg);
  delay(500);
  write(zero_deg);
  delay(500);
}

void CoffeeMachine::setBrewSchedule(int hour, int min){
  brew_hour = hour;
  brew_min = min;
  brewing_scheduled = true;
}

void CoffeeMachine::setStepperClosed(){
  setCurrentPosition(close_filter_steps);
  filter_open = false;
}

void CoffeeMachine::setStepperOpened(){
  setCurrentPosition(open_filter_fully_steps);
  filter_open = true;
}

int CoffeeMachine::getBrewMin(){
  return brew_min;
}

int CoffeeMachine::getBrewHour(){
  return brew_hour;
}

bool CoffeeMachine::getFilterOpen(){ // This could maybe return true if currentPosition() == open_filter_steps or false if !=? Would require one less member variable.
  return filter_open;
}

bool CoffeeMachine::getCurrentlyBrewing(){
  return currently_brewing;
}

bool CoffeeMachine::getBrewingScheduled(){
  return brewing_scheduled;
}

bool CoffeeMachine::getNewFilter(){
  return new_filter;
}

String CoffeeMachine::getMachineInfo(){
  String temp = "Coffee Filter: open_filter_fully_steps = " + String(open_filter_fully_steps) + ", " + "open_filter_steps = " + String(open_filter_steps) + ", " + "close_filter_steps = "  +  String(close_filter_steps) + "<br>";
  temp += "Power presser: zero_deg = " + String(zero_deg) +"deg, power_deg = " + String(power_deg) + "deg, delay_time = " + String(delay_time) + "ms.<br>";
  temp += "Archimedes screw: coffee_screw_zero_speed = " + String(coffee_zero_speed) + "deg, coffee_screw_speed = " + String(coffee_screw_speed) + "deg.<br>";
  temp += "Filling coffee: fill_time = " + String(fill_time) + "ms.<br>";
  temp += "Filter_open = "+ String(filter_open) + "power_on = " + String(power_on) + "deg, currently_brewing = " +  String(currently_brewing) + "brewing_scheduled = " + String(brewing_scheduled) + "filter_moving = " + String(filter_moving) + "new_filter = "  + String(new_filter) + ".<br>";
  temp += "Timer stuff: curr_hour = " + String(curr_hour) + "curr_min = " + String(curr_min) + "brew_hour = " + String(brew_hour) + "brew_min = " + String(brew_min) + ".<br>";
  return temp;
}