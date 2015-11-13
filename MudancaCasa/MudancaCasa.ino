#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>


 
/* Assign a unique ID to this sensor at the same time */
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
int newCasa; 
int signalCheck=1;
int signalCheck2=1;


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

void setup(void) 
{
  Serial.begin(9600);
  initialize_pin();
  Serial.println("ADXL345 Accelerometer Calibration"); 
  Serial.println("");

  Wire.begin();

  // Create an instance of the accelerometer on the default address (0x1D)
  accel = Adafruit_ADXL345_Unified(12345);
 // accel2 = Adafruit_ADXL345_Unified(0x53);
  
  /* Initialise the sensor */
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while(1);
  }
}
 
void loop(void)
{
//    Serial.println("Type key when ready..."); 
//    while (!Serial.available()){}  // wait for a character
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
    Serial.print(casa);
      Serial.println();

      for(int i=0;i<4;i++){
    if(dedosAtivos[i] ==0){
      Serial.print("Dedo ");
      Serial.print(i);
      Serial.print(" Vai: ");
      Serial.print(dedosAtivos[i]);
      Serial.print("dedo * 2^casa");
      Serial.print(i*(pow(2,casa)));
      Serial.println();
      
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
}
