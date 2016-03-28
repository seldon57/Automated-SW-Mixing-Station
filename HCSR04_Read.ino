int HCSR04Read(int trigPin, int echoPin){
  int i;
  int avg = 0;
  int mm;		    // Distance measured by the sensor in mm
  int duration;		// Duration between trig and echo

  for (i = 0; i < 5; i++){
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
	mm = (duration/2.00) / 2.91;
	avg = avg + mm;    //sum sensor value
  }
  avg = avg / 5;    // average
  return avg;
}

  
  
  
    
  