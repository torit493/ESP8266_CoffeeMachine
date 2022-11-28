#pragma once
#include <AccelStepper.h>
#include <Servo.h>
#include <String.h>

#define MAX_SPEED 500
#define ACCELERATION 100

class CoffeeMachine : public AccelStepper, public Servo{
  protected:
  uint8_t IN1, IN2, IN3, IN4, power_servo_p, coffee_servo_p, vib_p;
  // Parameters that should not change during operation:
  const int close_filter_steps      = 0;
  const int open_filter_fully_steps = 4500;
  const int open_filter_steps       = open_filter_fully_steps*2/3; //steps per rev is 2048
  const int zero_deg                = 0;                          // zero deg for power servo
  const int power_deg               = 95;                         // deg to press power
  const int coffee_screw_speed      = 40;                         // using a continuously rotating servo
  const int coffee_zero_speed       = 76;                         // ~76 means no rotation
  const int delay_time              = 10;                         // milliseconds. For slowing down servo movement.
  const int fill_time               = 10000;                      // milliseconds. 10000 ~15g- w small archimedes screw
  const int brew_time               = 1*3600;                     // seconds. The amount of second it takes to finish brewing.
  private:
  // Constantly changing parameters:
  bool filter_open, power_on, currently_brewing, brewing_scheduled, filter_moving, new_filter;
  int curr_hour, curr_min, brew_hour, brew_min;

  public:
  CoffeeMachine(uint8_t IN1, uint8_t IN2, uint8_t IN3, uint8_t IN4, uint8_t power_servo_p, uint8_t coffee_servo_p) : AccelStepper(HALF4WIRE, IN1, IN2, IN3, IN4)
  {
    setMaxSpeed(MAX_SPEED);
    setAcceleration(ACCELERATION);
    filter_open = false;
    currently_brewing = false;
    brew_hour = 7;
    brew_min = 0;
    brewing_scheduled = false;
    power_on = false;
    filter_moving = false;
    new_filter = true;
  }

  
  bool timeToBrew();
  void powerOn();
  void openFilter(int position);
  void closeFilter();
  void update(int curr_hour, int curr_min);
  void setupPowerServo();
  void fillCoffee();
  void resetMachine();

  void setBrewSchedule(int hour, int min);
  void setStepperClosed();
  void setStepperOpened();

  int getBrewMin();
  int getBrewHour();
  bool getFilterOpen();
  bool getCurrentlyBrewing();
  bool getBrewingScheduled();
  bool getNewFilter();

  String getMachineInfo();
};