/***********************************************************
* Arduino Theremin/Synth Code                              *
* Developed by Max Pierson                                 *
* Version Rev13 20 June 2009                               *
* Released under the WTFPL license, although I would       *
* appreciate Attribution and Share-Alike                   *
* See blog.wingedvictorydesign.com for the latest version. *
************************************************************/

#include "math.h"
//necessary for wah effect
#define PULSEPIN 2
//the I/O pin of the Ping unit
#define SPEAKERPIN 3
//audio playback on pin 3.
#define HEARTPIN 11
byte heartrate = 0; 
//used for the heartbeat effect

/*********************Analalog input variables ******************************/
int analog1 = 255; //volume
byte analog2 = 255; //waveform select
unsigned int analog3 = 0; 
float analog3buffer = 1; //wah effect
byte analog4; //decay
unsigned int decay = 255;
//Used to create an logarithmic decay of amplitude

int analog0offset = 375;
int analog1offset = 375;
int analog2offset = 564;
int analog3offset = 450;
int analog4offset = 369;
/* offsets that are subtracted from the analogReads to zero them out.  These will be
*  dynamically set in setup () */

/**************************Ultrasound Variables***************************/
unsigned long pulselength = 2000; 
//stores the incoming values from the Ping unit
int period = 3822;  
/* loaded into the timer1 OCR1A buffer to control the time between array steps, and
 *  thus the frequency of the output. */
int periodold = 3822;
// stores the prior value of period.  used to cue amplitude decay, etc.

/****************************Playback arrays and variables*************************/
#define SAMPLESIZE 32
char sine[] = {0,49,90,117,127,117,90,49,0,-49,-90,-117,-127,-117,-90,-49,0,49,90,117,127,117,90,49,0,-49,-90,-117,-127,-117,-90,-49};
//a simple sine wave with 32 samples
char overtones[] = {0,107,118,127,118,92,52,-8,17,46,-6,-36,-56,-67,-70,-80,0,80,70,67,56,36,6,-46,-17,8,-52,-92,-118,-127,-118,-107};
//the same wave with the overtones (and undertone) baked in
byte playbuffer[SAMPLESIZE];
//buffer used to store the composite output from the sampled tone and effects.
volatile byte waveindex = 0; //index variable for position in waveform array Sine[]
long playbufferelement = 0;



void setup() {

  pinMode(SPEAKERPIN, OUTPUT); //Set speaker pin to output
  pinMode(HEARTPIN, OUTPUT); //Red LED on pin 11
  
  configuredaregister();
  /* jump over to the configure_da_register tab, and do our manual register setup 
  *  (timer1 and timer2 config). */

  analogReference(EXTERNAL);
  
  delay(500); //give the photoresistors a moment to reach equilibrium
  analog1offset = analogRead(1) + 15;
  analog2offset = analogRead(2) + 15;
  analog3offset = analogRead(3) + 15;
  analog4offset = analogRead(4) + 15;
  //initial read of the photoresistors, used to offset the values during play
  
}//end setup()


void loop()
{
  heartrate--;
  analogWrite(HEARTPIN, heartrate);
  /* increments the 'heartbeat' LED on pin 11 downward.  Every 256 times through the main loop
  *  it will overflow to (underflow?) to 255. */
  
    
  periodold = (period + periodold) / 2;
  //average new values with old ones for better stability.
  period = getfrequency();
  //jump over to the getfrequency tab to measure the ultrasound distance and convert it to an appropriate range.

  /************************** Read the analog photoresistor inputs ***************************/
  analog1 = map(constrain(analogRead(1), analog1offset, 900), analog1offset, 900, 255, 0);
  analog2 = map(constrain(analogRead(2), analog2offset, 900), analog2offset, 900, 255, 0);
  analog3 = map(constrain(analogRead(3), analog3offset, 900), analog3offset, 900, 0, 500);
  analog4 = map(constrain(analogRead(4), analog4offset, 900), analog4offset, 900, 255, 230);
  /* take the analog inputs and scale them to a byte datatype.  Note that there's some dead space at the top
   *  and bottom of the sensor values-- so that the sensor stays at "off" or goes to "100%" at the end of travel. */

  /******************************* Apply volume and effects **********************************/
  if (abs(periodold - period) > 25) {
    decay = 255;
    }
  else {
    decay = ((decay * analog4) / 255); 
    }
  //if a new tone is produced, reset decay to full, otherwise increment it downward. 
  analog3buffer = float(analog3) / 200;
  //convert sensor 3 to a floating point number in the range of pi/16 to pi/4
      
  for (byte i = 0; i < SAMPLESIZE ; i++) {
    playbufferelement = (analog2*sine[i] + (255-analog2)*overtones[i]) / 255; 
    /* waveform selection-- as analog2 ranges from 0-255 it changes the tone from
    *  a pure sine wave to a more complex wavetype. */
    playbufferelement = playbufferelement*cos(analog3buffer*i);
    /* Wah effect-- creates a variable comb filter.  Note that as cos(0) = 1, when the input
    *  from the sensor is 0, the waveform is unchanged */
    playbufferelement = playbufferelement * analog1 * decay / 65025; 
    //volume and decay.  
    playbuffer[i] = playbufferelement + 128;
    //bias the values back to 0-255 for PWM output.
  }
  
}//end loop()




ISR(TIMER1_COMPA_vect) {
  /* timer1 ISR.  Every time it is called it sets the speaker pin 
   *  to the next value in playbuffer[].  frequency modulation is done by changing
   *  the timing between successive calls of this function, e.g. for a 1KHz tone,
   *  set the timing so that it runs through playbuffer[] 1000 times a second. */

  if (waveindex > SAMPLESIZE - 1) { //reset waveindex if it has reached the end of the array
    waveindex = 0;
    }
  OCR2B = playbuffer[waveindex]; 
  /* Set timer 2 PWM duty cycle to next value in array.  OCR2B is the register 
  *  corresponding to duty cycle for PWM pin 3. */
  waveindex++; //increment array index

  OCR1A = period; //set timer 1 compare match to new value of period.

} //end Timer1 ISR











