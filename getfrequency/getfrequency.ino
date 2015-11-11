  /************************** Ultrasound measurement ***************************/
unsigned int getfrequency() {
  
  pinMode(PULSEPIN, OUTPUT);
  digitalWrite(PULSEPIN, LOW);
  delayMicroseconds(2);
  digitalWrite(PULSEPIN, HIGH);  
  delayMicroseconds(5);
  digitalWrite(PULSEPIN, LOW);
  pinMode(PULSEPIN, INPUT);
  //send a pulse to the Ping sensor to start measurement, and set pulsepin to input
  
  while ((PIND & B100) != B100) {
    //while pulsepin is low, do nothing
    }
    
  pulselength = micros();  
  //record the time of the beginning of the pulse
  
  while ((PIND & B100) != 0) {
      //if (micros() - pulselength > 5500) {
      //break;
      //give up if no match is found within range.
      //}
    }
  //while the pulsepin is high, do nothing.
    
  pulselength = micros() - pulselength;
  /*  measure the length of the pulse by subtracting the beginning from the end.
   *  I'm using this algorithm instead of pulseIn() because it is unaffected by the
   *  number of times the timer1 ISR is called  */

  if (pulselength > 160 && pulselength < 5000) {
    /* throw away measurements that are outside usable range-- both too short--
     *  the sensor becomes quite noisy under 2in, and too long, about 3'*/
    period = (((pulselength - 160) * 103) / 35) + 955;
    /* takes the incoming 160-5000 sensor range and maps it to a period range of
    *  955 to 15289 (deep C to Soprano C). */
    return period;
    }
  else {
    return periodold; 
    }
    
} //end getfrequency()
