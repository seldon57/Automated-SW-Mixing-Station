#include <Time.h>
#include <TimeLib.h>

/*
 * created by seldon on RC
 * 
 * Automated Salt Water Mixing Station
 *
 * 55 Gallon Drum Level Monitoring Using Ultrasonic Sensor HC-SR04
 *
 * control using 8 relay module
 */
 

//IO Pins
const int PressureSwitchPin = 3;    // Pressure - ?? Jumper
const int Sens1_trigPin   = 4;      // Trig - Purple Jumper
const int Sens1_echoPin   = 5;      // Echo - Grey Jumper
const int Relay1Pin = 6;      	    // Not in use
const int Relay2Pin = 7;      	    // Not in use
const int Relay3Pin = 8;    	    // Not in use
const int Relay4Pin = 9;    	    // Not in use
const int Relay5Pin = 10;   	    // Blue
const int Relay6Pin = 11;   	    // Green
const int Relay7Pin = 12;   	    // Yellow
const int Relay8Pin = 13;   	    // Orange

int FeedSolenoidPin;
int FlushSolenoidPin;
int TankSolenoidPin;
int BoosterPumpPin;


int mm;				// Calculated distance in mm based on the speed of sound

// Define the number of samples to keep track of.  The higher the number,
// the more the readings will be smoothed, but the slower the output will
// respond to the input.  Using a constant rather than a normal variable lets
// use this value to determine the size of the readings array.
const int numReadings = 10;

int readIndex = 0;              // the index of the current reading
int readings[numReadings];     // the readings from the analog input
int sensor_total = 0;          // the running total
int sensor_average = 0;        // the average


int TankHeight = 850;	// Height of the tank in mm from the bottom to the sensor
int TankLevel = 0;			// Calculated height of the water in the tank

time_t initialtime = 0;
time_t currenttime = 0;

int InitialFlushTime = 300;
int FlushTime = 30;
int RunTime = 3600;

boolean PressureSwitch = false;	// bit to determine if the pressure switch is on

int TankLevelStatus = 0;
enum TankStatus {
  HighHigh, High, Normal, Low, LowLow
};
TankStatus level=HighHigh;

int RODIRunningStatus = 0;
enum RODIStatus {
  Off, InitialFlush, Flushing, Running
};
RODIStatus RODI=Off;
 
void setup() {
  //Serial Port begin
  Serial.begin (9600);
  
  //Define outputs
  pinMode(trigPin, OUTPUT);
  pinMode(Relay1Pin, OUTPUT);
  pinMode(Relay2Pin, OUTPUT);
  pinMode(Relay3Pin, OUTPUT);
  pinMode(Relay4Pin, OUTPUT);
  pinMode(Relay5Pin, OUTPUT);
  pinMode(Relay6Pin, OUTPUT);
  pinMode(Relay7Pin, OUTPUT);
  pinMode(Relay8Pin, OUTPUT);

  //Initialize Outputs
  digitalWrite(Relay1Pin, HIGH);
  digitalWrite(Relay2Pin, HIGH);
  digitalWrite(Relay3Pin, HIGH);
  digitalWrite(Relay4Pin, HIGH);
  digitalWrite(Relay5Pin, HIGH);
  digitalWrite(Relay6Pin, HIGH);
  digitalWrite(Relay7Pin, HIGH);
  digitalWrite(Relay8Pin, HIGH);
  
  //Set convienient naming scheme for outputs
  FeedSolenoidPin = Relay5Pin;
  FlushSolenoidPin = Relay6Pin;
  TankSolenoidPin = Relay7Pin;
  BoosterPumpPin = Relay8Pin;
  
  //Define inputs
  pinMode(echoPin, INPUT);
  pinMode(PressureSwitchPin, INPUT);
  digitalWrite(PressureSwitchPin, HIGH);      // turn on pullup resistor
  
  // initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
}
 
void loop(){
  
  TankLevelStatus = TankStatusRead();
  PressureSwitch = PressureSwitchRead();
  if (RODIRunningStatus == InitialFlush){
	RODIRunningStatus = RODIInitialFlush();
  }
  else if (RODIRunningStatus == Flushing){
	RODIRunningStatus = RODIFlushing();
  }
  else if (RODIRunningStatus == Running){
	RODIRunningStatus = RODIRunning();
  }
  else{
	RODIRunningStatus = RODIOff();
  }

  //Diagnostics out to serial
  Serial.print("Tank Level Status: ");
  if (TankLevelStatus == 0){
	Serial.print("HighHigh");
  }
  else if (TankLevelStatus == 1){
	Serial.print("High");
  }
  else if (TankLevelStatus == 2){
	Serial.print("Normal");
  }
  else if (TankLevelStatus == 3){
	Serial.print("Low");
  }
  else if (TankLevelStatus == 4){
	Serial.print("LowLow");
  }
  Serial.println();
  
  Serial.print("Pressure Switch Status: ");
  if (PressureSwitch == false){
	Serial.print("Off");
  }
  else if (PressureSwitch == true){
	Serial.print("On");
  }
  Serial.println();
  
  Serial.print("RODI Status: ");
  if (RODIRunningStatus == 0){
	Serial.print("Off");
  }
  else if (RODIRunningStatus == 1){
	Serial.print("Initial Flush");
  }
  else if (RODIRunningStatus == 2){
	Serial.print("Flushing");
  }
  else if (RODIRunningStatus == 3){
	Serial.print("Running");
  }
  Serial.println();
  Serial.println();
  
}


int PressureSwitchRead(){
  if(digitalRead(PressureSwitchPin) == LOW){
  delay(100);         // delay to debounce switch
  PressureSwitch = true;  
  }
  else{
  PressureSwitch = false;
  }
  return PressureSwitch;
}

int TankStatusRead(){

  //Calculate the water level in the tank
  TankLevel = TankHeight - mm;
  
  //14.2875 mm equals 1 gallon

  // Tank level state change while filling
  if ((level == LowLow) && (TankLevel > 28.575))    // at 2 gallons
    level = Low;
  if ((level == Low) && (TankLevel > 85.725))       // at 6 gallons
    level = Normal;
  if ((level == Normal) && (TankLevel > 642.9375))  //at 45 gallons
    level = High;
  if ((level == High) && (TankLevel > 785.8125))    // at 55 gallons
    level = HighHigh;

  //Tank level state change while emptying
  if ((level == HighHigh) && (TankLevel < 714.375)) // at 50 gallons
    level = High;
  if ((level == High) && (TankLevel < 628.65))      // at 44 gallons
    level = Normal;
  if ((level == Normal) && (TankLevel < 71.4375))   // at 5 gallons
    level = Low;
  if ((level == Low) && (TankLevel < 14.2875))      // at 1 gallon
    level = LowLow;

  //Tank level state change if HC-SR04 is not acting properly or fails
  if (mm > 2000)      // HC-SR04 is directly up against what it is measuring
    level = HighHigh;
  if (mm == 0)      // HC-SR04 has failed or is disconnected
    level = HighHigh;
    
  return level;
}

int RODIOff(){			
  while(RODI == Off)
  {
    digitalWrite(FeedSolenoidPin, HIGH);
    digitalWrite(FlushSolenoidPin, HIGH);
    digitalWrite(TankSolenoidPin, HIGH);
    digitalWrite(BoosterPumpPin, HIGH);
    TankLevelStatus = TankStatusRead();
    if(TankLevelStatus == LowLow)
      RODI = InitialFlush;
    Serial.print("RODI Status: Off");
    Serial.println();
    Serial.println();
  }
  return RODI;
}

int RODIInitialFlush(){
  initialtime = now();
  currenttime = now() - initialtime;
  while(currenttime <= InitialFlushTime && RODI == InitialFlush)
  {
  digitalWrite(FeedSolenoidPin, LOW);
  digitalWrite(FlushSolenoidPin, LOW);
  digitalWrite(TankSolenoidPin, LOW);
  digitalWrite(BoosterPumpPin, LOW);
  TankLevelStatus = TankStatusRead();
  PressureSwitch = PressureSwitchRead();
  if(TankLevelStatus == High)
    RODI = Off; 
  if(TankLevelStatus == HighHigh)
    RODI = Off;
  if(PressureSwitch == true)
    RODI = Off;
  currenttime = now() - initialtime;
  Serial.print("RODI Status: InitialFlush");
  Serial.println();
  Serial.print(currenttime);
  Serial.println();
  Serial.println();
  }
  if(currenttime >= InitialFlushTime)
    RODI = Running;
  return RODI;
}

int RODIFlushing(){
  initialtime = now();
  currenttime = now() - initialtime;
  while(currenttime <= FlushTime && RODI == Flushing)
  {
  digitalWrite(FeedSolenoidPin, LOW);
  digitalWrite(FlushSolenoidPin, LOW);
  digitalWrite(TankSolenoidPin, LOW);
  digitalWrite(BoosterPumpPin, LOW);
  TankLevelStatus = TankStatusRead();
  PressureSwitch = PressureSwitchRead();
  if(TankLevelStatus == High)
    RODI = Off; 
  if(TankLevelStatus == HighHigh)
    RODI = Off;
  if(PressureSwitch == true)
    RODI = Off;
  currenttime = now() - initialtime;
  Serial.print("RODI Status: Flushing");
  Serial.println();
  Serial.print(currenttime);
  Serial.println();
  Serial.println();
  }
  if(currenttime >= FlushTime)
    RODI = Running;
  return RODI;
}

int RODIRunning(){
  initialtime = now();
  currenttime = now() - initialtime;
  while(currenttime <= RunTime && RODI == Running)
  {
  digitalWrite(FeedSolenoidPin, LOW);
  digitalWrite(FlushSolenoidPin, HIGH);
  digitalWrite(TankSolenoidPin, LOW);
  digitalWrite(BoosterPumpPin, LOW);
  TankLevelStatus = TankStatusRead();
  PressureSwitch = PressureSwitchRead();
  if(TankLevelStatus == High)
    RODI = Off; 
  if(TankLevelStatus == HighHigh)
    RODI = Off;
  if(PressureSwitch == true)
    RODI = Off;
  currenttime = now() - initialtime;
  Serial.print("RODI Status: Running");
  Serial.println();
  Serial.print(currenttime);
  Serial.println();
  Serial.println();
  }
  if(currenttime >= RunTime)
    RODI = Flushing;
  return RODI;
}

int Smoothing(){
  // subtract the last reading:
  sensor_total = sensor_total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = HCSR04Read(Sens1_trigPin, Sens1_echoPin); // this should be the TankHeight reading rather than a raw analog input
  // add the reading to the total:
  sensor_total = sensor_total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  sensor_average = sensor_total / numReadings;
  
  return sensor_average;
}

int HCSR04Read(int trigPin, int echoPin){

  int duration;		// Duration between trig and echo
  int distance;		// Distance measured by the sensor in mm
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
 
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  duration = pulseIn(echoPin, HIGH);
 
  // convert the time into a distance
  distance = (duration/2.00) / 2.91;
    
  return distance;
}
