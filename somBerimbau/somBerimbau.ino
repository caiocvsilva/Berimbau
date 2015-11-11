int waveForm[] = {0,84,106,99,113,127,107,79,71,51,1,-26,7,43,23,-17,-26,-23,-43,-60,-49,-44,-66,-63,0,63,66,44,49,60,43,23,26,17,-23,-43,-8,26,-1,-51,-71,-79,-107,-127,-113,-99,-106,-84};

int waveVolume[] = {0, 22, 44, 64, 82, 98, 111, 120, 126, 128, 126, 120, 111, 98, 82, 64, 44, 22, 0, -22, -44, -64, -82, -98, -111, -120, -126, -128, -126, -120, -111, -98, -82, -64, -44, -22};
//a simple sine wave with 36 samples

int frequencies[] = { 39, 42, 44, 47,
50, 52, 56, 59, 63, 66, 70, 74, 79,
84, 89, 94, 100, 105, 112, 118, 126,
133, 141, 149};

int SPEAKER_PIN = 3;  //audio playback on pin 3.  This can also be set to pin 11.

volatile byte waveIndex = 0; //index variable for position in waveform array Sine[]
volatile byte currentvalue = 0;
const int SAMPLESIZE = 47;

int playbuffer[SAMPLESIZE];
long playbuffer_element;


byte frequency  = 160;
float volume = 0;
long duration;


void configurarRegistradores(){
  /// Configura os registradores
      
          
      /************************** PWM audio configuration ****************************/
      // Configures PWM on pins 3 and 11 to run at maximum speed, rather than the default
      // 500Hz, which is useless for audio output
      
      pinMode(SPEAKER_PIN,OUTPUT); //Speaker on pin 3
      
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
}


void setup() {

      Serial.begin(57600);
      configurarRegistradores();     

      duration = millis();
}//end setup()

 

//Define o timer 1. -> Isso que toca o som.

ISR(TIMER1_COMPA_vect) {
    /* timer1 ISR.  Every time it is called it sets
    *  speakerpin to the next value in Sine[].  frequency modulation is done by changing
    *  the timing between successive calls of this function, e.g. for a 1KHz tone,
    *  set the timing so that it runs through Sine[] 1000 times a second. */
    
    if (waveIndex > SAMPLESIZE - 1) { //reset waveIndex if it has reached the end of the array
      waveIndex = 0;
      }
    
      analogWrite(SPEAKER_PIN, waveVolume[waveIndex]);
     
    waveIndex++;
    
    OCR1A = frequency ;  // muda a frequencia da onda
} //end Timer1 ISR


int changeDedo = 0;
void loop()
{
//      Serial.println(666);
 
    Serial.print("Volume ");
    Serial.print(volume);
    Serial.print("  Wave: ");
    Serial.print(waveVolume[waveIndex]);
    Serial.print(" waveIndex: ");
    Serial.println(waveIndex);
    Serial.print(" Duration: ");
    Serial.println(millis() - duration);
    
       // Update the volume
       volume = 0.20;
     
       if (volume < 0) {
        volume = 0.0;
       }
        
      
       // Check that volume is not higher than 1
       if(volume > .95) {
        volume = .95;
       }
      
       // Update the volume in the waveform
       for(int i = 0; i < 47; i++) {
        waveVolume[i] = 
         ((waveForm[i] - 127) * volume) + 127;
       }
       changeDedo = (changeDedo + 1) % 6;
       // Set the frequency  according to the distance
       // between the two hands and the
       // fingers pressed
        frequency =  67;
               
       // Delay to avoid bouncing signals
       delay(50);
}
