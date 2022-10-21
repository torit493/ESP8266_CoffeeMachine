#include <AccelStepper.h>
#include <Servo.h>

#define MAX_SPEED 500
#define ACCELERATION 100

class CoffeeMachine : public AccelStepper, public Servo{
  protected:
  uint8_t IN1, IN2, IN3, IN4, power_servo_p, coffee_servo_p, vib_p;
  // Parameters that should not change during operation:
  const int open_filter_steps   = -1360;    //steps per rev is 2048
  const int close_filter_steps  = 0;
  const int zero_deg            = 0;
  const int power_deg           = 95;
  const int coffee_deg          = 90;
  const int delay_time          = 100;    // milliseconds. For slowing down servo movement.
  const int fill_time           = 3000;   // milliseconds. needs to be figured out later, maybe add one for half a litre and one for one cup?
  const int brew_time           = 1*3600; // seconds. The amount of second it takes to finish brewing.
  private:
  // Constantly changing parameters:
  bool filter_open, power_on, currently_brewing, brewing_scheduled, filter_moving;
  int curr_hour, curr_min, brew_hour, brew_min;

  public:
  CoffeeMachine(int IN1, int IN2, int IN3, int IN4, int power_servo_p, int coffee_servo_p, int vib_p) : AccelStepper(HALF4WIRE, IN1, IN2, IN3, IN4), Servo()
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
  }

  
  bool timeToBrew();
  void powerOn();
  void fillCoffee();
  void openFilter();
  void closeFilter();
  void update(int curr_hour, int curr_min);

  void setBrewSchedule(int hour, int min);

  int getVibPin();
  int getBrewMin();
  int getBrewHour();
  bool getFilterOpen();
  bool getCurrentlyBrewing();
  bool getBrewingScheduled();

};