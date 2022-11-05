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

int CoffeeMachine::getBrewMin(){
  return brew_min;
}

int CoffeeMachine::getBrewHour(){
  return brew_hour;
}

bool CoffeeMachine::getFilterOpen(){
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