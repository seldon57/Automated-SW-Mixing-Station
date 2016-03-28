
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