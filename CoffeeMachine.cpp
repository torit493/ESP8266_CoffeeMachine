#include "CoffeeMachine.h"

CoffeeMachine::CoffeeMachine(int coffee_hr, int coffee_min, bool scheduled_coffee){
  this->coffee_hr=coffee_hr;
  this->coffee_min=coffee_min;
  this->scheduled_coffee=scheduled_coffee;
  new_filter = false;
  opened_filter = false;
  power_pressed = false;
  filling_coffee = false;
  currently_brewing = false;
  power_pressed_h = -1; // Im thinking this is best? Cannot be mistaken and lets us know nothing has been pressed...
  power_pressed_m = -1;
}

bool CoffeeMachine::shouldBrewNow(int hr, int min){
  if (hr == coffee_hr && min == coffee_min && scheduled_coffee && new_filter){
    power_pressed_h = hr;
    power_pressed_m = min;
    currently_brewing = true;
    return true;
  }
  return false;
}

bool CoffeeMachine::isOn(int hr, int min){
  if (power_pressed){
    return true;
  }
}

void CoffeeMachine::update(int hr, int min){ // I want to say that power is not pressed anymore after one hour has passed from brew start.
  if (power_pressed && hr > coffee_hr){
    power_pressed = false;
  }
}

bool CoffeeMachine::allowFilterChange(){
  if (!currently_brewing){
    return true;
  }
}

void CoffeeMachine::setBrewSchedule(int coffee_hr, int coffee_min){
  this->coffee_hr=coffee_hr;
  this->coffee_min=coffee_min;
  scheduled_coffee=true;
}

void CoffeeMachine::setPowerPressed(int coffee_pressed_h, int coffee_pressed_m, bool power_pressed){
  this->power_pressed = power_pressed;
  this->power_pressed_h=power_pressed_h;
  this->power_pressed_m=power_pressed_m;
}

void CoffeeMachine::setFilterOpen(bool opened_filter){
  this->opened_filter = opened_filter;
}

bool CoffeeMachine::getFilterOpen(){
  return opened_filter;
}

void CoffeeMachine::setFillingCoffee(bool filling_coffee){
  this->filling_coffee=filling_coffee;
}

bool CoffeeMachine::getFillingCoffee(){
  return filling_coffee;
}

int CoffeeMachine::getCoffeeMinutes(){
  return coffee_min;
}

int CoffeeMachine::getCoffeeHours(){
  return coffee_hr;
}

void CoffeeMachine::setFilterState(bool new_filter){
  this->new_filter = new_filter;
}

bool CoffeeMachine::getFilterState(){
  return new_filter;
}

bool CoffeeMachine::getScheduledCoffee(){
  return scheduled_coffee;
}

int CoffeeMachine::getBrewStartHour(){
  return power_pressed_h;
}

bool CoffeeMachine::getCurrentlyBrewing(){
  return currently_brewing;
}

void CoffeeMachine::setCurrentlyBrewing(bool currently_brewing){
  this->currently_brewing = currently_brewing;
}