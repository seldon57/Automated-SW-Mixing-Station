int TankLevel(){

  //Measure the water level in the tank
  int WaterLevel = HCSR04Read(trigPin,echoPin);
  
  //Calculate the water level in the tank
  WaterLevel = TankHeight - WaterLevel;
  
  //14.2875 mm equals 1 gallon

  // Tank level state change while filling
  if ((level == LowLow) && (WaterLevel > 28.575))    // at 2 gallons
    level = Low;
  if ((level == Low) && (WaterLevel > 85.725))       // at 6 gallons
    level = Normal;
  if ((level == Normal) && (WaterLevel > 642.9375))  //at 45 gallons
    level = High;
  if ((level == High) && (WaterLevel > 785.8125))    // at 55 gallons
    level = HighHigh;

  //Tank level state change while emptying
  if ((level == HighHigh) && (WaterLevel < 714.375)) // at 50 gallons
    level = High;
  if ((level == High) && (WaterLevel < 628.65))      // at 44 gallons
    level = Normal;
  if ((level == Normal) && (WaterLevel < 71.4375))   // at 5 gallons
    level = Low;
  if ((level == Low) && (WaterLevel < 14.2875))      // at 1 gallon
    level = LowLow;

  //Tank level state change if HC-SR04 is not acting properly or fails
  if (WaterLevel > 2000)      // HC-SR04 is directly up against what it is measuring
    level = HighHigh;
  if (WaterLevel == 0)        // HC-SR04 has failed or is disconnected
    level = HighHigh;
    
  return level;
}