#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>


Adafruit_ADXL345_Unified accel;
//Adafruit_ADXL345_Unified accel2; 

//dedos
const int DEDO_1=50; //p48
const int DEDO_2=48;//p46
const int DEDO_3=46;//p44
const int DEDO_4=44; //p50
int dedosAtivos [5] = {9,9,9,9,9};

//accel 
float AccelMinX = 0;
float AccelMaxX = 0;
float AccelMinY = 0;
float AccelMaxY = 0;
float AccelMinZ = 0;
float AccelMaxZ = 0;
int casa=0;
int dedo=0;
int newCasa; 
int signalCheck=1;
int signalCheck2=1;


char sine[] = {125, 148, 171, 194, 209, 230, 252, 255,
253, 244, 235, 223, 207, 184, 169};

char sound[] = {0, 22, 44, 64, 82, 98, 111, 120, 126, 128, 126, 120, 111, 98, 82, 64, 44, 22, 0, -22, -44, -64, -82, -98, -111, -120, -126, -128, -126, -120, -111, -98, -82, -64, -44, -22};

//a simple sine wave with 36 samples

long frequencyComparator = 160; //variable used to store analog input pin 0
int delay_STANDARD = 300; // 1 s

long  delay_t;

byte speakerpin = 3;  //audio playback on pin 3.  This can also be set to pin 11.

volatile byte waveindex = 0; //index variable for position in waveform array Sine[]
volatile byte currentvalue = 0;



void initialize_pin(){
  pinMode(DEDO_1, INPUT); 
  pinMode(DEDO_2, INPUT); 
  pinMode(DEDO_3, INPUT); 
  pinMode(DEDO_4, INPUT); 
//  pinMode(DEDO_5, INPUT); 
} 

void readLuvas(){
  dedosAtivos[0] = digitalRead(DEDO_1);
  dedosAtivos[1] = digitalRead(DEDO_2);
  dedosAtivos[2] = digitalRead(DEDO_3);
  dedosAtivos[3] = digitalRead(DEDO_4);
//  dedosAtivos[4] = digitalRead(DEDO_5);
}



void setup() {
              //LEMBRAR DESSE PARANAUE DE SERIAL
              Serial.begin(57600);
              
              /************************** PWM audio configuration ****************************/
              // Configures PWM on pins 3 and 11 to run at maximum speed, rather than the default
              // 500Hz, which is useless for audio output
              
              pinMode(3,OUTPUT); //Speaker on pin 3
              
              cli(); //disable interrupts while registers are configured
              
              bitSet(TCCR2A, WGM20);
              bitSet(TCCR2A, WGM21); //set Timer2 to fast PWM mode (doubles PWM frequency)
              
              bitSet(TCCR2B, CS20);
              bitClear(TCCR2B, CS21);
              bitClear(TCCR2B, CS22);
              /* set prescaler to /1 (no prescaling).  The timer will overflow every
              *  62.5nS * 256ticks = 16uS, giving a PWM frequency of 62,500Hz, I think.   */
              
              sei(); //enable interrupts now that registers have been set
              
              /************************* Timer 1 interrupt configuration *************************/
              
              cli(); //disable interrupts while registers are configured
              
              bitClear(TCCR1A, COM1A1);
              bitClear(TCCR1A, COM1A1);
              bitClear(TCCR1A, COM1A1);
              bitClear(TCCR1A, COM1A1);
              /* Normal port operation, pins disconnected from timer operation (breaking pwm).
              *  Should be set this way by default, anyway. */
              
              bitClear(TCCR1A, WGM10);
              bitClear(TCCR1A, WGM11);
              bitSet(TCCR1B, WGM12);
              bitClear(TCCR1B, WGM13);
              /* Mode 4, CTC with TOP set by register OCR1A.  Allows us to set variable timing for
              *  the interrupt by writing new values to OCR1A. */
              
              bitClear(TCCR1B, CS10);
              bitSet(TCCR1B, CS11);
              bitClear(TCCR1B, CS12);
              /* set the clock prescaler to /8.  Since the processor ticks every 62.5ns, the timer
              *  will increment every .5uS.  Timer 1 is a 16-bit timer, so the maximum value is 65536,
              *  Giving us a theoretical range of .5us-32.7mS.  There are 48 samples, so the
              *  theoretical frequency range is 41.7KHz - .635Hz, which neatly covers the audio
              *  spectrum of 20KHz-20Hz.  Theoretical, because I wouldn't recommend actually calling
              *  the Timer1 interrupt every .5uS smile emoticon  */
              
              bitClear(TCCR1C, FOC1A);
              bitClear(TCCR1C, FOC1B);
              /* Disable Force Output Compare for Channels A and B, whatever that is.
              *  Should be set this way by default anyway. */
              
              OCR1A = 160;
              /* Initializes Output Compare Register A at 160, so a match will be generated every
              *  62.5nS * 8 * 160 = 80uS, for a 1/(80uS*48) = 260Hz tone. */
              
              bitClear(TIMSK1, ICIE1); //disable input capture interrupt
              bitClear(TIMSK1, OCIE1B); //disable Output Compare B Match Interrupt
              bitSet(TIMSK1, OCIE1A); //enable Output Compare A Match Interrupt
              bitClear(TIMSK1, TOIE1); //disable Overflow Interrupt Enable
              
              sei(); //enable interrupts now that registers have been set


            initialize_pin();
            Wire.begin();
            accel = Adafruit_ADXL345_Unified(12345);
             // accel2 = Adafruit_ADXL345_Unified(0x53);
          
            /* Initialise the sensor */
            if(!accel.begin())
            {
              /* There was a problem detecting the ADXL345 ... check your connections */
              Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
              while(1);
            }


}//end setup()

ISR(TIMER1_COMPA_vect) {
    
    if (waveindex > 47) { //reset waveindex if it has reached the end of the array
      waveindex = 0;
      }
     analogWrite(speakerpin, (sound[waveindex] + 128));
    waveindex++;
    
    OCR1A = frequencyComparator;

} //end Timer1 ISR


long selectFrequency(int dedo, char casa){
  int baseFrequency = 440;
  switch(dedo){
  case 0:
    baseFrequency = 263;
  break;
  case 1:
    baseFrequency = 293;
  break;
  case 2:
    baseFrequency = 329;
  break;
  case 3:
    baseFrequency = 391;
  break;
  case 4:
    baseFrequency = 440;
  break;  
  }
  baseFrequency = baseFrequency*pow(2,casa);
  Serial.print("Base Freq: ");
  Serial.println(baseFrequency);
  double f = 15*62.5E-9 * 8;
  f = f * baseFrequency;
  return (long)1.0/f;
  
}


void loop()
{
     delay(20);


      //ler dedos
      readLuvas();
    
    /* Get a new sensor event */ 
    sensors_event_t accelEvent;  
    accel.getEvent(&accelEvent);
  //  sensors_event_t accel2Event;  
  //  accel2.getEvent(&accel2Event);
    
    if (accelEvent.acceleration.y > 12) AccelMinX = 1;
    if (accelEvent.acceleration.y < 12) AccelMinX = 0;
    accelEvent.acceleration.x=accelEvent.acceleration.x+3.22;
    if (accelEvent.acceleration.x < 3 && accelEvent.acceleration.x > -3){  
      newCasa=0;
    }
    else {
      newCasa=((int)accelEvent.acceleration.x*2)/11;
    }
    if(newCasa>0){
      signalCheck=1;
    }
    if(newCasa<0){
      signalCheck=0;
    }
    newCasa+=casa;
    if(signalCheck==signalCheck2){
      if(newCasa<=0)
        casa=0;
      else if(newCasa>=4)
        casa=4;
      else{
        casa=newCasa;
  //    delay(150);
      }
    }
    else{
      delay(200);
    }
    signalCheck2=signalCheck;
  //  Serial.print(casa);
 //     Serial.println();

      for(int i=0;i<4;i++){
    if(dedosAtivos[i] ==0){
 //     Serial.print("Dedo ");
 //     Serial.print(i);
 //     Serial.print(" Vai: ");
 //     Serial.print(dedosAtivos[i]);
      Serial.print("dedo * 2^casa");
      Serial.print(i*(pow(2,casa)));
      Serial.println();
      dedo=i;
   }
  }
  
//    if (accelEvent.acceleration.x > AccelMaxX) AccelMaxX = accelEvent.acceleration.x;
//    
//    if (accelEvent.acceleration.y < AccelMinY) AccelMinY = accelEvent.acceleration.y;
//    if (accelEvent.acceleration.y > AccelMaxY) AccelMaxY = accelEvent.acceleration.y;
//  
//    if (accelEvent.acceleration.z < AccelMinZ) AccelMinZ = accelEvent.acceleration.z;
//    if (accelEvent.acceleration.z > AccelMaxZ) AccelMaxZ = accelEvent.acceleration.z;
  
//    Serial.print("Accel Minimums: "); Serial.print(AccelMinX); Serial.print("  ");Serial.print(AccelMinY); Serial.print("  "); Serial.print(AccelMinZ); Serial.println();
  
//    Serial.print("Accel Minimums: "); Serial.print(AccelMinX); Serial.print("  ");

//////////    if(AccelMinX == 1){
//////////      Serial.print("palheta");
//////////      Serial.println();
//////////    }
    
    
//    Serial.print("Accel Atual: "); Serial.print(accelEvent.acceleration.x); Serial.print("  ");Serial.print(accelEvent.acceleration.y); Serial.print("  "); Serial.print(accelEvent.acceleration.z); Serial.println();
  
  
//  if (accelEvent.acceleration.x < 5){
//    Serial.print("Palheta!!!! ");
//  }
  
  //   Serial.print("Accel Atual: "); Serial.print(accel2Event.acceleration.x); Serial.print("  ");Serial.print(accel2Event.acceleration.y); Serial.print("  "); Serial.print(accel2Event.acceleration.z); Serial.println();
//    Serial.print("Accel Maximums: "); Serial.print(AccelMaxX); Serial.print("  ");Serial.print(AccelMaxY); Serial.print("  "); Serial.print(AccelMaxZ); Serial.println();
 
    while (Serial.available())
    {
      Serial.read();  // clear the input buffer
    }







  
  for(casa=0; casa<4;casa++){
      frequencyComparator = selectFrequency(dedo, casa);
      delay_t = millis();
      Serial.print("casa: ");
      Serial.print(casa);
      Serial.print(" Frequencia: ");
      Serial.println(frequencyComparator);
      delay(1000);  
  }
}
