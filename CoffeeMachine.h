#pragma once
#include <AccelStepper.h>
#include <Servo.h>

#define MAX_SPEED 500
#define ACCELERATION 100

class CoffeeMachine : public AccelStepper, public Servo{
  protected:
  uint8_t IN1, IN2, IN3, IN4, power_servo_p, coffee_servo_p, vib_p;
  // Parameters that should not change during operation:
  const int open_filter_steps   = -1500;    //steps per rev is 2048
  const int close_filter_steps  = 0;
  const int zero_deg            = 0;        // zero deg for power servo
  const int power_deg           = 95;       // deg to press power
  const int coffee_close_deg    = 0;        // deg for closed valve
  const int coffee_open_deg     = 180;      // deg when open valve
  const int delay_time          = 10;       // milliseconds. For slowing down servo movement.
  const int fill_time           = 3000;     // milliseconds. needs to be figured out later, maybe add one for half a litre and one for one cup?
  const int brew_time           = 1*3600;   // seconds. The amount of second it takes to finish brewing.
  private:
  // Constantly changing parameters:
  bool filter_open, power_on, currently_brewing, brewing_scheduled, filter_moving, new_filter;
  int curr_hour, curr_min, brew_hour, brew_min;

  public:
  CoffeeMachine(uint8_t IN1, uint8_t IN2, uint8_t IN3, uint8_t IN4, uint8_t power_servo_p, uint8_t coffee_servo_p, uint8_t vib_p) : AccelStepper(HALF4WIRE, IN1, IN2, IN3, IN4)
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
  void openFilter();
  void closeFilter();
  void closeCoffeeValve();
  void openCoffeeValve();
  void update(int curr_hour, int curr_min);
  void vibrate();
  void setupCoffeeServo();
  void setupPowerServo();
  void fillCoffee();
  void resetMachine();

  void setBrewSchedule(int hour, int min);

  int getVibPin();
  int getBrewMin();
  int getBrewHour();
  bool getFilterOpen();
  bool getCurrentlyBrewing();
  bool getBrewingScheduled();
  bool getNewFilter();
};